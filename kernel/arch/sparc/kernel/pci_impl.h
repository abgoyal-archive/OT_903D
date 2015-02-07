

#ifndef PCI_IMPL_H
#define PCI_IMPL_H

#include <linux/types.h>
#include <linux/spinlock.h>
#include <linux/pci.h>
#include <linux/msi.h>
#include <linux/of_device.h>
#include <asm/io.h>
#include <asm/prom.h>
#include <asm/iommu.h>


#define PCI_STC_FLUSHFLAG_INIT(STC) \
	(*((STC)->strbuf_flushflag) = 0UL)
#define PCI_STC_FLUSHFLAG_SET(STC) \
	(*((STC)->strbuf_flushflag) != 0UL)

#ifdef CONFIG_PCI_MSI
struct pci_pbm_info;
struct sparc64_msiq_ops {
	int (*get_head)(struct pci_pbm_info *pbm, unsigned long msiqid,
			unsigned long *head);
	int (*dequeue_msi)(struct pci_pbm_info *pbm, unsigned long msiqid,
			   unsigned long *head, unsigned long *msi);
	int (*set_head)(struct pci_pbm_info *pbm, unsigned long msiqid,
			unsigned long head);
	int (*msi_setup)(struct pci_pbm_info *pbm, unsigned long msiqid,
			 unsigned long msi, int is_msi64);
	int (*msi_teardown)(struct pci_pbm_info *pbm, unsigned long msi);
	int (*msiq_alloc)(struct pci_pbm_info *pbm);
	void (*msiq_free)(struct pci_pbm_info *pbm);
	int (*msiq_build_irq)(struct pci_pbm_info *pbm, unsigned long msiqid,
			      unsigned long devino);
};

extern void sparc64_pbm_msi_init(struct pci_pbm_info *pbm,
				 const struct sparc64_msiq_ops *ops);

struct sparc64_msiq_cookie {
	struct pci_pbm_info *pbm;
	unsigned long msiqid;
};
#endif

struct pci_pbm_info {
	struct pci_pbm_info		*next;
	struct pci_pbm_info		*sibling;
	int				index;

	/* Physical address base of controller registers. */
	unsigned long			controller_regs;

	/* Physical address base of PBM registers. */
	unsigned long			pbm_regs;

	/* Physical address of DMA sync register, if any.  */
	unsigned long			sync_reg;

	/* Opaque 32-bit system bus Port ID. */
	u32				portid;

	/* Opaque 32-bit handle used for hypervisor calls.  */
	u32				devhandle;

	/* Chipset version information. */
	int				chip_type;
#define PBM_CHIP_TYPE_SABRE		1
#define PBM_CHIP_TYPE_PSYCHO		2
#define PBM_CHIP_TYPE_SCHIZO		3
#define PBM_CHIP_TYPE_SCHIZO_PLUS	4
#define PBM_CHIP_TYPE_TOMATILLO		5
	int				chip_version;
	int				chip_revision;

	/* Name used for top-level resources. */
	char				*name;

	/* OBP specific information. */
	struct of_device		*op;
	u64				ino_bitmap;

	/* PBM I/O and Memory space resources. */
	struct resource			io_space;
	struct resource			mem_space;

	/* Base of PCI Config space, can be per-PBM or shared. */
	unsigned long			config_space;

	/* This will be 12 on PCI-E controllers, 8 elsewhere.  */
	unsigned long			config_space_reg_bits;

	unsigned long			pci_afsr;
	unsigned long			pci_afar;
	unsigned long			pci_csr;

	/* State of 66MHz capabilities on this PBM. */
	int				is_66mhz_capable;
	int				all_devs_66mhz;

#ifdef CONFIG_PCI_MSI
	/* MSI info.  */
	u32				msiq_num;
	u32				msiq_ent_count;
	u32				msiq_first;
	u32				msiq_first_devino;
	u32				msiq_rotor;
	struct sparc64_msiq_cookie	*msiq_irq_cookies;
	u32				msi_num;
	u32				msi_first;
	u32				msi_data_mask;
	u32				msix_data_width;
	u64				msi32_start;
	u64				msi64_start;
	u32				msi32_len;
	u32				msi64_len;
	void				*msi_queues;
	unsigned long			*msi_bitmap;
	unsigned int			*msi_irq_table;
	int (*setup_msi_irq)(unsigned int *virt_irq_p, struct pci_dev *pdev,
			     struct msi_desc *entry);
	void (*teardown_msi_irq)(unsigned int virt_irq, struct pci_dev *pdev);
	const struct sparc64_msiq_ops	*msi_ops;
#endif /* !(CONFIG_PCI_MSI) */

	/* This PBM's streaming buffer. */
	struct strbuf			stc;

	/* IOMMU state, potentially shared by both PBM segments. */
	struct iommu			*iommu;

	/* Now things for the actual PCI bus probes. */
	unsigned int			pci_first_busno;
	unsigned int			pci_last_busno;
	struct pci_bus			*pci_bus;
	struct pci_ops			*pci_ops;

	int				numa_node;
};

extern struct pci_pbm_info *pci_pbm_root;

extern int pci_num_pbms;

/* PCI bus scanning and fixup support. */
extern void pci_get_pbm_props(struct pci_pbm_info *pbm);
extern struct pci_bus *pci_scan_one_pbm(struct pci_pbm_info *pbm,
					struct device *parent);
extern void pci_determine_mem_io_space(struct pci_pbm_info *pbm);

/* Error reporting support. */
extern void pci_scan_for_target_abort(struct pci_pbm_info *, struct pci_bus *);
extern void pci_scan_for_master_abort(struct pci_pbm_info *, struct pci_bus *);
extern void pci_scan_for_parity_error(struct pci_pbm_info *, struct pci_bus *);

/* Configuration space access. */
extern void pci_config_read8(u8 *addr, u8 *ret);
extern void pci_config_read16(u16 *addr, u16 *ret);
extern void pci_config_read32(u32 *addr, u32 *ret);
extern void pci_config_write8(u8 *addr, u8 val);
extern void pci_config_write16(u16 *addr, u16 val);
extern void pci_config_write32(u32 *addr, u32 val);

extern struct pci_ops sun4u_pci_ops;
extern struct pci_ops sun4v_pci_ops;

extern volatile int pci_poke_in_progress;
extern volatile int pci_poke_cpu;
extern volatile int pci_poke_faulted;

#endif /* !(PCI_IMPL_H) */
