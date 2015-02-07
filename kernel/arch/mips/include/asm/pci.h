
#ifndef _ASM_PCI_H
#define _ASM_PCI_H

#include <linux/mm.h>

#ifdef __KERNEL__


#include <linux/ioport.h>

struct pci_controller {
	struct pci_controller *next;
	struct pci_bus *bus;

	struct pci_ops *pci_ops;
	struct resource *mem_resource;
	unsigned long mem_offset;
	struct resource *io_resource;
	unsigned long io_offset;
	unsigned long io_map_base;

	unsigned int index;
	/* For compatibility with current (as of July 2003) pciutils
	   and XFree86. Eventually will be removed. */
	unsigned int need_domain_info;

	int iommu;

	/* Optional access methods for reading/writing the bus number
	   of the PCI controller */
	int (*get_busno)(void);
	void (*set_busno)(int busno);
};

extern struct pci_controller * alloc_pci_controller(void);
extern void register_pci_controller(struct pci_controller *hose);

extern int pcibios_map_irq(const struct pci_dev *dev, u8 slot, u8 pin);



extern unsigned int pcibios_assign_all_busses(void);

extern unsigned long PCIBIOS_MIN_IO;
extern unsigned long PCIBIOS_MIN_MEM;

#define PCIBIOS_MIN_CARDBUS_IO	0x4000

extern void pcibios_set_master(struct pci_dev *dev);

static inline void pcibios_penalize_isa_irq(int irq, int active)
{
	/* We don't do dynamic PCI IRQ allocation */
}

#define HAVE_PCI_MMAP

extern int pci_mmap_page_range(struct pci_dev *dev, struct vm_area_struct *vma,
	enum pci_mmap_state mmap_state, int write_combine);


#include <linux/types.h>
#include <linux/slab.h>
#include <asm/scatterlist.h>
#include <linux/string.h>
#include <asm/io.h>

struct pci_dev;

extern unsigned int PCI_DMA_BUS_IS_PHYS;

#ifdef CONFIG_PCI
static inline void pci_dma_burst_advice(struct pci_dev *pdev,
					enum pci_dma_burst_strategy *strat,
					unsigned long *strategy_parameter)
{
	*strat = PCI_DMA_BURST_INFINITY;
	*strategy_parameter = ~0UL;
}
#endif

extern void pcibios_resource_to_bus(struct pci_dev *dev,
	struct pci_bus_region *region, struct resource *res);

extern void pcibios_bus_to_resource(struct pci_dev *dev, struct resource *res,
				    struct pci_bus_region *region);

#define pci_domain_nr(bus) ((struct pci_controller *)(bus)->sysdata)->index

static inline int pci_proc_domain(struct pci_bus *bus)
{
	struct pci_controller *hose = bus->sysdata;
	return hose->need_domain_info;
}

#endif /* __KERNEL__ */

/* implement the pci_ DMA API in terms of the generic device dma_ one */
#include <asm-generic/pci-dma-compat.h>

/* Do platform specific device initialization at pci_enable_device() time */
extern int pcibios_plat_dev_init(struct pci_dev *dev);

/* Chances are this interrupt is wired PC-style ...  */
static inline int pci_get_legacy_ide_irq(struct pci_dev *dev, int channel)
{
	return channel ? 15 : 14;
}

extern int pci_probe_only;

extern char * (*pcibios_plat_setup)(char *str);

#endif /* _ASM_PCI_H */