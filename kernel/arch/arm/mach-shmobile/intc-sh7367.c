
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/io.h>
#include <linux/sh_intc.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>

enum {
	UNUSED_INTCA = 0,
	ENABLED,
	DISABLED,

	/* interrupt sources INTCA */
	IRQ0A, IRQ1A, IRQ2A, IRQ3A, IRQ4A, IRQ5A, IRQ6A, IRQ7A,
	IRQ8A, IRQ9A, IRQ10A, IRQ11A, IRQ12A, IRQ13A, IRQ14A, IRQ15A,
	DIRC,
	CRYPT1_ERR, CRYPT2_STD,
	IIC1_ALI1, IIC1_TACKI1, IIC1_WAITI1, IIC1_DTEI1,
	ARM11_IRQPMU, ARM11_COMMTX, ARM11_COMMRX,
	ETM11_ACQCMP, ETM11_FULL,
	MFI_MFIM, MFI_MFIS,
	BBIF1, BBIF2,
	USBDMAC_USHDMI,
	USBHS_USHI0, USBHS_USHI1,
	CMT1_CMT10, CMT1_CMT11, CMT1_CMT12, CMT1_CMT13, CMT2, CMT3,
	KEYSC_KEY,
	SCIFA0, SCIFA1, SCIFA2, SCIFA3,
	MSIOF2, MSIOF1,
	SCIFA4, SCIFA5, SCIFB,
	FLCTL_FLSTEI, FLCTL_FLTENDI, FLCTL_FLTREQ0I, FLCTL_FLTREQ1I,
	SDHI0,
	SDHI1,
	MSU_MSU, MSU_MSU2,
	IREM,
	SIU,
	SPU,
	IRDA,
	TPU0, TPU1, TPU2, TPU3, TPU4,
	LCRC,
	PINT1, PINT2,
	TTI20,
	MISTY,
	DDM,
	SDHI2,
	RWDT0, RWDT1,
	DMAC_1_DEI0, DMAC_1_DEI1, DMAC_1_DEI2, DMAC_1_DEI3,
	DMAC_2_DEI4, DMAC_2_DEI5, DMAC_2_DADERR,
	DMAC2_1_DEI0, DMAC2_1_DEI1, DMAC2_1_DEI2, DMAC2_1_DEI3,
	DMAC2_2_DEI4, DMAC2_2_DEI5, DMAC2_2_DADERR,
	DMAC3_1_DEI0, DMAC3_1_DEI1, DMAC3_1_DEI2, DMAC3_1_DEI3,
	DMAC3_2_DEI4, DMAC3_2_DEI5, DMAC3_2_DADERR,

	/* interrupt groups INTCA */
	DMAC_1, DMAC_2,	DMAC2_1, DMAC2_2, DMAC3_1, DMAC3_2,
	ETM11, ARM11, USBHS, FLCTL, IIC1
};

