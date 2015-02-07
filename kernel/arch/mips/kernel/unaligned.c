
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/signal.h>
#include <linux/smp.h>
#include <linux/sched.h>
#include <linux/debugfs.h>
#include <trace/trap.h>
#include <asm/asm.h>
#include <asm/branch.h>
#include <asm/byteorder.h>
#include <asm/cop2.h>
#include <asm/inst.h>
#include <asm/uaccess.h>
#include <asm/system.h>

#define STR(x)  __STR(x)
#define __STR(x)  #x

enum {
	UNALIGNED_ACTION_QUIET,
	UNALIGNED_ACTION_SIGNAL,
	UNALIGNED_ACTION_SHOW,
};
#ifdef CONFIG_DEBUG_FS
static u32 unaligned_instructions;
static u32 unaligned_action;
#else
#define unaligned_action UNALIGNED_ACTION_QUIET
#endif
extern void show_registers(struct pt_regs *regs);

static void emulate_load_store_insn(struct pt_regs *regs,
	void __user *addr, unsigned int __user *pc)
{
	union mips_instruction insn;
	unsigned long value;
	unsigned int res;

	regs->regs[0] = 0;

	/*
	 * This load never faults.
	 */
	__get_user(insn.word, pc);

	switch (insn.i_format.opcode) {
	/*
	 * These are instructions that a compiler doesn't generate.  We
	 * can assume therefore that the code is MIPS-aware and
	 * really buggy.  Emulating these instructions would break the
	 * semantics anyway.
	 */
	case ll_op:
	case lld_op:
	case sc_op:
	case scd_op:

	/*
	 * For these instructions the only way to create an address
	 * error is an attempted access to kernel/supervisor address
	 * space.
	 */
	case ldl_op:
	case ldr_op:
	case lwl_op:
	case lwr_op:
	case sdl_op:
	case sdr_op:
	case swl_op:
	case swr_op:
	case lb_op:
	case lbu_op:
	case sb_op:
		goto sigbus;

	/*
	 * The remaining opcodes are the ones that are really of interest.
	 */
	case lh_op:
		if (!access_ok(VERIFY_READ, addr, 2))
			goto sigbus;

		__asm__ __volatile__ (".set\tnoat\n"
#ifdef __BIG_ENDIAN
			"1:\tlb\t%0, 0(%2)\n"
			"2:\tlbu\t$1, 1(%2)\n\t"
#endif
#ifdef __LITTLE_ENDIAN
			"1:\tlb\t%0, 1(%2)\n"
			"2:\tlbu\t$1, 0(%2)\n\t"
#endif
			"sll\t%0, 0x8\n\t"
			"or\t%0, $1\n\t"
			"li\t%1, 0\n"
			"3:\t.set\tat\n\t"
			".section\t.fixup,\"ax\"\n\t"
			"4:\tli\t%1, %3\n\t"
			"j\t3b\n\t"
			".previous\n\t"
			".section\t__ex_table,\"a\"\n\t"
			STR(PTR)"\t1b, 4b\n\t"
			STR(PTR)"\t2b, 4b\n\t"
			".previous"
			: "=&r" (value), "=r" (res)
			: "r" (addr), "i" (-EFAULT));
		if (res)
			goto fault;
		compute_return_epc(regs);
		regs->regs[insn.i_format.rt] = value;
		break;

	case lw_op:
		if (!access_ok(VERIFY_READ, addr, 4))
			goto sigbus;

		__asm__ __volatile__ (
#ifdef __BIG_ENDIAN
			"1:\tlwl\t%0, (%2)\n"
			"2:\tlwr\t%0, 3(%2)\n\t"
#endif
#ifdef __LITTLE_ENDIAN
			"1:\tlwl\t%0, 3(%2)\n"
			"2:\tlwr\t%0, (%2)\n\t"
#endif
			"li\t%1, 0\n"
			"3:\t.section\t.fixup,\"ax\"\n\t"
			"4:\tli\t%1, %3\n\t"
			"j\t3b\n\t"
			".previous\n\t"
			".section\t__ex_table,\"a\"\n\t"
			STR(PTR)"\t1b, 4b\n\t"
			STR(PTR)"\t2b, 4b\n\t"
			".previous"
			: "=&r" (value), "=r" (res)
			: "r" (addr), "i" (-EFAULT));
		if (res)
			goto fault;
		compute_return_epc(regs);
		regs->regs[insn.i_format.rt] = value;
		break;

	case lhu_op:
		if (!access_ok(VERIFY_READ, addr, 2))
			goto sigbus;

		__asm__ __volatile__ (
			".set\tnoat\n"
#ifdef __BIG_ENDIAN
			"1:\tlbu\t%0, 0(%2)\n"
			"2:\tlbu\t$1, 1(%2)\n\t"
#endif
#ifdef __LITTLE_ENDIAN
			"1:\tlbu\t%0, 1(%2)\n"
			"2:\tlbu\t$1, 0(%2)\n\t"
#endif
			"sll\t%0, 0x8\n\t"
			"or\t%0, $1\n\t"
			"li\t%1, 0\n"
			"3:\t.set\tat\n\t"
			".section\t.fixup,\"ax\"\n\t"
			"4:\tli\t%1, %3\n\t"
			"j\t3b\n\t"
			".previous\n\t"
			".section\t__ex_table,\"a\"\n\t"
			STR(PTR)"\t1b, 4b\n\t"
			STR(PTR)"\t2b, 4b\n\t"
			".previous"
			: "=&r" (value), "=r" (res)
			: "r" (addr), "i" (-EFAULT));
		if (res)
			goto fault;
		compute_return_epc(regs);
		regs->regs[insn.i_format.rt] = value;
		break;

	case lwu_op:
#ifdef CONFIG_64BIT
		/*
		 * A 32-bit kernel might be running on a 64-bit processor.  But
		 * if we're on a 32-bit processor and an i-cache incoherency
		 * or race makes us see a 64-bit instruction here the sdl/sdr
		 * would blow up, so for now we don't handle unaligned 64-bit
		 * instructions on 32-bit kernels.
		 */
		if (!access_ok(VERIFY_READ, addr, 4))
			goto sigbus;

		__asm__ __volatile__ (
#ifdef __BIG_ENDIAN
			"1:\tlwl\t%0, (%2)\n"
			"2:\tlwr\t%0, 3(%2)\n\t"
#endif
#ifdef __LITTLE_ENDIAN
			"1:\tlwl\t%0, 3(%2)\n"
			"2:\tlwr\t%0, (%2)\n\t"
#endif
			"dsll\t%0, %0, 32\n\t"
			"dsrl\t%0, %0, 32\n\t"
			"li\t%1, 0\n"
			"3:\t.section\t.fixup,\"ax\"\n\t"
			"4:\tli\t%1, %3\n\t"
			"j\t3b\n\t"
			".previous\n\t"
			".section\t__ex_table,\"a\"\n\t"
			STR(PTR)"\t1b, 4b\n\t"
			STR(PTR)"\t2b, 4b\n\t"
			".previous"
			: "=&r" (value), "=r" (res)
			: "r" (addr), "i" (-EFAULT));
		if (res)
			goto fault;
		compute_return_epc(regs);
		regs->regs[insn.i_format.rt] = value;
		break;
#endif /* CONFIG_64BIT */

		/* Cannot handle 64-bit instructions in 32-bit kernel */
		goto sigill;

	case ld_op:
#ifdef CONFIG_64BIT
		/*
		 * A 32-bit kernel might be running on a 64-bit processor.  But
		 * if we're on a 32-bit processor and an i-cache incoherency
		 * or race makes us see a 64-bit instruction here the sdl/sdr
		 * would blow up, so for now we don't handle unaligned 64-bit
		 * instructions on 32-bit kernels.
		 */
		if (!access_ok(VERIFY_READ, addr, 8))
			goto sigbus;

		__asm__ __volatile__ (
#ifdef __BIG_ENDIAN
			"1:\tldl\t%0, (%2)\n"
			"2:\tldr\t%0, 7(%2)\n\t"
#endif
#ifdef __LITTLE_ENDIAN
			"1:\tldl\t%0, 7(%2)\n"
			"2:\tldr\t%0, (%2)\n\t"
#endif
			"li\t%1, 0\n"
			"3:\t.section\t.fixup,\"ax\"\n\t"
			"4:\tli\t%1, %3\n\t"
			"j\t3b\n\t"
			".previous\n\t"
			".section\t__ex_table,\"a\"\n\t"
			STR(PTR)"\t1b, 4b\n\t"
			STR(PTR)"\t2b, 4b\n\t"
			".previous"
			: "=&r" (value), "=r" (res)
			: "r" (addr), "i" (-EFAULT));
		if (res)
			goto fault;
		compute_return_epc(regs);
		regs->regs[insn.i_format.rt] = value;
		break;
#endif /* CONFIG_64BIT */

		/* Cannot handle 64-bit instructions in 32-bit kernel */
		goto sigill;

	case sh_op:
		if (!access_ok(VERIFY_WRITE, addr, 2))
			goto sigbus;

		value = regs->regs[insn.i_format.rt];
		__asm__ __volatile__ (
#ifdef __BIG_ENDIAN
			".set\tnoat\n"
			"1:\tsb\t%1, 1(%2)\n\t"
			"srl\t$1, %1, 0x8\n"
			"2:\tsb\t$1, 0(%2)\n\t"
			".set\tat\n\t"
#endif
#ifdef __LITTLE_ENDIAN
			".set\tnoat\n"
			"1:\tsb\t%1, 0(%2)\n\t"
			"srl\t$1,%1, 0x8\n"
			"2:\tsb\t$1, 1(%2)\n\t"
			".set\tat\n\t"
#endif
			"li\t%0, 0\n"
			"3:\n\t"
			".section\t.fixup,\"ax\"\n\t"
			"4:\tli\t%0, %3\n\t"
			"j\t3b\n\t"
			".previous\n\t"
			".section\t__ex_table,\"a\"\n\t"
			STR(PTR)"\t1b, 4b\n\t"
			STR(PTR)"\t2b, 4b\n\t"
			".previous"
			: "=r" (res)
			: "r" (value), "r" (addr), "i" (-EFAULT));
		if (res)
			goto fault;
		compute_return_epc(regs);
		break;

	case sw_op:
		if (!access_ok(VERIFY_WRITE, addr, 4))
			goto sigbus;

		value = regs->regs[insn.i_format.rt];
		__asm__ __volatile__ (
#ifdef __BIG_ENDIAN
			"1:\tswl\t%1,(%2)\n"
			"2:\tswr\t%1, 3(%2)\n\t"
#endif
#ifdef __LITTLE_ENDIAN
			"1:\tswl\t%1, 3(%2)\n"
			"2:\tswr\t%1, (%2)\n\t"
#endif
			"li\t%0, 0\n"
			"3:\n\t"
			".section\t.fixup,\"ax\"\n\t"
			"4:\tli\t%0, %3\n\t"
			"j\t3b\n\t"
			".previous\n\t"
			".section\t__ex_table,\"a\"\n\t"
			STR(PTR)"\t1b, 4b\n\t"
			STR(PTR)"\t2b, 4b\n\t"
			".previous"
		: "=r" (res)
		: "r" (value), "r" (addr), "i" (-EFAULT));
		if (res)
			goto fault;
		compute_return_epc(regs);
		break;

	case sd_op:
#ifdef CONFIG_64BIT
		/*
		 * A 32-bit kernel might be running on a 64-bit processor.  But
		 * if we're on a 32-bit processor and an i-cache incoherency
		 * or race makes us see a 64-bit instruction here the sdl/sdr
		 * would blow up, so for now we don't handle unaligned 64-bit
		 * instructions on 32-bit kernels.
		 */
		if (!access_ok(VERIFY_WRITE, addr, 8))
			goto sigbus;

		value = regs->regs[insn.i_format.rt];
		__asm__ __volatile__ (
#ifdef __BIG_ENDIAN
			"1:\tsdl\t%1,(%2)\n"
			"2:\tsdr\t%1, 7(%2)\n\t"
#endif
#ifdef __LITTLE_ENDIAN
			"1:\tsdl\t%1, 7(%2)\n"
			"2:\tsdr\t%1, (%2)\n\t"
#endif
			"li\t%0, 0\n"
			"3:\n\t"
			".section\t.fixup,\"ax\"\n\t"
			"4:\tli\t%0, %3\n\t"
			"j\t3b\n\t"
			".previous\n\t"
			".section\t__ex_table,\"a\"\n\t"
			STR(PTR)"\t1b, 4b\n\t"
			STR(PTR)"\t2b, 4b\n\t"
			".previous"
		: "=r" (res)
		: "r" (value), "r" (addr), "i" (-EFAULT));
		if (res)
			goto fault;
		compute_return_epc(regs);
		break;
#endif /* CONFIG_64BIT */

		/* Cannot handle 64-bit instructions in 32-bit kernel */
		goto sigill;

	case lwc1_op:
	case ldc1_op:
	case swc1_op:
	case sdc1_op:
		/*
		 * I herewith declare: this does not happen.  So send SIGBUS.
		 */
		goto sigbus;

	/*
	 * COP2 is available to implementor for application specific use.
	 * It's up to applications to register a notifier chain and do
	 * whatever they have to do, including possible sending of signals.
	 */
	case lwc2_op:
		cu2_notifier_call_chain(CU2_LWC2_OP, regs);
		break;

	case ldc2_op:
		cu2_notifier_call_chain(CU2_LDC2_OP, regs);
		break;

	case swc2_op:
		cu2_notifier_call_chain(CU2_SWC2_OP, regs);
		break;

	case sdc2_op:
		cu2_notifier_call_chain(CU2_SDC2_OP, regs);
		break;

	default:
		/*
		 * Pheeee...  We encountered an yet unknown instruction or
		 * cache coherence problem.  Die sucker, die ...
		 */
		goto sigill;
	}

#ifdef CONFIG_DEBUG_FS
	unaligned_instructions++;
#endif

	return;

fault:
	/* Did we have an exception handler installed? */
	if (fixup_exception(regs))
		return;

	die_if_kernel("Unhandled kernel unaligned access", regs);
	force_sig(SIGSEGV, current);

	return;

sigbus:
	die_if_kernel("Unhandled kernel unaligned access", regs);
	force_sig(SIGBUS, current);

	return;

sigill:
	die_if_kernel("Unhandled kernel unaligned access or invalid instruction", regs);
	force_sig(SIGILL, current);
}

