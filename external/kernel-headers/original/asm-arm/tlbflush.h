#ifndef _ASMARM_TLBFLUSH_H
#define _ASMARM_TLBFLUSH_H


#ifndef CONFIG_MMU

#define tlb_flush(tlb)	((void) tlb)

#else /* CONFIG_MMU */

#include <asm/glue.h>

#define TLB_V3_PAGE	(1 << 0)
#define TLB_V4_U_PAGE	(1 << 1)
#define TLB_V4_D_PAGE	(1 << 2)
#define TLB_V4_I_PAGE	(1 << 3)
#define TLB_V6_U_PAGE	(1 << 4)
#define TLB_V6_D_PAGE	(1 << 5)
#define TLB_V6_I_PAGE	(1 << 6)

#define TLB_V3_FULL	(1 << 8)
#define TLB_V4_U_FULL	(1 << 9)
#define TLB_V4_D_FULL	(1 << 10)
#define TLB_V4_I_FULL	(1 << 11)
#define TLB_V6_U_FULL	(1 << 12)
#define TLB_V6_D_FULL	(1 << 13)
#define TLB_V6_I_FULL	(1 << 14)

#define TLB_V6_U_ASID	(1 << 16)
#define TLB_V6_D_ASID	(1 << 17)
#define TLB_V6_I_ASID	(1 << 18)

#define TLB_DCLEAN	(1 << 30)
#define TLB_WB		(1 << 31)

#undef _TLB
#undef MULTI_TLB

#define v3_tlb_flags	(TLB_V3_FULL | TLB_V3_PAGE)

#ifdef CONFIG_CPU_TLB_V3
# define v3_possible_flags	v3_tlb_flags
# define v3_always_flags	v3_tlb_flags
# ifdef _TLB
#  define MULTI_TLB 1
# else
#  define _TLB v3
# endif
#else
# define v3_possible_flags	0
# define v3_always_flags	(-1UL)
#endif

#define v4_tlb_flags	(TLB_V4_U_FULL | TLB_V4_U_PAGE)

#ifdef CONFIG_CPU_TLB_V4WT
# define v4_possible_flags	v4_tlb_flags
# define v4_always_flags	v4_tlb_flags
# ifdef _TLB
#  define MULTI_TLB 1
# else
#  define _TLB v4
# endif
#else
# define v4_possible_flags	0
# define v4_always_flags	(-1UL)
#endif

#define v4wbi_tlb_flags	(TLB_WB | TLB_DCLEAN | \
			 TLB_V4_I_FULL | TLB_V4_D_FULL | \
			 TLB_V4_I_PAGE | TLB_V4_D_PAGE)

#ifdef CONFIG_CPU_TLB_V4WBI
# define v4wbi_possible_flags	v4wbi_tlb_flags
# define v4wbi_always_flags	v4wbi_tlb_flags
# ifdef _TLB
#  define MULTI_TLB 1
# else
#  define _TLB v4wbi
# endif
#else
# define v4wbi_possible_flags	0
# define v4wbi_always_flags	(-1UL)
#endif

#define v4wb_tlb_flags	(TLB_WB | TLB_DCLEAN | \
			 TLB_V4_I_FULL | TLB_V4_D_FULL | \
			 TLB_V4_D_PAGE)

#ifdef CONFIG_CPU_TLB_V4WB
# define v4wb_possible_flags	v4wb_tlb_flags
# define v4wb_always_flags	v4wb_tlb_flags
# ifdef _TLB
#  define MULTI_TLB 1
# else
#  define _TLB v4wb
# endif
#else
# define v4wb_possible_flags	0
# define v4wb_always_flags	(-1UL)
#endif

#define v6wbi_tlb_flags (TLB_WB | TLB_DCLEAN | \
			 TLB_V6_I_FULL | TLB_V6_D_FULL | \
			 TLB_V6_I_PAGE | TLB_V6_D_PAGE | \
			 TLB_V6_I_ASID | TLB_V6_D_ASID)

#ifdef CONFIG_CPU_TLB_V6
# define v6wbi_possible_flags	v6wbi_tlb_flags
# define v6wbi_always_flags	v6wbi_tlb_flags
# ifdef _TLB
#  define MULTI_TLB 1
# else
#  define _TLB v6wbi
# endif
#else
# define v6wbi_possible_flags	0
# define v6wbi_always_flags	(-1UL)
#endif

#ifndef _TLB
#error Unknown TLB model
#endif

#ifndef __ASSEMBLY__

struct cpu_tlb_fns {
	void (*flush_user_range)(unsigned long, unsigned long, struct vm_area_struct *);
	void (*flush_kern_range)(unsigned long, unsigned long);
	unsigned long tlb_flags;
};

#ifdef MULTI_TLB

#define __cpu_flush_user_tlb_range	cpu_tlb.flush_user_range
#define __cpu_flush_kern_tlb_range	cpu_tlb.flush_kern_range

