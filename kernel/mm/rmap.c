


#include <linux/mm.h>
#include <linux/pagemap.h>
#include <linux/swap.h>
#include <linux/swapops.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/ksm.h>
#include <linux/rmap.h>
#include <linux/rcupdate.h>
#include <linux/module.h>
#include <linux/memcontrol.h>
#include <linux/mmu_notifier.h>
#include <linux/migrate.h>

#include <asm/tlbflush.h>

#include "internal.h"

static struct kmem_cache *anon_vma_cachep;
static struct kmem_cache *anon_vma_chain_cachep;

static inline struct anon_vma *anon_vma_alloc(void)
{
	return kmem_cache_alloc(anon_vma_cachep, GFP_KERNEL);
}

void anon_vma_free(struct anon_vma *anon_vma)
{
	kmem_cache_free(anon_vma_cachep, anon_vma);
}

static inline struct anon_vma_chain *anon_vma_chain_alloc(void)
{
	return kmem_cache_alloc(anon_vma_chain_cachep, GFP_KERNEL);
}

void anon_vma_chain_free(struct anon_vma_chain *anon_vma_chain)
{
	kmem_cache_free(anon_vma_chain_cachep, anon_vma_chain);
}

int anon_vma_prepare(struct vm_area_struct *vma)
{
	struct anon_vma *anon_vma = vma->anon_vma;
	struct anon_vma_chain *avc;

	might_sleep();
	if (unlikely(!anon_vma)) {
		struct mm_struct *mm = vma->vm_mm;
		struct anon_vma *allocated;

		avc = anon_vma_chain_alloc();
		if (!avc)
			goto out_enomem;

		anon_vma = find_mergeable_anon_vma(vma);
		allocated = NULL;
		if (!anon_vma) {
			anon_vma = anon_vma_alloc();
			if (unlikely(!anon_vma))
				goto out_enomem_free_avc;
			allocated = anon_vma;
		}

		spin_lock(&anon_vma->lock);
		/* page_table_lock to protect against threads */
		spin_lock(&mm->page_table_lock);
		if (likely(!vma->anon_vma)) {
			vma->anon_vma = anon_vma;
			avc->anon_vma = anon_vma;
			avc->vma = vma;
			list_add(&avc->same_vma, &vma->anon_vma_chain);
			list_add(&avc->same_anon_vma, &anon_vma->head);
			allocated = NULL;
			avc = NULL;
		}
		spin_unlock(&mm->page_table_lock);
		spin_unlock(&anon_vma->lock);

		if (unlikely(allocated))
			anon_vma_free(allocated);
		if (unlikely(avc))
			anon_vma_chain_free(avc);
	}
	return 0;

 out_enomem_free_avc:
	anon_vma_chain_free(avc);
 out_enomem:
	return -ENOMEM;
}

static void anon_vma_chain_link(struct vm_area_struct *vma,
				struct anon_vma_chain *avc,
				struct anon_vma *anon_vma)
{
	avc->vma = vma;
	avc->anon_vma = anon_vma;
	list_add(&avc->same_vma, &vma->anon_vma_chain);

	spin_lock(&anon_vma->lock);
	list_add_tail(&avc->same_anon_vma, &anon_vma->head);
	spin_unlock(&anon_vma->lock);
}

int anon_vma_clone(struct vm_area_struct *dst, struct vm_area_struct *src)
{
	struct anon_vma_chain *avc, *pavc;

	list_for_each_entry_reverse(pavc, &src->anon_vma_chain, same_vma) {
		avc = anon_vma_chain_alloc();
		if (!avc)
			goto enomem_failure;
		anon_vma_chain_link(dst, avc, pavc->anon_vma);
	}
	return 0;

 enomem_failure:
	unlink_anon_vmas(dst);
	return -ENOMEM;
}

int anon_vma_fork(struct vm_area_struct *vma, struct vm_area_struct *pvma)
{
	struct anon_vma_chain *avc;
	struct anon_vma *anon_vma;

	/* Don't bother if the parent process has no anon_vma here. */
	if (!pvma->anon_vma)
		return 0;

	/*
	 * First, attach the new VMA to the parent VMA's anon_vmas,
	 * so rmap can find non-COWed pages in child processes.
	 */
	if (anon_vma_clone(vma, pvma))
		return -ENOMEM;

	/* Then add our own anon_vma. */
	anon_vma = anon_vma_alloc();
	if (!anon_vma)
		goto out_error;
	avc = anon_vma_chain_alloc();
	if (!avc)
		goto out_error_free_anon_vma;
	anon_vma_chain_link(vma, avc, anon_vma);
	/* Mark this anon_vma as the one where our new (COWed) pages go. */
	vma->anon_vma = anon_vma;

	return 0;

 out_error_free_anon_vma:
	anon_vma_free(anon_vma);
 out_error:
	unlink_anon_vmas(vma);
	return -ENOMEM;
}

