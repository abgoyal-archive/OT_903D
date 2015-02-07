
#ifndef __iop_fifo_out_extra_defs_h
#define __iop_fifo_out_extra_defs_h

/* Main access macros */
#ifndef REG_RD
#define REG_RD( scope, inst, reg ) \
  REG_READ( reg_##scope##_##reg, \
            (inst) + REG_RD_ADDR_##scope##_##reg )
#endif

#ifndef REG_WR
#define REG_WR( scope, inst, reg, val ) \
  REG_WRITE( reg_##scope##_##reg, \
             (inst) + REG_WR_ADDR_##scope##_##reg, (val) )
#endif

#ifndef REG_RD_VECT
#define REG_RD_VECT( scope, inst, reg, index ) \
  REG_READ( reg_##scope##_##reg, \
            (inst) + REG_RD_ADDR_##scope##_##reg + \
	    (index) * STRIDE_##scope##_##reg )
#endif

#ifndef REG_WR_VECT
#define REG_WR_VECT( scope, inst, reg, index, val ) \
  REG_WRITE( reg_##scope##_##reg, \
             (inst) + REG_WR_ADDR_##scope##_##reg + \
	     (index) * STRIDE_##scope##_##reg, (val) )
#endif

#ifndef REG_RD_INT
#define REG_RD_INT( scope, inst, reg ) \
  REG_READ( int, (inst) + REG_RD_ADDR_##scope##_##reg )
#endif

#ifndef REG_WR_INT
#define REG_WR_INT( scope, inst, reg, val ) \
  REG_WRITE( int, (inst) + REG_WR_ADDR_##scope##_##reg, (val) )
#endif

#ifndef REG_RD_INT_VECT
#define REG_RD_INT_VECT( scope, inst, reg, index ) \
  REG_READ( int, (inst) + REG_RD_ADDR_##scope##_##reg + \
	    (index) * STRIDE_##scope##_##reg )
#endif

#ifndef REG_WR_INT_VECT
#define REG_WR_INT_VECT( scope, inst, reg, index, val ) \
  REG_WRITE( int, (inst) + REG_WR_ADDR_##scope##_##reg + \
	     (index) * STRIDE_##scope##_##reg, (val) )
#endif

#ifndef REG_TYPE_CONV
#define REG_TYPE_CONV( type, orgtype, val ) \
  ( { union { orgtype o; type n; } r; r.o = val; r.n; } )
#endif

#ifndef reg_page_size
#define reg_page_size 8192
#endif

#ifndef REG_ADDR
#define REG_ADDR( scope, inst, reg ) \
  ( (inst) + REG_RD_ADDR_##scope##_##reg )
#endif

#ifndef REG_ADDR_VECT
#define REG_ADDR_VECT( scope, inst, reg, index ) \
  ( (inst) + REG_RD_ADDR_##scope##_##reg + \
    (index) * STRIDE_##scope##_##reg )
#endif

/* C-code for register scope iop_fifo_out_extra */

/* Register rs_rd_data, scope iop_fifo_out_extra, type rs */
typedef unsigned int reg_iop_fifo_out_extra_rs_rd_data;
#define REG_RD_ADDR_iop_fifo_out_extra_rs_rd_data 0

/* Register r_rd_data, scope iop_fifo_out_extra, type r */
typedef unsigned int reg_iop_fifo_out_extra_r_rd_data;
#define REG_RD_ADDR_iop_fifo_out_extra_r_rd_data 4

/* Register r_stat, scope iop_fifo_out_extra, type r */
typedef struct {
  unsigned int avail_bytes    : 4;
  unsigned int last           : 8;
  unsigned int dif_in_en      : 1;
  unsigned int dif_out_en     : 1;
  unsigned int zero_data_last : 1;
  unsigned int dummy1         : 17;
} reg_iop_fifo_out_extra_r_stat;
#define REG_RD_ADDR_iop_fifo_out_extra_r_stat 8

/* Register rw_strb_dif_out, scope iop_fifo_out_extra, type rw */
typedef unsigned int reg_iop_fifo_out_extra_rw_strb_dif_out;
#define REG_RD_ADDR_iop_fifo_out_extra_rw_strb_dif_out 12
#define REG_WR_ADDR_iop_fifo_out_extra_rw_strb_dif_out 12

/* Register rw_intr_mask, scope iop_fifo_out_extra, type rw */
typedef struct {
  unsigned int urun      : 1;
  unsigned int last_data : 1;
  unsigned int dav       : 1;
  unsigned int free      : 1;
  unsigned int orun      : 1;
  unsigned int dummy1    : 27;
} reg_iop_fifo_out_extra_rw_intr_mask;
#define REG_RD_ADDR_iop_fifo_out_extra_rw_intr_mask 16
#define REG_WR_ADDR_iop_fifo_out_extra_rw_intr_mask 16

/* Register rw_ack_intr, scope iop_fifo_out_extra, type rw */
typedef struct {
  unsigned int urun      : 1;
  unsigned int last_data : 1;
  unsigned int dav       : 1;
  unsigned int free      : 1;
  unsigned int orun      : 1;
  unsigned int dummy1    : 27;
} reg_iop_fifo_out_extra_rw_ack_intr;
#define REG_RD_ADDR_iop_fifo_out_extra_rw_ack_intr 20
#define REG_WR_ADDR_iop_fifo_out_extra_rw_ack_intr 20

/* Register r_intr, scope iop_fifo_out_extra, type r */
typedef struct {
  unsigned int urun      : 1;
  unsigned int last_data : 1;
  unsigned int dav       : 1;
  unsigned int free      : 1;
  unsigned int orun      : 1;
  unsigned int dummy1    : 27;
} reg_iop_fifo_out_extra_r_intr;
#define REG_RD_ADDR_iop_fifo_out_extra_r_intr 24

/* Register r_masked_intr, scope iop_fifo_out_extra, type r */
typedef struct {
  unsigned int urun      : 1;
  unsigned int last_data : 1;
  unsigned int dav       : 1;
  unsigned int free      : 1;
  unsigned int orun      : 1;
  unsigned int dummy1    : 27;
} reg_iop_fifo_out_extra_r_masked_intr;
#define REG_RD_ADDR_iop_fifo_out_extra_r_masked_intr 28


/* Constants */
enum {
  regk_iop_fifo_out_extra_no               = 0x00000000,
  regk_iop_fifo_out_extra_rw_intr_mask_default = 0x00000000,
  regk_iop_fifo_out_extra_yes              = 0x00000001
};
#endif /* __iop_fifo_out_extra_defs_h */