#else

#define __cpu_flush_user_tlb_range	__glue(_TLB,_flush_user_tlb_range)
#define __cpu_flush_kern_tlb_range	__glue(_TLB,_flush_kern_tlb_range)

extern void __cpu_flush_user_tlb_range(unsigned long, unsigned long, struct vm_area_struct *);
extern void __cpu_flush_kern_tlb_range(unsigned long, unsigned long);

#endif

extern struct cpu_tlb_fns cpu_tlb;

#define __cpu_tlb_flags			cpu_tlb.tlb_flags


#define possible_tlb_flags	(v3_possible_flags | \
				 v4_possible_flags | \
				 v4wbi_possible_flags | \
				 v4wb_possible_flags | \
				 v6wbi_possible_flags)

#define always_tlb_flags	(v3_always_flags & \
				 v4_always_flags & \
				 v4wbi_always_flags & \
				 v4wb_always_flags & \
				 v6wbi_always_flags)

#define tlb_flag(f)	((always_tlb_flags & (f)) || (__tlb_flag & possible_tlb_flags & (f)))

static inline void local_flush_tlb_all(void)
{
	const int zero = 0;
	const unsigned int __tlb_flag = __cpu_tlb_flags;

	if (tlb_flag(TLB_WB))
		asm("mcr%? p15, 0, %0, c7, c10, 4" : : "r" (zero));

	if (tlb_flag(TLB_V3_FULL))
		asm("mcr%? p15, 0, %0, c6, c0, 0" : : "r" (zero));
	if (tlb_flag(TLB_V4_U_FULL | TLB_V6_U_FULL))
		asm("mcr%? p15, 0, %0, c8, c7, 0" : : "r" (zero));
	if (tlb_flag(TLB_V4_D_FULL | TLB_V6_D_FULL))
		asm("mcr%? p15, 0, %0, c8, c6, 0" : : "r" (zero));
	if (tlb_flag(TLB_V4_I_FULL | TLB_V6_I_FULL))
		asm("mcr%? p15, 0, %0, c8, c5, 0" : : "r" (zero));
}

static inline void local_flush_tlb_mm(struct mm_struct *mm)
{
	const int zero = 0;
	const int asid = ASID(mm);
	const unsigned int __tlb_flag = __cpu_tlb_flags;

	if (tlb_flag(TLB_WB))
		asm("mcr%? p15, 0, %0, c7, c10, 4" : : "r" (zero));

	if (cpu_isset(smp_processor_id(), mm->cpu_vm_mask)) {
		if (tlb_flag(TLB_V3_FULL))
			asm("mcr%? p15, 0, %0, c6, c0, 0" : : "r" (zero));
		if (tlb_flag(TLB_V4_U_FULL))
			asm("mcr%? p15, 0, %0, c8, c7, 0" : : "r" (zero));
		if (tlb_flag(TLB_V4_D_FULL))
			asm("mcr%? p15, 0, %0, c8, c6, 0" : : "r" (zero));
		if (tlb_flag(TLB_V4_I_FULL))
			asm("mcr%? p15, 0, %0, c8, c5, 0" : : "r" (zero));
	}

	if (tlb_flag(TLB_V6_U_ASID))
		asm("mcr%? p15, 0, %0, c8, c7, 2" : : "r" (asid));
	if (tlb_flag(TLB_V6_D_ASID))
		asm("mcr%? p15, 0, %0, c8, c6, 2" : : "r" (asid));
	if (tlb_flag(TLB_V6_I_ASID))
		asm("mcr%? p15, 0, %0, c8, c5, 2" : : "r" (asid));
}

static inline void
local_flush_tlb_page(struct vm_area_struct *vma, unsigned long uaddr)
{
	const int zero = 0;
	const unsigned int __tlb_flag = __cpu_tlb_flags;

	uaddr = (uaddr & PAGE_MASK) | ASID(vma->vm_mm);

	if (tlb_flag(TLB_WB))
		asm("mcr%? p15, 0, %0, c7, c10, 4" : : "r" (zero));

	if (cpu_isset(smp_processor_id(), vma->vm_mm->cpu_vm_mask)) {
		if (tlb_flag(TLB_V3_PAGE))
			asm("mcr%? p15, 0, %0, c6, c0, 0" : : "r" (uaddr));
		if (tlb_flag(TLB_V4_U_PAGE))
			asm("mcr%? p15, 0, %0, c8, c7, 1" : : "r" (uaddr));
		if (tlb_flag(TLB_V4_D_PAGE))
			asm("mcr%? p15, 0, %0, c8, c6, 1" : : "r" (uaddr));
		if (tlb_flag(TLB_V4_I_PAGE))
			asm("mcr%? p15, 0, %0, c8, c5, 1" : : "r" (uaddr));
		if (!tlb_flag(TLB_V4_I_PAGE) && tlb_flag(TLB_V4_I_FULL))
			asm("mcr%? p15, 0, %0, c8, c5, 0" : : "r" (zero));
	}

	if (tlb_flag(TLB_V6_U_PAGE))
		asm("mcr%? p15, 0, %0, c8, c7, 1" : : "r" (uaddr));
	if (tlb_flag(TLB_V6_D_PAGE))
		asm("mcr%? p15, 0, %0, c8, c6, 1" : : "r" (uaddr));
	if (tlb_flag(TLB_V6_I_PAGE))
		asm("mcr%? p15, 0, %0, c8, c5, 1" : : "r" (uaddr));
}

