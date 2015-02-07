
#ifndef __iop_sap_in_defs_asm_h
#define __iop_sap_in_defs_asm_h


#ifndef REG_FIELD
#define REG_FIELD( scope, reg, field, value ) \
  REG_FIELD_X_( value, reg_##scope##_##reg##___##field##___lsb )
#define REG_FIELD_X_( value, shift ) ((value) << shift)
#endif

#ifndef REG_STATE
#define REG_STATE( scope, reg, field, symbolic_value ) \
  REG_STATE_X_( regk_##scope##_##symbolic_value, reg_##scope##_##reg##___##field##___lsb )
#define REG_STATE_X_( k, shift ) (k << shift)
#endif

#ifndef REG_MASK
#define REG_MASK( scope, reg, field ) \
  REG_MASK_X_( reg_##scope##_##reg##___##field##___width, reg_##scope##_##reg##___##field##___lsb )
#define REG_MASK_X_( width, lsb ) (((1 << width)-1) << lsb)
#endif

#ifndef REG_LSB
#define REG_LSB( scope, reg, field ) reg_##scope##_##reg##___##field##___lsb
#endif

#ifndef REG_BIT
#define REG_BIT( scope, reg, field ) reg_##scope##_##reg##___##field##___bit
#endif

#ifndef REG_ADDR
#define REG_ADDR( scope, inst, reg ) REG_ADDR_X_(inst, reg_##scope##_##reg##_offset)
#define REG_ADDR_X_( inst, offs ) ((inst) + offs)
#endif

#ifndef REG_ADDR_VECT
#define REG_ADDR_VECT( scope, inst, reg, index ) \
         REG_ADDR_VECT_X_(inst, reg_##scope##_##reg##_offset, index, \
			 STRIDE_##scope##_##reg )
#define REG_ADDR_VECT_X_( inst, offs, index, stride ) \
                          ((inst) + offs + (index) * stride)
#endif

#define STRIDE_iop_sap_in_rw_bus_byte 4
/* Register rw_bus_byte, scope iop_sap_in, type rw */
#define reg_iop_sap_in_rw_bus_byte___sync_sel___lsb 0
#define reg_iop_sap_in_rw_bus_byte___sync_sel___width 2
#define reg_iop_sap_in_rw_bus_byte___sync_ext_src___lsb 2
#define reg_iop_sap_in_rw_bus_byte___sync_ext_src___width 3
#define reg_iop_sap_in_rw_bus_byte___sync_edge___lsb 5
#define reg_iop_sap_in_rw_bus_byte___sync_edge___width 2
#define reg_iop_sap_in_rw_bus_byte___delay___lsb 7
#define reg_iop_sap_in_rw_bus_byte___delay___width 2
#define reg_iop_sap_in_rw_bus_byte_offset 0

#define STRIDE_iop_sap_in_rw_gio 4
/* Register rw_gio, scope iop_sap_in, type rw */
#define reg_iop_sap_in_rw_gio___sync_sel___lsb 0
#define reg_iop_sap_in_rw_gio___sync_sel___width 2
#define reg_iop_sap_in_rw_gio___sync_ext_src___lsb 2
#define reg_iop_sap_in_rw_gio___sync_ext_src___width 3
#define reg_iop_sap_in_rw_gio___sync_edge___lsb 5
#define reg_iop_sap_in_rw_gio___sync_edge___width 2
#define reg_iop_sap_in_rw_gio___delay___lsb 7
#define reg_iop_sap_in_rw_gio___delay___width 2
#define reg_iop_sap_in_rw_gio___logic___lsb 9
#define reg_iop_sap_in_rw_gio___logic___width 2
#define reg_iop_sap_in_rw_gio_offset 16


/* Constants */
#define regk_iop_sap_in_and                       0x00000002
#define regk_iop_sap_in_ext_clk200                0x00000003
#define regk_iop_sap_in_gio0                      0x00000000
#define regk_iop_sap_in_gio12                     0x00000003
#define regk_iop_sap_in_gio16                     0x00000004
#define regk_iop_sap_in_gio20                     0x00000005
#define regk_iop_sap_in_gio24                     0x00000006
#define regk_iop_sap_in_gio28                     0x00000007
#define regk_iop_sap_in_gio4                      0x00000001
#define regk_iop_sap_in_gio8                      0x00000002
#define regk_iop_sap_in_inv                       0x00000001
#define regk_iop_sap_in_neg                       0x00000002
#define regk_iop_sap_in_no                        0x00000000
#define regk_iop_sap_in_no_del_ext_clk200         0x00000002
#define regk_iop_sap_in_none                      0x00000000
#define regk_iop_sap_in_one                       0x00000001
#define regk_iop_sap_in_or                        0x00000003
#define regk_iop_sap_in_pos                       0x00000001
#define regk_iop_sap_in_pos_neg                   0x00000003
#define regk_iop_sap_in_rw_bus_byte_default       0x00000000
#define regk_iop_sap_in_rw_bus_byte_size          0x00000004
#define regk_iop_sap_in_rw_gio_default            0x00000000
#define regk_iop_sap_in_rw_gio_size               0x00000020
#define regk_iop_sap_in_timer_grp0_tmr3           0x00000000
#define regk_iop_sap_in_timer_grp1_tmr3           0x00000001
#define regk_iop_sap_in_tmr_clk200                0x00000001
#define regk_iop_sap_in_two                       0x00000002
#define regk_iop_sap_in_two_clk200                0x00000000
#endif /* __iop_sap_in_defs_asm_h */