static void anon_vma_unlink(struct anon_vma_chain *anon_vma_chain)
{
	struct anon_vma *anon_vma = anon_vma_chain->anon_vma;
	int empty;

	/* If anon_vma_fork fails, we can get an empty anon_vma_chain. */
	if (!anon_vma)
		return;

	spin_lock(&anon_vma->lock);
	list_del(&anon_vma_chain->same_anon_vma);

	/* We must garbage collect the anon_vma if it's empty */
	empty = list_empty(&anon_vma->head) && !anonvma_external_refcount(anon_vma);
	spin_unlock(&anon_vma->lock);

	if (empty)
		anon_vma_free(anon_vma);
}

void unlink_anon_vmas(struct vm_area_struct *vma)
{
	struct anon_vma_chain *avc, *next;

	/* Unlink each anon_vma chained to the VMA. */
	list_for_each_entry_safe(avc, next, &vma->anon_vma_chain, same_vma) {
		anon_vma_unlink(avc);
		list_del(&avc->same_vma);
		anon_vma_chain_free(avc);
	}
}

static void anon_vma_ctor(void *data)
{
	struct anon_vma *anon_vma = data;

	spin_lock_init(&anon_vma->lock);
	anonvma_external_refcount_init(anon_vma);
	INIT_LIST_HEAD(&anon_vma->head);
}

void __init anon_vma_init(void)
{
	anon_vma_cachep = kmem_cache_create("anon_vma", sizeof(struct anon_vma),
			0, SLAB_DESTROY_BY_RCU|SLAB_PANIC, anon_vma_ctor);
	anon_vma_chain_cachep = KMEM_CACHE(anon_vma_chain, SLAB_PANIC);
}

struct anon_vma *page_lock_anon_vma(struct page *page)
{
	struct anon_vma *anon_vma;
	unsigned long anon_mapping;

	rcu_read_lock();
	anon_mapping = (unsigned long) ACCESS_ONCE(page->mapping);
	if ((anon_mapping & PAGE_MAPPING_FLAGS) != PAGE_MAPPING_ANON)
		goto out;
	if (!page_mapped(page))
		goto out;

	anon_vma = (struct anon_vma *) (anon_mapping - PAGE_MAPPING_ANON);
	spin_lock(&anon_vma->lock);
	return anon_vma;
out:
	rcu_read_unlock();
	return NULL;
}

void page_unlock_anon_vma(struct anon_vma *anon_vma)
{
	spin_unlock(&anon_vma->lock);
	rcu_read_unlock();
}

static inline unsigned long
vma_address(struct page *page, struct vm_area_struct *vma)
{
	pgoff_t pgoff = page->index << (PAGE_CACHE_SHIFT - PAGE_SHIFT);
	unsigned long address;

	address = vma->vm_start + ((pgoff - vma->vm_pgoff) << PAGE_SHIFT);
	if (unlikely(address < vma->vm_start || address >= vma->vm_end)) {
		/* page should be within @vma mapping range */
		return -EFAULT;
	}
	return address;
}

unsigned long page_address_in_vma(struct page *page, struct vm_area_struct *vma)
{
	if (PageAnon(page))
		;
	else if (page->mapping && !(vma->vm_flags & VM_NONLINEAR)) {
		if (!vma->vm_file ||
		    vma->vm_file->f_mapping != page->mapping)
			return -EFAULT;
	} else
		return -EFAULT;
	return vma_address(page, vma);
}

pte_t *page_check_address(struct page *page, struct mm_struct *mm,
			  unsigned long address, spinlock_t **ptlp, int sync)
{
	pgd_t *pgd;
	pud_t *pud;
	pmd_t *pmd;
	pte_t *pte;
	spinlock_t *ptl;

	pgd = pgd_offset(mm, address);
	if (!pgd_present(*pgd))
		return NULL;

	pud = pud_offset(pgd, address);
	if (!pud_present(*pud))
		return NULL;

	pmd = pmd_offset(pud, address);
	if (!pmd_present(*pmd))
		return NULL;

	pte = pte_offset_map(pmd, address);
	/* Make a quick check before getting the lock */
	if (!sync && !pte_present(*pte)) {
		pte_unmap(pte);
		return NULL;
	}

	ptl = pte_lockptr(mm, pmd);
	spin_lock(ptl);
	if (pte_present(*pte) && page_to_pfn(page) == pte_pfn(*pte)) {
		*ptlp = ptl;
		return pte;
	}
	pte_unmap_unlock(pte, ptl);
	return NULL;
}

int page_mapped_in_vma(struct page *page, struct vm_area_struct *vma)
{
	unsigned long address;
	pte_t *pte;
	spinlock_t *ptl;

	address = vma_address(page, vma);
	if (address == -EFAULT)		/* out of vma range */
		return 0;
	pte = page_check_address(page, vma->vm_mm, address, &ptl, 1);
	if (!pte)			/* the page is not in this mm */
		return 0;
	pte_unmap_unlock(pte, ptl);

	return 1;
}

