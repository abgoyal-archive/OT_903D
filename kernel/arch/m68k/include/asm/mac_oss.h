

#define OSS_BASE	(0x50f1a000)


#define OSS_NUBUS0	0
#define OSS_NUBUS1	1
#define OSS_NUBUS2	2
#define OSS_NUBUS3	3
#define OSS_NUBUS4	4
#define OSS_NUBUS5	5
#define OSS_IOPISM	6
#define OSS_IOPSCC	7
#define OSS_SOUND	8
#define OSS_SCSI	9
#define OSS_60HZ	10
#define OSS_VIA1	11
#define OSS_UNUSED1	12
#define OSS_UNUSED2	13
#define OSS_PARITY	14
#define OSS_UNUSED3	15

#define OSS_NUM_SOURCES	16


#define OSS_IP_NUBUS0	0x0001
#define OSS_IP_NUBUS1	0x0002
#define OSS_IP_NUBUS2	0x0004
#define OSS_IP_NUBUS3	0x0008
#define OSS_IP_NUBUS4	0x0010
#define OSS_IP_NUBUS5	0x0020
#define OSS_IP_IOPISM	0x0040
#define OSS_IP_IOPSCC	0x0080
#define OSS_IP_SOUND	0x0100
#define OSS_IP_SCSI	0x0200
#define OSS_IP_60HZ	0x0400
#define OSS_IP_VIA1	0x0800
#define OSS_IP_UNUSED1	0x1000
#define OSS_IP_UNUSED2	0x2000
#define OSS_IP_PARITY	0x4000
#define OSS_IP_UNUSED3	0x8000

#define OSS_IP_NUBUS (OSS_IP_NUBUS0|OSS_IP_NUBUS1|OSS_IP_NUBUS2|OSS_IP_NUBUS3|OSS_IP_NUBUS4|OSS_IP_NUBUS5)


#define OSS_POWEROFF	0x80


#define OSS_IRQLEV_DISABLED	0
#define OSS_IRQLEV_IOPISM	1	/* ADB? */
#define OSS_IRQLEV_SCSI		IRQ_AUTO_2
#define OSS_IRQLEV_NUBUS	IRQ_AUTO_3	/* keep this on its own level */
#define OSS_IRQLEV_IOPSCC	IRQ_AUTO_4	/* matches VIA alternate mapping */
#define OSS_IRQLEV_SOUND	IRQ_AUTO_5	/* matches VIA alternate mapping */
#define OSS_IRQLEV_60HZ		6	/* matches VIA alternate mapping */
#define OSS_IRQLEV_VIA1		IRQ_AUTO_6	/* matches VIA alternate mapping */
#define OSS_IRQLEV_PARITY	7	/* matches VIA alternate mapping */

#ifndef __ASSEMBLY__

struct mac_oss {
    __u8  irq_level[0x10];	/* [0x000-0x00f] Interrupt levels */
    __u8  padding0[0x1F2];	/* [0x010-0x201] IO space filler */
    __u16 irq_pending;		/* [0x202-0x203] pending interrupts bits */
    __u8  rom_ctrl;		/* [0x204-0x204] ROM cntl reg (for poweroff) */
    __u8  padding1[0x2];	/* [0x205-0x206] currently unused by A/UX */
    __u8  ack_60hz;		/* [0x207-0x207] 60 Hz ack. */
};

extern volatile struct mac_oss *oss;
extern int oss_present;

#endif /* __ASSEMBLY__ */