static struct intc_vect intca_vectors[] = {
	INTC_VECT(IRQ0A, 0x0200), INTC_VECT(IRQ1A, 0x0220),
	INTC_VECT(IRQ2A, 0x0240), INTC_VECT(IRQ3A, 0x0260),
	INTC_VECT(IRQ4A, 0x0280), INTC_VECT(IRQ5A, 0x02a0),
	INTC_VECT(IRQ6A, 0x02c0), INTC_VECT(IRQ7A, 0x02e0),
	INTC_VECT(IRQ8A, 0x0300), INTC_VECT(IRQ9A, 0x0320),
	INTC_VECT(IRQ10A, 0x0340), INTC_VECT(IRQ11A, 0x0360),
	INTC_VECT(IRQ12A, 0x0380), INTC_VECT(IRQ13A, 0x03a0),
	INTC_VECT(IRQ14A, 0x03c0), INTC_VECT(IRQ15A, 0x03e0),
	INTC_VECT(DIRC, 0x0560),
	INTC_VECT(CRYPT1_ERR, 0x05e0),
	INTC_VECT(CRYPT2_STD, 0x0700),
	INTC_VECT(IIC1_ALI1, 0x0780), INTC_VECT(IIC1_TACKI1, 0x07a0),
	INTC_VECT(IIC1_WAITI1, 0x07c0), INTC_VECT(IIC1_DTEI1, 0x07e0),
	INTC_VECT(ARM11_IRQPMU, 0x0800), INTC_VECT(ARM11_COMMTX, 0x0840),
	INTC_VECT(ARM11_COMMRX, 0x0860),
	INTC_VECT(ETM11_ACQCMP, 0x0880), INTC_VECT(ETM11_FULL, 0x08a0),
	INTC_VECT(MFI_MFIM, 0x0900), INTC_VECT(MFI_MFIS, 0x0920),
	INTC_VECT(BBIF1, 0x0940), INTC_VECT(BBIF2, 0x0960),
	INTC_VECT(USBDMAC_USHDMI, 0x0a00),
	INTC_VECT(USBHS_USHI0, 0x0a20), INTC_VECT(USBHS_USHI1, 0x0a40),
	INTC_VECT(CMT1_CMT10, 0x0b00), INTC_VECT(CMT1_CMT11, 0x0b20),
	INTC_VECT(CMT1_CMT12, 0x0b40), INTC_VECT(CMT1_CMT13, 0x0b60),
	INTC_VECT(CMT2, 0x0b80), INTC_VECT(CMT3, 0x0ba0),
	INTC_VECT(KEYSC_KEY, 0x0be0),
	INTC_VECT(SCIFA0, 0x0c00), INTC_VECT(SCIFA1, 0x0c20),
	INTC_VECT(SCIFA2, 0x0c40), INTC_VECT(SCIFA3, 0x0c60),
	INTC_VECT(MSIOF2, 0x0c80), INTC_VECT(MSIOF1, 0x0d00),
	INTC_VECT(SCIFA4, 0x0d20), INTC_VECT(SCIFA5, 0x0d40),
	INTC_VECT(SCIFB, 0x0d60),
	INTC_VECT(FLCTL_FLSTEI, 0x0d80), INTC_VECT(FLCTL_FLTENDI, 0x0da0),
	INTC_VECT(FLCTL_FLTREQ0I, 0x0dc0), INTC_VECT(FLCTL_FLTREQ1I, 0x0de0),
	INTC_VECT(SDHI0, 0x0e00), INTC_VECT(SDHI0, 0x0e20),
	INTC_VECT(SDHI0, 0x0e40), INTC_VECT(SDHI0, 0x0e60),
	INTC_VECT(SDHI1, 0x0e80), INTC_VECT(SDHI1, 0x0ea0),
	INTC_VECT(SDHI1, 0x0ec0), INTC_VECT(SDHI1, 0x0ee0),
	INTC_VECT(MSU_MSU, 0x0f20), INTC_VECT(MSU_MSU2, 0x0f40),
	INTC_VECT(IREM, 0x0f60),
	INTC_VECT(SIU, 0x0fa0),
	INTC_VECT(SPU, 0x0fc0),
	INTC_VECT(IRDA, 0x0480),
	INTC_VECT(TPU0, 0x04a0), INTC_VECT(TPU1, 0x04c0),
	INTC_VECT(TPU2, 0x04e0), INTC_VECT(TPU3, 0x0500),
	INTC_VECT(TPU4, 0x0520),
	INTC_VECT(LCRC, 0x0540),
	INTC_VECT(PINT1, 0x1000), INTC_VECT(PINT2, 0x1020),
	INTC_VECT(TTI20, 0x1100),
	INTC_VECT(MISTY, 0x1120),
	INTC_VECT(DDM, 0x1140),
	INTC_VECT(SDHI2, 0x1200), INTC_VECT(SDHI2, 0x1220),
	INTC_VECT(SDHI2, 0x1240), INTC_VECT(SDHI2, 0x1260),
	INTC_VECT(RWDT0, 0x1280), INTC_VECT(RWDT1, 0x12a0),
	INTC_VECT(DMAC_1_DEI0, 0x2000), INTC_VECT(DMAC_1_DEI1, 0x2020),
	INTC_VECT(DMAC_1_DEI2, 0x2040), INTC_VECT(DMAC_1_DEI3, 0x2060),
	INTC_VECT(DMAC_2_DEI4, 0x2080), INTC_VECT(DMAC_2_DEI5, 0x20a0),
	INTC_VECT(DMAC_2_DADERR, 0x20c0),
	INTC_VECT(DMAC2_1_DEI0, 0x2100), INTC_VECT(DMAC2_1_DEI1, 0x2120),
	INTC_VECT(DMAC2_1_DEI2, 0x2140), INTC_VECT(DMAC2_1_DEI3, 0x2160),
	INTC_VECT(DMAC2_2_DEI4, 0x2180), INTC_VECT(DMAC2_2_DEI5, 0x21a0),
	INTC_VECT(DMAC2_2_DADERR, 0x21c0),
	INTC_VECT(DMAC3_1_DEI0, 0x2200), INTC_VECT(DMAC3_1_DEI1, 0x2220),
	INTC_VECT(DMAC3_1_DEI2, 0x2240), INTC_VECT(DMAC3_1_DEI3, 0x2260),
	INTC_VECT(DMAC3_2_DEI4, 0x2280), INTC_VECT(DMAC3_2_DEI5, 0x22a0),
	INTC_VECT(DMAC3_2_DADERR, 0x22c0),
};