int page_referenced_one(struct page *page, struct vm_area_struct *vma,
			unsigned long address, unsigned int *mapcount,
			unsigned long *vm_flags)
{
	struct mm_struct *mm = vma->vm_mm;
	pte_t *pte;
	spinlock_t *ptl;
	int referenced = 0;

	pte = page_check_address(page, mm, address, &ptl, 0);
	if (!pte)
		goto out;

	/*
	 * Don't want to elevate referenced for mlocked page that gets this far,
	 * in order that it progresses to try_to_unmap and is moved to the
	 * unevictable list.
	 */
	if (vma->vm_flags & VM_LOCKED) {
		*mapcount = 1;	/* break early from loop */
		*vm_flags |= VM_LOCKED;
		goto out_unmap;
	}

	if (ptep_clear_flush_young_notify(vma, address, pte)) {
		/*
		 * Don't treat a reference through a sequentially read
		 * mapping as such.  If the page has been used in
		 * another mapping, we will catch it; if this other
		 * mapping is already gone, the unmap path will have
		 * set PG_referenced or activated the page.
		 */
		if (likely(!VM_SequentialReadHint(vma)))
			referenced++;
	}

	/* Pretend the page is referenced if the task has the
	   swap token and is in the middle of a page fault. */
	if (mm != current->mm && has_swap_token(mm) &&
			rwsem_is_locked(&mm->mmap_sem))
		referenced++;

out_unmap:
	(*mapcount)--;
	pte_unmap_unlock(pte, ptl);

	if (referenced)
		*vm_flags |= vma->vm_flags;
out:
	return referenced;
}

static int page_referenced_anon(struct page *page,
				struct mem_cgroup *mem_cont,
				unsigned long *vm_flags)
{
	unsigned int mapcount;
	struct anon_vma *anon_vma;
	struct anon_vma_chain *avc;
	int referenced = 0;

	anon_vma = page_lock_anon_vma(page);
	if (!anon_vma)
		return referenced;

	mapcount = page_mapcount(page);
	list_for_each_entry(avc, &anon_vma->head, same_anon_vma) {
		struct vm_area_struct *vma = avc->vma;
		unsigned long address = vma_address(page, vma);
		if (address == -EFAULT)
			continue;
		/*
		 * If we are reclaiming on behalf of a cgroup, skip
		 * counting on behalf of references from different
		 * cgroups
		 */
		if (mem_cont && !mm_match_cgroup(vma->vm_mm, mem_cont))
			continue;
		referenced += page_referenced_one(page, vma, address,
						  &mapcount, vm_flags);
		if (!mapcount)
			break;
	}

	page_unlock_anon_vma(anon_vma);
	return referenced;
}

static int page_referenced_file(struct page *page,
				struct mem_cgroup *mem_cont,
				unsigned long *vm_flags)
{
	unsigned int mapcount;
	struct address_space *mapping = page->mapping;
	pgoff_t pgoff = page->index << (PAGE_CACHE_SHIFT - PAGE_SHIFT);
	struct vm_area_struct *vma;
	struct prio_tree_iter iter;
	int referenced = 0;

	/*
	 * The caller's checks on page->mapping and !PageAnon have made
	 * sure that this is a file page: the check for page->mapping
	 * excludes the case just before it gets set on an anon page.
	 */
	BUG_ON(PageAnon(page));

	/*
	 * The page lock not only makes sure that page->mapping cannot
	 * suddenly be NULLified by truncation, it makes sure that the
	 * structure at mapping cannot be freed and reused yet,
	 * so we can safely take mapping->i_mmap_lock.
	 */
	BUG_ON(!PageLocked(page));

	spin_lock(&mapping->i_mmap_lock);

	/*
	 * i_mmap_lock does not stabilize mapcount at all, but mapcount
	 * is more likely to be accurate if we note it after spinning.
	 */
	mapcount = page_mapcount(page);

	vma_prio_tree_foreach(vma, &iter, &mapping->i_mmap, pgoff, pgoff) {
		unsigned long address = vma_address(page, vma);
		if (address == -EFAULT)
			continue;
		/*
		 * If we are reclaiming on behalf of a cgroup, skip
		 * counting on behalf of references from different
		 * cgroups
		 */
		if (mem_cont && !mm_match_cgroup(vma->vm_mm, mem_cont))
			continue;
		referenced += page_referenced_one(page, vma, address,
						  &mapcount, vm_flags);
		if (!mapcount)
			break;
	}

	spin_unlock(&mapping->i_mmap_lock);
	return referenced;
}

