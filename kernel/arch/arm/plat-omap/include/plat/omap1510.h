

#ifndef __ASM_ARCH_OMAP15XX_H
#define __ASM_ARCH_OMAP15XX_H


/* Syntax: XX_BASE = Virtual base address, XX_START = Physical base address */

#define OMAP1510_DSP_BASE	0xE0000000
#define OMAP1510_DSP_SIZE	0x28000
#define OMAP1510_DSP_START	0xE0000000

#define OMAP1510_DSPREG_BASE	0xE1000000
#define OMAP1510_DSPREG_SIZE	SZ_128K
#define OMAP1510_DSPREG_START	0xE1000000

#define OMAP1510_DSP_MMU_BASE	(0xfffed200)

#endif /*  __ASM_ARCH_OMAP15XX_H */