static struct intc_group intca_groups[] __initdata = {
	INTC_GROUP(DMAC_1, DMAC_1_DEI0,
		   DMAC_1_DEI1, DMAC_1_DEI2, DMAC_1_DEI3),
	INTC_GROUP(DMAC_2, DMAC_2_DEI4,
		   DMAC_2_DEI5, DMAC_2_DADERR),
	INTC_GROUP(DMAC2_1, DMAC2_1_DEI0,
		   DMAC2_1_DEI1, DMAC2_1_DEI2, DMAC2_1_DEI3),
	INTC_GROUP(DMAC2_2, DMAC2_2_DEI4,
		   DMAC2_2_DEI5, DMAC2_2_DADERR),
	INTC_GROUP(DMAC3_1, DMAC3_1_DEI0,
		   DMAC3_1_DEI1, DMAC3_1_DEI2, DMAC3_1_DEI3),
	INTC_GROUP(DMAC3_2, DMAC3_2_DEI4,
		   DMAC3_2_DEI5, DMAC3_2_DADERR),
	INTC_GROUP(ETM11, ETM11_ACQCMP, ETM11_FULL),
	INTC_GROUP(ARM11, ARM11_IRQPMU, ARM11_COMMTX, ARM11_COMMTX),
	INTC_GROUP(USBHS, USBHS_USHI0, USBHS_USHI1),
	INTC_GROUP(FLCTL, FLCTL_FLSTEI, FLCTL_FLTENDI,
		   FLCTL_FLTREQ0I, FLCTL_FLTREQ1I),
	INTC_GROUP(IIC1, IIC1_ALI1, IIC1_TACKI1, IIC1_WAITI1, IIC1_DTEI1),
};

