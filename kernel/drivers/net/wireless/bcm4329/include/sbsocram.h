


#ifndef	_SBSOCRAM_H
#define	_SBSOCRAM_H

#ifndef _LANGUAGE_ASSEMBLY


#ifndef PAD
#define	_PADLINE(line)	pad ## line
#define	_XSTR(line)	_PADLINE(line)
#define	PAD		_XSTR(__LINE__)
#endif	


typedef volatile struct sbsocramregs {
	uint32	coreinfo;
	uint32	bwalloc;
	uint32	extracoreinfo;
	uint32	biststat;
	uint32	bankidx;
	uint32	standbyctrl;

	uint32	errlogstatus;	
	uint32	errlogaddr;	
	
	uint32	cambankidx;
	uint32	cambankstandbyctrl;
	uint32	cambankpatchctrl;
	uint32	cambankpatchtblbaseaddr;
	uint32	cambankcmdreg;
	uint32	cambankdatareg;
	uint32	cambankmaskreg;
	uint32	PAD[17];
	uint32	extmemconfig;
	uint32	extmemparitycsr;
	uint32	extmemparityerrdata;
	uint32	extmemparityerrcnt;
	uint32	extmemwrctrlandsize;
	uint32	PAD[84];
	uint32	workaround;
	uint32	pwrctl;		
} sbsocramregs_t;

#endif	


#define	SR_COREINFO		0x00
#define	SR_BWALLOC		0x04
#define	SR_BISTSTAT		0x0c
#define	SR_BANKINDEX		0x10
#define	SR_BANKSTBYCTL		0x14
#define SR_PWRCTL		0x1e8


#define	SRCI_PT_MASK		0x00070000	
#define	SRCI_PT_SHIFT		16

#define SRCI_PT_OCP_OCP		0
#define SRCI_PT_AXI_OCP		1
#define SRCI_PT_ARM7AHB_OCP	2
#define SRCI_PT_CM3AHB_OCP	3
#define SRCI_PT_AXI_AXI		4
#define SRCI_PT_AHB_AXI		5

#define SRCI_LSS_MASK		0x00f00000
#define SRCI_LSS_SHIFT		20
#define SRCI_LRS_MASK		0x0f000000
#define SRCI_LRS_SHIFT		24


#define	SRCI_MS0_MASK		0xf
#define SR_MS0_BASE		16


#define	SRCI_ROMNB_MASK		0xf000
#define	SRCI_ROMNB_SHIFT	12
#define	SRCI_ROMBSZ_MASK	0xf00
#define	SRCI_ROMBSZ_SHIFT	8
#define	SRCI_SRNB_MASK		0xf0
#define	SRCI_SRNB_SHIFT		4
#define	SRCI_SRBSZ_MASK		0xf
#define	SRCI_SRBSZ_SHIFT	0

#define SR_BSZ_BASE		14


#define	SRSC_SBYOVR_MASK	0x80000000
#define	SRSC_SBYOVR_SHIFT	31
#define	SRSC_SBYOVRVAL_MASK	0x60000000
#define	SRSC_SBYOVRVAL_SHIFT	29
#define	SRSC_SBYEN_MASK		0x01000000	
#define	SRSC_SBYEN_SHIFT	24


#define SRPC_PMU_STBYDIS_MASK	0x00000010	
#define SRPC_PMU_STBYDIS_SHIFT	4
#define SRPC_STBYOVRVAL_MASK	0x00000008
#define SRPC_STBYOVRVAL_SHIFT	3
#define SRPC_STBYOVR_MASK	0x00000007
#define SRPC_STBYOVR_SHIFT	0


#define SRECC_NUM_BANKS_MASK   0x000000F0
#define SRECC_NUM_BANKS_SHIFT  4
#define SRECC_BANKSIZE_MASK    0x0000000F
#define SRECC_BANKSIZE_SHIFT   0

#define SRECC_BANKSIZE(value)	 (1 << (value))


#define SRCBPC_PATCHENABLE 0x80000000

#define SRP_ADDRESS   0x0001FFFC
#define SRP_VALID     0x8000


#define SRCMD_WRITE  0x00020000
#define SRCMD_READ   0x00010000
#define SRCMD_DONE   0x80000000

#define SRCMD_DONE_DLY	1000


#endif	