int page_referenced(struct page *page,
		    int is_locked,
		    struct mem_cgroup *mem_cont,
		    unsigned long *vm_flags)
{
	int referenced = 0;
	int we_locked = 0;

	*vm_flags = 0;
	if (page_mapped(page) && page_rmapping(page)) {
		if (!is_locked && (!PageAnon(page) || PageKsm(page))) {
			we_locked = trylock_page(page);
			if (!we_locked) {
				referenced++;
				goto out;
			}
		}
		if (unlikely(PageKsm(page)))
			referenced += page_referenced_ksm(page, mem_cont,
								vm_flags);
		else if (PageAnon(page))
			referenced += page_referenced_anon(page, mem_cont,
								vm_flags);
		else if (page->mapping)
			referenced += page_referenced_file(page, mem_cont,
								vm_flags);
		if (we_locked)
			unlock_page(page);
	}
out:
	if (page_test_and_clear_young(page))
		referenced++;

	return referenced;
}

static int page_mkclean_one(struct page *page, struct vm_area_struct *vma,
			    unsigned long address)
{
	struct mm_struct *mm = vma->vm_mm;
	pte_t *pte;
	spinlock_t *ptl;
	int ret = 0;

	pte = page_check_address(page, mm, address, &ptl, 1);
	if (!pte)
		goto out;

	if (pte_dirty(*pte) || pte_write(*pte)) {
		pte_t entry;

		flush_cache_page(vma, address, pte_pfn(*pte));
		entry = ptep_clear_flush_notify(vma, address, pte);
		entry = pte_wrprotect(entry);
		entry = pte_mkclean(entry);
		set_pte_at(mm, address, pte, entry);
		ret = 1;
	}

	pte_unmap_unlock(pte, ptl);
out:
	return ret;
}

static int page_mkclean_file(struct address_space *mapping, struct page *page)
{
	pgoff_t pgoff = page->index << (PAGE_CACHE_SHIFT - PAGE_SHIFT);
	struct vm_area_struct *vma;
	struct prio_tree_iter iter;
	int ret = 0;

	BUG_ON(PageAnon(page));

	spin_lock(&mapping->i_mmap_lock);
	vma_prio_tree_foreach(vma, &iter, &mapping->i_mmap, pgoff, pgoff) {
		if (vma->vm_flags & VM_SHARED) {
			unsigned long address = vma_address(page, vma);
			if (address == -EFAULT)
				continue;
			ret += page_mkclean_one(page, vma, address);
		}
	}
	spin_unlock(&mapping->i_mmap_lock);
	return ret;
}

int page_mkclean(struct page *page)
{
	int ret = 0;

	BUG_ON(!PageLocked(page));

	if (page_mapped(page)) {
		struct address_space *mapping = page_mapping(page);
		if (mapping) {
			ret = page_mkclean_file(mapping, page);
			if (page_test_dirty(page)) {
				page_clear_dirty(page);
				ret = 1;
			}
		}
	}

	return ret;
}
EXPORT_SYMBOL_GPL(page_mkclean);

void page_move_anon_rmap(struct page *page,
	struct vm_area_struct *vma, unsigned long address)
{
	struct anon_vma *anon_vma = vma->anon_vma;

	VM_BUG_ON(!PageLocked(page));
	VM_BUG_ON(!anon_vma);
	VM_BUG_ON(page->index != linear_page_index(vma, address));

	anon_vma = (void *) anon_vma + PAGE_MAPPING_ANON;
	page->mapping = (struct address_space *) anon_vma;
}

static void __page_set_anon_rmap(struct page *page,
	struct vm_area_struct *vma, unsigned long address, int exclusive)
{
	struct anon_vma *anon_vma = vma->anon_vma;

	BUG_ON(!anon_vma);

	/*
	 * If the page isn't exclusively mapped into this vma,
	 * we must use the _oldest_ possible anon_vma for the
	 * page mapping!
	 *
	 * So take the last AVC chain entry in the vma, which is
	 * the deepest ancestor, and use the anon_vma from that.
	 */
	if (!exclusive) {
		struct anon_vma_chain *avc;
		avc = list_entry(vma->anon_vma_chain.prev, struct anon_vma_chain, same_vma);
		anon_vma = avc->anon_vma;
	}

	anon_vma = (void *) anon_vma + PAGE_MAPPING_ANON;
	page->mapping = (struct address_space *) anon_vma;
	page->index = linear_page_index(vma, address);
}

static void __page_check_anon_rmap(struct page *page,
	struct vm_area_struct *vma, unsigned long address)
{
#ifdef CONFIG_DEBUG_VM
	/*
	 * The page's anon-rmap details (mapping and index) are guaranteed to
	 * be set up correctly at this point.
	 *
	 * We have exclusion against page_add_anon_rmap because the caller
	 * always holds the page locked, except if called from page_dup_rmap,
	 * in which case the page is already known to be setup.
	 *
	 * We have exclusion against page_add_new_anon_rmap because those pages
	 * are initially only visible via the pagetables, and the pte is locked
	 * over the call to page_add_new_anon_rmap.
	 */
	BUG_ON(page->index != linear_page_index(vma, address));
#endif
}

