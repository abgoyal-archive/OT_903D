
#ifndef _ASM_X86_PERF_EVENT_H
#define _ASM_X86_PERF_EVENT_H


#define X86_PMC_MAX_GENERIC				       32
#define X86_PMC_MAX_FIXED					3

#define X86_PMC_IDX_GENERIC				        0
#define X86_PMC_IDX_FIXED				       32
#define X86_PMC_IDX_MAX					       64

#define MSR_ARCH_PERFMON_PERFCTR0			      0xc1
#define MSR_ARCH_PERFMON_PERFCTR1			      0xc2

#define MSR_ARCH_PERFMON_EVENTSEL0			     0x186
#define MSR_ARCH_PERFMON_EVENTSEL1			     0x187

#define ARCH_PERFMON_EVENTSEL_EVENT			0x000000FFULL
#define ARCH_PERFMON_EVENTSEL_UMASK			0x0000FF00ULL
#define ARCH_PERFMON_EVENTSEL_USR			(1ULL << 16)
#define ARCH_PERFMON_EVENTSEL_OS			(1ULL << 17)
#define ARCH_PERFMON_EVENTSEL_EDGE			(1ULL << 18)
#define ARCH_PERFMON_EVENTSEL_INT			(1ULL << 20)
#define ARCH_PERFMON_EVENTSEL_ANY			(1ULL << 21)
#define ARCH_PERFMON_EVENTSEL_ENABLE			(1ULL << 22)
#define ARCH_PERFMON_EVENTSEL_INV			(1ULL << 23)
#define ARCH_PERFMON_EVENTSEL_CMASK			0xFF000000ULL

#define AMD64_EVENTSEL_EVENT	\
	(ARCH_PERFMON_EVENTSEL_EVENT | (0x0FULL << 32))
#define INTEL_ARCH_EVENT_MASK	\
	(ARCH_PERFMON_EVENTSEL_UMASK | ARCH_PERFMON_EVENTSEL_EVENT)

#define X86_RAW_EVENT_MASK		\
	(ARCH_PERFMON_EVENTSEL_EVENT |	\
	 ARCH_PERFMON_EVENTSEL_UMASK |	\
	 ARCH_PERFMON_EVENTSEL_EDGE  |	\
	 ARCH_PERFMON_EVENTSEL_INV   |	\
	 ARCH_PERFMON_EVENTSEL_CMASK)
#define AMD64_RAW_EVENT_MASK		\
	(X86_RAW_EVENT_MASK          |  \
	 AMD64_EVENTSEL_EVENT)

#define ARCH_PERFMON_UNHALTED_CORE_CYCLES_SEL		      0x3c
#define ARCH_PERFMON_UNHALTED_CORE_CYCLES_UMASK		(0x00 << 8)
#define ARCH_PERFMON_UNHALTED_CORE_CYCLES_INDEX			 0
#define ARCH_PERFMON_UNHALTED_CORE_CYCLES_PRESENT \
		(1 << (ARCH_PERFMON_UNHALTED_CORE_CYCLES_INDEX))

#define ARCH_PERFMON_BRANCH_MISSES_RETIRED			 6

union cpuid10_eax {
	struct {
		unsigned int version_id:8;
		unsigned int num_counters:8;
		unsigned int bit_width:8;
		unsigned int mask_length:8;
	} split;
	unsigned int full;
};

union cpuid10_edx {
	struct {
		unsigned int num_counters_fixed:4;
		unsigned int reserved:28;
	} split;
	unsigned int full;
};



#define MSR_ARCH_PERFMON_FIXED_CTR_CTRL			0x38d


/* Instr_Retired.Any: */
#define MSR_ARCH_PERFMON_FIXED_CTR0			0x309
#define X86_PMC_IDX_FIXED_INSTRUCTIONS			(X86_PMC_IDX_FIXED + 0)

/* CPU_CLK_Unhalted.Core: */
#define MSR_ARCH_PERFMON_FIXED_CTR1			0x30a
#define X86_PMC_IDX_FIXED_CPU_CYCLES			(X86_PMC_IDX_FIXED + 1)

/* CPU_CLK_Unhalted.Ref: */
#define MSR_ARCH_PERFMON_FIXED_CTR2			0x30b
#define X86_PMC_IDX_FIXED_BUS_CYCLES			(X86_PMC_IDX_FIXED + 2)

#define X86_PMC_IDX_FIXED_BTS				(X86_PMC_IDX_FIXED + 16)

/* IbsFetchCtl bits/masks */
#define IBS_FETCH_RAND_EN		(1ULL<<57)
#define IBS_FETCH_VAL			(1ULL<<49)
#define IBS_FETCH_ENABLE		(1ULL<<48)
#define IBS_FETCH_CNT			0xFFFF0000ULL
#define IBS_FETCH_MAX_CNT		0x0000FFFFULL

/* IbsOpCtl bits */
#define IBS_OP_CNT_CTL			(1ULL<<19)
#define IBS_OP_VAL			(1ULL<<18)
#define IBS_OP_ENABLE			(1ULL<<17)
#define IBS_OP_MAX_CNT			0x0000FFFFULL

#ifdef CONFIG_PERF_EVENTS
extern void init_hw_perf_events(void);
extern void perf_events_lapic_init(void);

#define PERF_EVENT_INDEX_OFFSET			0

#define PERF_EFLAGS_EXACT	(1UL << 3)

struct pt_regs;
extern unsigned long perf_instruction_pointer(struct pt_regs *regs);
extern unsigned long perf_misc_flags(struct pt_regs *regs);
#define perf_misc_flags(regs)	perf_misc_flags(regs)

#else
static inline void init_hw_perf_events(void)		{ }
static inline void perf_events_lapic_init(void)	{ }
#endif

#endif /* _ASM_X86_PERF_EVENT_H */
