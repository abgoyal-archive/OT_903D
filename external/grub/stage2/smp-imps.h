
#ifndef _SMP_IMPS_H
#define _SMP_IMPS_H

/* make sure "apic.h" is included */
#ifndef _APIC_H
#error		Must include "apic.h" before "smp-imps.h"
#endif /* !_APIC_H */


#ifdef IMPS_DEBUG
#define IMPS_DEBUG_PRINT(x)  KERNEL_PRINT(x)
#else /* !IMPS_DEBUG */
#define IMPS_DEBUG_PRINT(x)
#endif /* !IMPS_DEBUG */

#define IMPS_MAX_CPUS			APIC_BROADCAST_ID


#define IMPS_APIC_ID(x)							\
  ( imps_any_new_apics ? APIC_NEW_ID(x) : APIC_OLD_ID(x) )

#define IMPS_FPS_SIGNATURE	('_' | ('M'<<8) | ('P'<<16) | ('_'<<24))
#define IMPS_FPS_IMCRP_BIT	0x80
#define IMPS_FPS_DEFAULT_MAX	7

#define IMPS_CTH_SIGNATURE	('P' | ('C'<<8) | ('M'<<16) | ('P'<<24))

#define		IMPS_FLAG_ENABLED	1
#define IMPS_BCT_PROCESSOR		0
#define		IMPS_CPUFLAG_BOOT	2
#define IMPS_BCT_BUS			1
#define IMPS_BCT_IOAPIC			2
#define IMPS_BCT_IO_INTERRUPT		3
#define IMPS_BCT_LOCAL_INTERRUPT	4
#define		IMPS_INT_INT		0
#define		IMPS_INT_NMI		1
#define		IMPS_INT_SMI		2
#define		IMPS_INT_EXTINT		3



typedef struct imps_fps imps_fps;	/* MP floating pointer structure */
typedef struct imps_cth imps_cth;	/* MP configuration table header */
typedef struct imps_processor imps_processor;
typedef struct imps_bus imps_bus;
typedef struct imps_ioapic imps_ioapic;
typedef struct imps_interrupt imps_interrupt;



struct imps_fps
  {
    unsigned sig;
    imps_cth *cth_ptr;
    unsigned char length;
    unsigned char spec_rev;
    unsigned char checksum;
    unsigned char feature_info[5];
  };

struct imps_cth
  {
    unsigned sig;
    unsigned short base_length;
    unsigned char spec_rev;
    unsigned char checksum;
    char oem_id[8];
    char prod_id[12];
    unsigned oem_table_ptr;
    unsigned short oem_table_size;
    unsigned short entry_count;
    unsigned lapic_addr;
    unsigned short extended_length;
    unsigned char extended_checksum;
    char reserved[1];
  };


struct imps_processor
  {
    unsigned char type;		/* must be 0 */
    unsigned char apic_id;
    unsigned char apic_ver;
    unsigned char flags;
    unsigned signature;
    unsigned features;
    char reserved[8];
  };

struct imps_bus
  {
    unsigned char type;		/* must be 1 */
    unsigned char id;
    char bus_type[6];
  };

struct imps_ioapic
  {
    unsigned char type;		/* must be 2 */
    unsigned char id;
    unsigned char ver;
    unsigned char flags;
    unsigned addr;
  };

struct imps_interrupt
  {
    unsigned char type;		/* must be 3 or 4 */
    unsigned char int_type;
    unsigned short flags;
    unsigned char source_bus_id;
    unsigned char source_bus_irq;
    unsigned char dest_apic_id;
    unsigned char dest_apic_intin;
  };




int imps_probe (void);



#define IMPS_LAPIC_READ(x)  (*((volatile unsigned *) (imps_lapic_addr+(x))))
#define IMPS_LAPIC_WRITE(x, y)   \
   (*((volatile unsigned *) (imps_lapic_addr+(x))) = (y))

#endif /* !_SMP_IMPS_H */