void page_add_anon_rmap(struct page *page,
	struct vm_area_struct *vma, unsigned long address)
{
	int first = atomic_inc_and_test(&page->_mapcount);
	if (first)
		__inc_zone_page_state(page, NR_ANON_PAGES);
	if (unlikely(PageKsm(page)))
		return;

	VM_BUG_ON(!PageLocked(page));
	VM_BUG_ON(address < vma->vm_start || address >= vma->vm_end);
	if (first)
		__page_set_anon_rmap(page, vma, address, 0);
	else
		__page_check_anon_rmap(page, vma, address);
}

void page_add_new_anon_rmap(struct page *page,
	struct vm_area_struct *vma, unsigned long address)
{
	VM_BUG_ON(address < vma->vm_start || address >= vma->vm_end);
	SetPageSwapBacked(page);
	atomic_set(&page->_mapcount, 0); /* increment count (starts at -1) */
	__inc_zone_page_state(page, NR_ANON_PAGES);
	__page_set_anon_rmap(page, vma, address, 1);
	if (page_evictable(page, vma))
		lru_cache_add_lru(page, LRU_ACTIVE_ANON);
	else
		add_page_to_unevictable_list(page);
}

void page_add_file_rmap(struct page *page)
{
	if (atomic_inc_and_test(&page->_mapcount)) {
		__inc_zone_page_state(page, NR_FILE_MAPPED);
		mem_cgroup_update_file_mapped(page, 1);
	}
}

void page_remove_rmap(struct page *page)
{
	/* page still mapped by someone else? */
	if (!atomic_add_negative(-1, &page->_mapcount))
		return;

	/*
	 * Now that the last pte has gone, s390 must transfer dirty
	 * flag from storage key to struct page.  We can usually skip
	 * this if the page is anon, so about to be freed; but perhaps
	 * not if it's in swapcache - there might be another pte slot
	 * containing the swap entry, but page not yet written to swap.
	 */
	if ((!PageAnon(page) || PageSwapCache(page)) && page_test_dirty(page)) {
		page_clear_dirty(page);
		set_page_dirty(page);
	}
	if (PageAnon(page)) {
		mem_cgroup_uncharge_page(page);
		__dec_zone_page_state(page, NR_ANON_PAGES);
	} else {
		__dec_zone_page_state(page, NR_FILE_MAPPED);
		mem_cgroup_update_file_mapped(page, -1);
	}
	/*
	 * It would be tidy to reset the PageAnon mapping here,
	 * but that might overwrite a racing page_add_anon_rmap
	 * which increments mapcount after us but sets mapping
	 * before us: so leave the reset to free_hot_cold_page,
	 * and remember that it's only reliable while mapped.
	 * Leaving it set also helps swapoff to reinstate ptes
	 * faster for those pages still in swapcache.
	 */
}

