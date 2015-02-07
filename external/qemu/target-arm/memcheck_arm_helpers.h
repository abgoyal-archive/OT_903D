


#ifndef QEMU_TARGET_ARM_MEMCHECK_ARM_HELPERS_H
#define QEMU_TARGET_ARM_MEMCHECK_ARM_HELPERS_H

#ifndef CONFIG_MEMCHECK
#error CONFIG_MEMCHECK is not defined.
#endif  // CONFIG_MEMCHECK

#include "helpers.h"
#include "memcheck/memcheck_api.h"

/* Array of return addresses detected in gen_intermediate_code_internal. */
AddrArray   ret_addresses = { 0 };

static inline int
watch_call_stack(DisasContext *s)
{
    if (!memcheck_enabled || !memcheck_watch_call_stack) {
        return 0;
    }

#ifndef CONFIG_USER_ONLY
    if (!s->user) {
        /* We're not interested in kernel mode CPU stack. */
        return 0;
    }
#endif  // CONFIG_USER_ONLY

    /* We're not interested in kernel code stack (pc >= 0xC0000000).
     * Android specific: We're also not interested in android linker stack
     * (0xB0000000 - 0xB00FFFFF) */
    if (s->pc >= 0xC0000000 || (0xB0000000 <= s->pc && s->pc <= 0xB00FFFFF)) {
        return 0;
    }
    return 1;
}

static inline int
is_arm_bl_or_blx(uint32_t insn)
{
    /* ARM BL  (immediate): xxxx 1011 xxxx xxxx xxxx xxxx xxxx xxxx
     * ARM BLX (immediate): 1111 101x xxxx xxxx xxxx xxxx xxxx xxxx
     * ARM BLX (register):  xxxx 0001 0010 xxxx xxxx xxxx 0011 xxxx
     */
    if ((insn & 0x0F000000) == 0x0B000000 ||    // ARM BL (imm)
        (insn & 0xFE000000) == 0xFA000000 ||    // ARM BLX (imm)
        (insn & 0x0FF000F0) == 0x12000030) {    // ARM BLX (reg)
        return 1;
    }
    return 0;
}

static inline int
is_thumb_bl_or_blx(uint16_t insn, target_ulong pc, target_ulong* ret_off)
{
    /* THUMB BLX(register):      0100 0111 1xxx xxxx
     * THUMB BL(1-stimmediate):  1111 0xxx xxxx xxxx
     * THUMB BLX(1-stimmediate): 1111 0xxx xxxx xxxx
     */
    if ((insn & 0xFF80) == 0x4780) {            // THUMB BLX(reg)
        *ret_off = 2;
        return 1;
    } else if ((insn & 0xF800) == 0xF000) {     // THUMB BL(X)(imm)
        // This is a 32-bit THUMB. Get the second half of the instuction.
        insn = lduw_code(pc + 2);
        if ((insn & 0xC000) == 0xC000) {
            *ret_off = 4;
            return 1;
        }
    }
    return 0;
}

static int
register_ret_address(CPUState* env, target_ulong addr)
{
    int ret;
    if ((0x90000000 <= addr && addr <= 0xBFFFFFFF)) {
        /* Address belongs to a module that always loads at this fixed address.
         * So, we can keep this address in the global array. */
        ret = addrarray_add(&ret_addresses, get_phys_addr_code(env, addr));
    } else {
        ret = addrarray_add(&ret_addresses, get_phys_addr_code(env, addr));
    }
    assert(ret != 0);

    if (ret == 1) {
        /* If this ret address has been added to the array, we need to make sure
         * that all TBs that contain translated code for that address are
         * invalidated. This will force retranslation of that code, which will
         * make sure that our ret callback is set. This is also important part
         * in keeping consistency between translated code, and intermediate code
         * generated for guest PC calculation. If we don't invalidate TBs, and
         * PC calculation code is generated, there will be inconsistency due to
         * the fact that TB code doesn't contain ret callback, while PC calc
         * code contains it. This inconsistency will lead to an immanent
         * segmentation fault.*/
        TranslationBlock* tb;
        const target_ulong phys_pc = get_phys_addr_code(env, addr);
        const target_ulong phys_page1 = phys_pc & TARGET_PAGE_MASK;

        for(tb = tb_phys_hash[tb_phys_hash_func(phys_pc)]; tb != NULL;
            tb = tb->phys_hash_next) {
            if (tb->pc == addr && tb->page_addr[0] == phys_page1) {
                tb_phys_invalidate(tb, -1);
            }
        }
    }
    return ret;
}

static inline int
is_ret_address(CPUState* env, target_ulong addr)
{
    if ((0x90000000 <= addr && addr <= 0xBFFFFFFF)) {
        return addrarray_check(&ret_addresses, get_phys_addr_code(env, addr));
    } else {
        return addrarray_check(&ret_addresses, get_phys_addr_code(env, addr));
    }
}

/* Adds "on_call" callback into generated intermediate code. */
static inline void
set_on_call(target_ulong pc, target_ulong ret)
{
    TCGv_ptr tmp_pc = tcg_const_ptr(pc & ~1);
    TCGv_ptr tmp_ret = tcg_const_ptr(ret & ~1);

    gen_helper_on_call(tmp_pc, tmp_ret);

    tcg_temp_free_ptr(tmp_ret);
    tcg_temp_free_ptr(tmp_pc);
}

/* Adds "on_ret" callback into generated intermediate code. */
static inline void
set_on_ret(target_ulong ret)
{
    TCGv_ptr tmp_ret = tcg_const_ptr(ret & ~1);

    gen_helper_on_ret(tmp_ret);

    tcg_temp_free_ptr(tmp_ret);
}

#endif  // QEMU_TARGET_ARM_MEMCHECK_ARM_HELPERS_H