static inline void local_flush_tlb_kernel_page(unsigned long kaddr)
{
	const int zero = 0;
	const unsigned int __tlb_flag = __cpu_tlb_flags;

	kaddr &= PAGE_MASK;

	if (tlb_flag(TLB_WB))
		asm("mcr%? p15, 0, %0, c7, c10, 4" : : "r" (zero));

	if (tlb_flag(TLB_V3_PAGE))
		asm("mcr%? p15, 0, %0, c6, c0, 0" : : "r" (kaddr));
	if (tlb_flag(TLB_V4_U_PAGE))
		asm("mcr%? p15, 0, %0, c8, c7, 1" : : "r" (kaddr));
	if (tlb_flag(TLB_V4_D_PAGE))
		asm("mcr%? p15, 0, %0, c8, c6, 1" : : "r" (kaddr));
	if (tlb_flag(TLB_V4_I_PAGE))
		asm("mcr%? p15, 0, %0, c8, c5, 1" : : "r" (kaddr));
	if (!tlb_flag(TLB_V4_I_PAGE) && tlb_flag(TLB_V4_I_FULL))
		asm("mcr%? p15, 0, %0, c8, c5, 0" : : "r" (zero));

	if (tlb_flag(TLB_V6_U_PAGE))
		asm("mcr%? p15, 0, %0, c8, c7, 1" : : "r" (kaddr));
	if (tlb_flag(TLB_V6_D_PAGE))
		asm("mcr%? p15, 0, %0, c8, c6, 1" : : "r" (kaddr));
	if (tlb_flag(TLB_V6_I_PAGE))
		asm("mcr%? p15, 0, %0, c8, c5, 1" : : "r" (kaddr));

	/* The ARM ARM states that the completion of a TLB maintenance
	 * operation is only guaranteed by a DSB instruction
	 */
	if (tlb_flag(TLB_V6_U_PAGE | TLB_V6_D_PAGE | TLB_V6_I_PAGE))
		asm("mcr%? p15, 0, %0, c7, c10, 4" : : "r" (zero));
}

static inline void flush_pmd_entry(pmd_t *pmd)
{
	const unsigned int zero = 0;
	const unsigned int __tlb_flag = __cpu_tlb_flags;

	if (tlb_flag(TLB_DCLEAN))
		asm("mcr%?	p15, 0, %0, c7, c10, 1	@ flush_pmd"
			: : "r" (pmd));
	if (tlb_flag(TLB_WB))
		asm("mcr%?	p15, 0, %0, c7, c10, 4	@ flush_pmd"
			: : "r" (zero));
}

static inline void clean_pmd_entry(pmd_t *pmd)
{
	const unsigned int __tlb_flag = __cpu_tlb_flags;

	if (tlb_flag(TLB_DCLEAN))
		asm("mcr%?	p15, 0, %0, c7, c10, 1	@ flush_pmd"
			: : "r" (pmd));
}

#undef tlb_flag
#undef always_tlb_flags
#undef possible_tlb_flags

#define local_flush_tlb_range(vma,start,end)	__cpu_flush_user_tlb_range(start,end,vma)
#define local_flush_tlb_kernel_range(s,e)	__cpu_flush_kern_tlb_range(s,e)

#ifndef CONFIG_SMP
#define flush_tlb_all		local_flush_tlb_all
#define flush_tlb_mm		local_flush_tlb_mm
#define flush_tlb_page		local_flush_tlb_page
#define flush_tlb_kernel_page	local_flush_tlb_kernel_page
#define flush_tlb_range		local_flush_tlb_range
#define flush_tlb_kernel_range	local_flush_tlb_kernel_range
#else
extern void flush_tlb_all(void);
extern void flush_tlb_mm(struct mm_struct *mm);
extern void flush_tlb_page(struct vm_area_struct *vma, unsigned long uaddr);
extern void flush_tlb_kernel_page(unsigned long kaddr);
extern void flush_tlb_range(struct vm_area_struct *vma, unsigned long start, unsigned long end);
extern void flush_tlb_kernel_range(unsigned long start, unsigned long end);
#endif

extern void update_mmu_cache(struct vm_area_struct *vma, unsigned long addr, pte_t pte);

#define flush_tlb_pgtables(mm,start,end)	do { } while (0)

#endif

#endif /* CONFIG_MMU */

#endif