int try_to_unmap_one(struct page *page, struct vm_area_struct *vma,
		     unsigned long address, enum ttu_flags flags)
{
	struct mm_struct *mm = vma->vm_mm;
	pte_t *pte;
	pte_t pteval;
	spinlock_t *ptl;
	int ret = SWAP_AGAIN;

	pte = page_check_address(page, mm, address, &ptl, 0);
	if (!pte)
		goto out;

	/*
	 * If the page is mlock()d, we cannot swap it out.
	 * If it's recently referenced (perhaps page_referenced
	 * skipped over this mm) then we should reactivate it.
	 */
	if (!(flags & TTU_IGNORE_MLOCK)) {
		if (vma->vm_flags & VM_LOCKED)
			goto out_mlock;

		if (TTU_ACTION(flags) == TTU_MUNLOCK)
			goto out_unmap;
	}
	if (!(flags & TTU_IGNORE_ACCESS)) {
		if (ptep_clear_flush_young_notify(vma, address, pte)) {
			ret = SWAP_FAIL;
			goto out_unmap;
		}
  	}

	/* Nuke the page table entry. */
	flush_cache_page(vma, address, page_to_pfn(page));
	pteval = ptep_clear_flush_notify(vma, address, pte);

	/* Move the dirty bit to the physical page now the pte is gone. */
	if (pte_dirty(pteval))
		set_page_dirty(page);

	/* Update high watermark before we lower rss */
	update_hiwater_rss(mm);

	if (PageHWPoison(page) && !(flags & TTU_IGNORE_HWPOISON)) {
		if (PageAnon(page))
			dec_mm_counter(mm, MM_ANONPAGES);
		else
			dec_mm_counter(mm, MM_FILEPAGES);
		set_pte_at(mm, address, pte,
				swp_entry_to_pte(make_hwpoison_entry(page)));
	} else if (PageAnon(page)) {
		swp_entry_t entry = { .val = page_private(page) };

		if (PageSwapCache(page)) {
			/*
			 * Store the swap location in the pte.
			 * See handle_pte_fault() ...
			 */
			if (swap_duplicate(entry) < 0) {
				set_pte_at(mm, address, pte, pteval);
				ret = SWAP_FAIL;
				goto out_unmap;
			}
			if (list_empty(&mm->mmlist)) {
				spin_lock(&mmlist_lock);
				if (list_empty(&mm->mmlist))
					list_add(&mm->mmlist, &init_mm.mmlist);
				spin_unlock(&mmlist_lock);
			}
			dec_mm_counter(mm, MM_ANONPAGES);
			inc_mm_counter(mm, MM_SWAPENTS);
		} else if (PAGE_MIGRATION) {
			/*
			 * Store the pfn of the page in a special migration
			 * pte. do_swap_page() will wait until the migration
			 * pte is removed and then restart fault handling.
			 */
			BUG_ON(TTU_ACTION(flags) != TTU_MIGRATION);
			entry = make_migration_entry(page, pte_write(pteval));
		}
		set_pte_at(mm, address, pte, swp_entry_to_pte(entry));
		BUG_ON(pte_file(*pte));
	} else if (PAGE_MIGRATION && (TTU_ACTION(flags) == TTU_MIGRATION)) {
		/* Establish migration entry for a file page */
		swp_entry_t entry;
		entry = make_migration_entry(page, pte_write(pteval));
		set_pte_at(mm, address, pte, swp_entry_to_pte(entry));
	} else
		dec_mm_counter(mm, MM_FILEPAGES);

	page_remove_rmap(page);
	page_cache_release(page);

out_unmap:
	pte_unmap_unlock(pte, ptl);
out:
	return ret;

out_mlock:
	pte_unmap_unlock(pte, ptl);


	/*
	 * We need mmap_sem locking, Otherwise VM_LOCKED check makes
	 * unstable result and race. Plus, We can't wait here because
	 * we now hold anon_vma->lock or mapping->i_mmap_lock.
	 * if trylock failed, the page remain in evictable lru and later
	 * vmscan could retry to move the page to unevictable lru if the
	 * page is actually mlocked.
	 */
	if (down_read_trylock(&vma->vm_mm->mmap_sem)) {
		if (vma->vm_flags & VM_LOCKED) {
			mlock_vma_page(page);
			ret = SWAP_MLOCK;
		}
		up_read(&vma->vm_mm->mmap_sem);
	}
	return ret;
}

#define CLUSTER_SIZE	min(32*PAGE_SIZE, PMD_SIZE)
#define CLUSTER_MASK	(~(CLUSTER_SIZE - 1))

static int try_to_unmap_cluster(unsigned long cursor, unsigned int *mapcount,
		struct vm_area_struct *vma, struct page *check_page)
{
	struct mm_struct *mm = vma->vm_mm;
	pgd_t *pgd;
	pud_t *pud;
	pmd_t *pmd;
	pte_t *pte;
	pte_t pteval;
	spinlock_t *ptl;
	struct page *page;
	unsigned long address;
	unsigned long end;
	int ret = SWAP_AGAIN;
	int locked_vma = 0;

	address = (vma->vm_start + cursor) & CLUSTER_MASK;
	end = address + CLUSTER_SIZE;
	if (address < vma->vm_start)
		address = vma->vm_start;
	if (end > vma->vm_end)
		end = vma->vm_end;

	pgd = pgd_offset(mm, address);
	if (!pgd_present(*pgd))
		return ret;

	pud = pud_offset(pgd, address);
	if (!pud_present(*pud))
		return ret;

	pmd = pmd_offset(pud, address);
	if (!pmd_present(*pmd))
		return ret;

	/*
	 * If we can acquire the mmap_sem for read, and vma is VM_LOCKED,
	 * keep the sem while scanning the cluster for mlocking pages.
	 */
	if (down_read_trylock(&vma->vm_mm->mmap_sem)) {
		locked_vma = (vma->vm_flags & VM_LOCKED);
		if (!locked_vma)
			up_read(&vma->vm_mm->mmap_sem); /* don't need it */
	}

	pte = pte_offset_map_lock(mm, pmd, address, &ptl);

	/* Update high watermark before we lower rss */
	update_hiwater_rss(mm);

	for (; address < end; pte++, address += PAGE_SIZE) {
		if (!pte_present(*pte))
			continue;
		page = vm_normal_page(vma, address, *pte);
		BUG_ON(!page || PageAnon(page));

		if (locked_vma) {
			mlock_vma_page(page);   /* no-op if already mlocked */
			if (page == check_page)
				ret = SWAP_MLOCK;
			continue;	/* don't unmap */
		}

		if (ptep_clear_flush_young_notify(vma, address, pte))
			continue;

		/* Nuke the page table entry. */
		flush_cache_page(vma, address, pte_pfn(*pte));
		pteval = ptep_clear_flush_notify(vma, address, pte);

		/* If nonlinear, store the file page offset in the pte. */
		if (page->index != linear_page_index(vma, address))
			set_pte_at(mm, address, pte, pgoff_to_pte(page->index));

		/* Move the dirty bit to the physical page now the pte is gone. */
		if (pte_dirty(pteval))
			set_page_dirty(page);

		page_remove_rmap(page);
		page_cache_release(page);
		dec_mm_counter(mm, MM_FILEPAGES);
		(*mapcount)--;
	}
	pte_unmap_unlock(pte - 1, ptl);
	if (locked_vma)
		up_read(&vma->vm_mm->mmap_sem);
	return ret;
}