static struct intc_mask_reg intca_mask_registers[] = {
	{ 0xe6900040, 0xe6900060, 8, /* INTMSK00A / INTMSKCLR00A */
	  { IRQ0A, IRQ1A, IRQ2A, IRQ3A, IRQ4A, IRQ5A, IRQ6A, IRQ7A } },
	{ 0xe6900044, 0xe6900064, 8, /* INTMSK10A / INTMSKCLR10A */
	  { IRQ8A, IRQ9A, IRQ10A, IRQ11A, IRQ12A, IRQ13A, IRQ14A, IRQ15A } },
	{ 0xe6940080, 0xe69400c0, 8, /* IMR0A / IMCR0A */
	  { DMAC2_1_DEI3, DMAC2_1_DEI2, DMAC2_1_DEI1, DMAC2_1_DEI0,
	    ARM11_IRQPMU, 0, ARM11_COMMTX, ARM11_COMMRX } },
	{ 0xe6940084, 0xe69400c4, 8, /* IMR1A / IMCR1A */
	  { CRYPT1_ERR, CRYPT2_STD, DIRC, 0,
	    DMAC_1_DEI3, DMAC_1_DEI2, DMAC_1_DEI1, DMAC_1_DEI0 } },
	{ 0xe6940088, 0xe69400c8, 8, /* IMR2A / IMCR2A */
	  { PINT1, PINT2, 0, 0,
	    BBIF1, BBIF2, MFI_MFIS, MFI_MFIM } },
	{ 0xe694008c, 0xe69400cc, 8, /* IMR3A / IMCR3A */
	  { DMAC3_1_DEI3, DMAC3_1_DEI2, DMAC3_1_DEI1, DMAC3_1_DEI0,
	    DMAC3_2_DADERR, DMAC3_2_DEI5, DMAC3_2_DEI4, IRDA } },
	{ 0xe6940090, 0xe69400d0, 8, /* IMR4A / IMCR4A */
	  { DDM, 0, 0, 0,
	    0, 0, ETM11_FULL, ETM11_ACQCMP } },
	{ 0xe6940094, 0xe69400d4, 8, /* IMR5A / IMCR5A */
	  { KEYSC_KEY, DMAC_2_DADERR, DMAC_2_DEI5, DMAC_2_DEI4,
	    SCIFA3, SCIFA2, SCIFA1, SCIFA0 } },
	{ 0xe6940098, 0xe69400d8, 8, /* IMR6A / IMCR6A */
	  { SCIFB, SCIFA5, SCIFA4, MSIOF1,
	    0, 0, MSIOF2, 0 } },
	{ 0xe694009c, 0xe69400dc, 8, /* IMR7A / IMCR7A */
	  { DISABLED, DISABLED, ENABLED, ENABLED,
	    FLCTL_FLTREQ1I, FLCTL_FLTREQ0I, FLCTL_FLTENDI, FLCTL_FLSTEI } },
	{ 0xe69400a0, 0xe69400e0, 8, /* IMR8A / IMCR8A */
	  { DISABLED, DISABLED, ENABLED, ENABLED,
	    TTI20, USBDMAC_USHDMI, SPU, SIU } },
	{ 0xe69400a4, 0xe69400e4, 8, /* IMR9A / IMCR9A */
	  { CMT1_CMT13, CMT1_CMT12, CMT1_CMT11, CMT1_CMT10,
	    CMT2, USBHS_USHI1, USBHS_USHI0, 0 } },
	{ 0xe69400a8, 0xe69400e8, 8, /* IMR10A / IMCR10A */
	  { 0, DMAC2_2_DADERR, DMAC2_2_DEI5, DMAC2_2_DEI4,
	    0, 0, 0, 0 } },
	{ 0xe69400ac, 0xe69400ec, 8, /* IMR11A / IMCR11A */
	  { IIC1_DTEI1, IIC1_WAITI1, IIC1_TACKI1, IIC1_ALI1,
	    LCRC, MSU_MSU2, IREM, MSU_MSU } },
	{ 0xe69400b0, 0xe69400f0, 8, /* IMR12A / IMCR12A */
	  { 0, 0, TPU0, TPU1,
	    TPU2, TPU3, TPU4, 0 } },
	{ 0xe69400b4, 0xe69400f4, 8, /* IMR13A / IMCR13A */
	  { DISABLED, DISABLED, ENABLED, ENABLED,
	    MISTY, CMT3, RWDT1, RWDT0 } },
};

static struct intc_prio_reg intca_prio_registers[] = {
	{ 0xe6900010, 0, 32, 4, /* INTPRI00A */
	  { IRQ0A, IRQ1A, IRQ2A, IRQ3A, IRQ4A, IRQ5A, IRQ6A, IRQ7A } },
	{ 0xe6900014, 0, 32, 4, /* INTPRI10A */
	  { IRQ8A, IRQ9A, IRQ10A, IRQ11A, IRQ12A, IRQ13A, IRQ14A, IRQ15A } },

