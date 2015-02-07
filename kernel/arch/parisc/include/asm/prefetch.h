

#ifndef __ASM_PARISC_PREFETCH_H
#define __ASM_PARISC_PREFETCH_H

#ifndef __ASSEMBLY__
#ifdef CONFIG_PREFETCH

#define ARCH_HAS_PREFETCH
static inline void prefetch(const void *addr)
{
	__asm__("ldw 0(%0), %%r0" : : "r" (addr));
}

/* LDD is a PA2.0 addition. */
#ifdef CONFIG_PA20
#define ARCH_HAS_PREFETCHW
static inline void prefetchw(const void *addr)
{
	__asm__("ldd 0(%0), %%r0" : : "r" (addr));
}
#endif /* CONFIG_PA20 */

#endif /* CONFIG_PREFETCH */
#endif /* __ASSEMBLY__ */

#endif /* __ASM_PARISC_PROCESSOR_H */