static bool is_vma_temporary_stack(struct vm_area_struct *vma)
{
	int maybe_stack = vma->vm_flags & (VM_GROWSDOWN | VM_GROWSUP);

	if (!maybe_stack)
		return false;

	if ((vma->vm_flags & VM_STACK_INCOMPLETE_SETUP) ==
						VM_STACK_INCOMPLETE_SETUP)
		return true;

	return false;
}

static int try_to_unmap_anon(struct page *page, enum ttu_flags flags)
{
	struct anon_vma *anon_vma;
	struct anon_vma_chain *avc;
	int ret = SWAP_AGAIN;

	anon_vma = page_lock_anon_vma(page);
	if (!anon_vma)
		return ret;

	list_for_each_entry(avc, &anon_vma->head, same_anon_vma) {
		struct vm_area_struct *vma = avc->vma;
		unsigned long address;

		/*
		 * During exec, a temporary VMA is setup and later moved.
		 * The VMA is moved under the anon_vma lock but not the
		 * page tables leading to a race where migration cannot
		 * find the migration ptes. Rather than increasing the
		 * locking requirements of exec(), migration skips
		 * temporary VMAs until after exec() completes.
		 */
		if (PAGE_MIGRATION && (flags & TTU_MIGRATION) &&
				is_vma_temporary_stack(vma))
			continue;

		address = vma_address(page, vma);
		if (address == -EFAULT)
			continue;
		ret = try_to_unmap_one(page, vma, address, flags);
		if (ret != SWAP_AGAIN || !page_mapped(page))
			break;
	}

	page_unlock_anon_vma(anon_vma);
	return ret;
}

static int try_to_unmap_file(struct page *page, enum ttu_flags flags)
{
	struct address_space *mapping = page->mapping;
	pgoff_t pgoff = page->index << (PAGE_CACHE_SHIFT - PAGE_SHIFT);
	struct vm_area_struct *vma;
	struct prio_tree_iter iter;
	int ret = SWAP_AGAIN;
	unsigned long cursor;
	unsigned long max_nl_cursor = 0;
	unsigned long max_nl_size = 0;
	unsigned int mapcount;

	spin_lock(&mapping->i_mmap_lock);
	vma_prio_tree_foreach(vma, &iter, &mapping->i_mmap, pgoff, pgoff) {
		unsigned long address = vma_address(page, vma);
		if (address == -EFAULT)
			continue;
		ret = try_to_unmap_one(page, vma, address, flags);
		if (ret != SWAP_AGAIN || !page_mapped(page))
			goto out;
	}

	if (list_empty(&mapping->i_mmap_nonlinear))
		goto out;

	/*
	 * We don't bother to try to find the munlocked page in nonlinears.
	 * It's costly. Instead, later, page reclaim logic may call
	 * try_to_unmap(TTU_MUNLOCK) and recover PG_mlocked lazily.
	 */
	if (TTU_ACTION(flags) == TTU_MUNLOCK)
		goto out;

	list_for_each_entry(vma, &mapping->i_mmap_nonlinear,
						shared.vm_set.list) {
		cursor = (unsigned long) vma->vm_private_data;
		if (cursor > max_nl_cursor)
			max_nl_cursor = cursor;
		cursor = vma->vm_end - vma->vm_start;
		if (cursor > max_nl_size)
			max_nl_size = cursor;
	}

	if (max_nl_size == 0) {	/* all nonlinears locked or reserved ? */
		ret = SWAP_FAIL;
		goto out;
	}

	/*
	 * We don't try to search for this page in the nonlinear vmas,
	 * and page_referenced wouldn't have found it anyway.  Instead
	 * just walk the nonlinear vmas trying to age and unmap some.
	 * The mapcount of the page we came in with is irrelevant,
	 * but even so use it as a guide to how hard we should try?
	 */
	mapcount = page_mapcount(page);
	if (!mapcount)
		goto out;
	cond_resched_lock(&mapping->i_mmap_lock);

	max_nl_size = (max_nl_size + CLUSTER_SIZE - 1) & CLUSTER_MASK;
	if (max_nl_cursor == 0)
		max_nl_cursor = CLUSTER_SIZE;

	do {
		list_for_each_entry(vma, &mapping->i_mmap_nonlinear,
						shared.vm_set.list) {
			cursor = (unsigned long) vma->vm_private_data;
			while ( cursor < max_nl_cursor &&
				cursor < vma->vm_end - vma->vm_start) {
				if (try_to_unmap_cluster(cursor, &mapcount,
						vma, page) == SWAP_MLOCK)
					ret = SWAP_MLOCK;
				cursor += CLUSTER_SIZE;
				vma->vm_private_data = (void *) cursor;
				if ((int)mapcount <= 0)
					goto out;
			}
			vma->vm_private_data = (void *) max_nl_cursor;
		}
		cond_resched_lock(&mapping->i_mmap_lock);
		max_nl_cursor += CLUSTER_SIZE;
	} while (max_nl_cursor <= max_nl_size);

	/*
	 * Don't loop forever (perhaps all the remaining pages are
	 * in locked vmas).  Reset cursor on all unreserved nonlinear
	 * vmas, now forgetting on which ones it had fallen behind.
	 */
	list_for_each_entry(vma, &mapping->i_mmap_nonlinear, shared.vm_set.list)
		vma->vm_private_data = NULL;
out:
	spin_unlock(&mapping->i_mmap_lock);
	return ret;
}