	{ 0xe6940000, 0, 16, 4, /* IPRAA */ { DMAC3_1, DMAC3_2, CMT2, LCRC } },
	{ 0xe6940004, 0, 16, 4, /* IPRBA */ { IRDA, ETM11, BBIF1, BBIF2 } },
	{ 0xe6940008, 0, 16, 4, /* IPRCA */ { CRYPT1_ERR, CRYPT2_STD,
					      CMT1_CMT11, ARM11 } },
	{ 0xe694000c, 0, 16, 4, /* IPRDA */ { PINT1, PINT2,
					      CMT1_CMT12, TPU4 } },
	{ 0xe6940010, 0, 16, 4, /* IPREA */ { DMAC_1, MFI_MFIS,
					      MFI_MFIM, USBHS } },
	{ 0xe6940014, 0, 16, 4, /* IPRFA */ { KEYSC_KEY, DMAC_2,
					      0, CMT1_CMT10 } },
	{ 0xe6940018, 0, 16, 4, /* IPRGA */ { SCIFA0, SCIFA1,
					      SCIFA2, SCIFA3 } },
	{ 0xe694001c, 0, 16, 4, /* IPRGH */ { MSIOF2, USBDMAC_USHDMI,
					      FLCTL, SDHI0 } },
	{ 0xe6940020, 0, 16, 4, /* IPRIA */ { MSIOF1, SCIFA4, MSU_MSU, IIC1 } },
	{ 0xe6940024, 0, 16, 4, /* IPRJA */ { DMAC2_1, DMAC2_2, SIU, TTI20 } },
	{ 0xe6940028, 0, 16, 4, /* IPRKA */ { 0, CMT1_CMT13, IREM, SDHI1 } },
	{ 0xe694002c, 0, 16, 4, /* IPRLA */ { TPU0, TPU1, TPU2, TPU3 } },
	{ 0xe6940030, 0, 16, 4, /* IPRMA */ { MISTY, CMT3, RWDT1, RWDT0 } },
	{ 0xe6940034, 0, 16, 4, /* IPRNA */ { SCIFB, SCIFA5, SPU, DDM } },
	{ 0xe6940038, 0, 16, 4, /* IPROA */ { 0, 0, DIRC, SDHI2 } },
};

static struct intc_sense_reg intca_sense_registers[] __initdata = {
	{ 0xe6900000, 16, 2, /* ICR1A */
	  { IRQ0A, IRQ1A, IRQ2A, IRQ3A, IRQ4A, IRQ5A, IRQ6A, IRQ7A } },
	{ 0xe6900004, 16, 2, /* ICR2A */
	  { IRQ8A, IRQ9A, IRQ10A, IRQ11A, IRQ12A, IRQ13A, IRQ14A, IRQ15A } },
};

static struct intc_mask_reg intca_ack_registers[] __initdata = {
	{ 0xe6900020, 0, 8, /* INTREQ00A */
	  { IRQ0A, IRQ1A, IRQ2A, IRQ3A, IRQ4A, IRQ5A, IRQ6A, IRQ7A } },
	{ 0xe6900024, 0, 8, /* INTREQ10A */
	  { IRQ8A, IRQ9A, IRQ10A, IRQ11A, IRQ12A, IRQ13A, IRQ14A, IRQ15A } },
};

static struct intc_desc intca_desc __initdata = {
	.name = "sh7367-intca",
	.force_enable = ENABLED,
	.force_disable = DISABLED,
	.hw = INTC_HW_DESC(intca_vectors, intca_groups,
			   intca_mask_registers, intca_prio_registers,
			   intca_sense_registers, intca_ack_registers),
};

void __init sh7367_init_irq(void)
{
	/* INTCA */
	register_intc_controller(&intca_desc);
}