asmlinkage void do_ade(struct pt_regs *regs)
{
	unsigned int __user *pc;
	mm_segment_t seg;

	trace_trap_entry(regs, CAUSE_EXCCODE(regs->cp0_cause));

	/*
	 * Did we catch a fault trying to load an instruction?
	 * Or are we running in MIPS16 mode?
	 */
	if ((regs->cp0_badvaddr == regs->cp0_epc) || (regs->cp0_epc & 0x1))
		goto sigbus;

	pc = (unsigned int __user *) exception_epc(regs);
	if (user_mode(regs) && !test_thread_flag(TIF_FIXADE))
		goto sigbus;
	if (unaligned_action == UNALIGNED_ACTION_SIGNAL)
		goto sigbus;
	else if (unaligned_action == UNALIGNED_ACTION_SHOW)
		show_registers(regs);

	/*
	 * Do branch emulation only if we didn't forward the exception.
	 * This is all so but ugly ...
	 */
	seg = get_fs();
	if (!user_mode(regs))
		set_fs(KERNEL_DS);
	emulate_load_store_insn(regs, (void __user *)regs->cp0_badvaddr, pc);
	set_fs(seg);

	trace_trap_exit();

	return;

sigbus:
	die_if_kernel("Kernel unaligned instruction access", regs);
	force_sig(SIGBUS, current);

	/*
	 * XXX On return from the signal handler we should advance the epc
	 */

	trace_trap_exit();
}

#ifdef CONFIG_DEBUG_FS
extern struct dentry *mips_debugfs_dir;
static int __init debugfs_unaligned(void)
{
	struct dentry *d;

	if (!mips_debugfs_dir)
		return -ENODEV;
	d = debugfs_create_u32("unaligned_instructions", S_IRUGO,
			       mips_debugfs_dir, &unaligned_instructions);
	if (!d)
		return -ENOMEM;
	d = debugfs_create_u32("unaligned_action", S_IRUGO | S_IWUSR,
			       mips_debugfs_dir, &unaligned_action);
	if (!d)
		return -ENOMEM;
	return 0;
}
__initcall(debugfs_unaligned);
#endif