int try_to_unmap(struct page *page, enum ttu_flags flags)
{
	int ret;

	BUG_ON(!PageLocked(page));

	if (unlikely(PageKsm(page)))
		ret = try_to_unmap_ksm(page, flags);
	else if (PageAnon(page))
		ret = try_to_unmap_anon(page, flags);
	else
		ret = try_to_unmap_file(page, flags);
	if (ret != SWAP_MLOCK && !page_mapped(page))
		ret = SWAP_SUCCESS;
	return ret;
}

int try_to_munlock(struct page *page)
{
	VM_BUG_ON(!PageLocked(page) || PageLRU(page));

	if (unlikely(PageKsm(page)))
		return try_to_unmap_ksm(page, TTU_MUNLOCK);
	else if (PageAnon(page))
		return try_to_unmap_anon(page, TTU_MUNLOCK);
	else
		return try_to_unmap_file(page, TTU_MUNLOCK);
}

#ifdef CONFIG_MIGRATION
static int rmap_walk_anon(struct page *page, int (*rmap_one)(struct page *,
		struct vm_area_struct *, unsigned long, void *), void *arg)
{
	struct anon_vma *anon_vma;
	struct anon_vma_chain *avc;
	int ret = SWAP_AGAIN;

	/*
	 * Note: remove_migration_ptes() cannot use page_lock_anon_vma()
	 * because that depends on page_mapped(); but not all its usages
	 * are holding mmap_sem. Users without mmap_sem are required to
	 * take a reference count to prevent the anon_vma disappearing
	 */
	anon_vma = page_anon_vma(page);
	if (!anon_vma)
		return ret;
	spin_lock(&anon_vma->lock);
	list_for_each_entry(avc, &anon_vma->head, same_anon_vma) {
		struct vm_area_struct *vma = avc->vma;
		unsigned long address = vma_address(page, vma);
		if (address == -EFAULT)
			continue;
		ret = rmap_one(page, vma, address, arg);
		if (ret != SWAP_AGAIN)
			break;
	}
	spin_unlock(&anon_vma->lock);
	return ret;
}

static int rmap_walk_file(struct page *page, int (*rmap_one)(struct page *,
		struct vm_area_struct *, unsigned long, void *), void *arg)
{
	struct address_space *mapping = page->mapping;
	pgoff_t pgoff = page->index << (PAGE_CACHE_SHIFT - PAGE_SHIFT);
	struct vm_area_struct *vma;
	struct prio_tree_iter iter;
	int ret = SWAP_AGAIN;

	if (!mapping)
		return ret;
	spin_lock(&mapping->i_mmap_lock);
	vma_prio_tree_foreach(vma, &iter, &mapping->i_mmap, pgoff, pgoff) {
		unsigned long address = vma_address(page, vma);
		if (address == -EFAULT)
			continue;
		ret = rmap_one(page, vma, address, arg);
		if (ret != SWAP_AGAIN)
			break;
	}
	/*
	 * No nonlinear handling: being always shared, nonlinear vmas
	 * never contain migration ptes.  Decide what to do about this
	 * limitation to linear when we need rmap_walk() on nonlinear.
	 */
	spin_unlock(&mapping->i_mmap_lock);
	return ret;
}

int rmap_walk(struct page *page, int (*rmap_one)(struct page *,
		struct vm_area_struct *, unsigned long, void *), void *arg)
{
	VM_BUG_ON(!PageLocked(page));

	if (unlikely(PageKsm(page)))
		return rmap_walk_ksm(page, rmap_one, arg);
	else if (PageAnon(page))
		return rmap_walk_anon(page, rmap_one, arg);
	else
		return rmap_walk_file(page, rmap_one, arg);
}
#endif /* CONFIG_MIGRATION */
