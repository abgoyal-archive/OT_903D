
#include <linux/module.h>
#include <linux/reboot.h>
#include <linux/spinlock.h>
#include <linux/pci_ids.h>
#include <linux/interrupt.h>
#include <linux/moduleparam.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <linux/timer.h>
#include <linux/pci.h>
#include <linux/aer.h>
#include <linux/slab.h>
#include <asm/dma.h>
#include <asm/io.h>
#include <asm/system.h>
#include <asm/uaccess.h>
#include <scsi/scsi_host.h>
#include <scsi/scsi.h>
#include <scsi/scsi_cmnd.h>
#include <scsi/scsi_tcq.h>
#include <scsi/scsi_device.h>
#include <scsi/scsi_transport.h>
#include <scsi/scsicam.h>
#include "arcmsr.h"

#ifdef CONFIG_SCSI_ARCMSR_RESET
	static int sleeptime = 20;
	static int retrycount = 12;
	module_param(sleeptime, int, S_IRUGO|S_IWUSR);
	MODULE_PARM_DESC(sleeptime, "The waiting period for FW ready while bus reset");
	module_param(retrycount, int, S_IRUGO|S_IWUSR);
	MODULE_PARM_DESC(retrycount, "The retry count for FW ready while bus reset");
#endif
MODULE_AUTHOR("Erich Chen <support@areca.com.tw>");
MODULE_DESCRIPTION("ARECA (ARC11xx/12xx/13xx/16xx) SATA/SAS RAID Host Bus Adapter");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_VERSION(ARCMSR_DRIVER_VERSION);

static int arcmsr_iop_message_xfer(struct AdapterControlBlock *acb,
					struct scsi_cmnd *cmd);
static int arcmsr_iop_confirm(struct AdapterControlBlock *acb);
static int arcmsr_abort(struct scsi_cmnd *);
static int arcmsr_bus_reset(struct scsi_cmnd *);
static int arcmsr_bios_param(struct scsi_device *sdev,
		struct block_device *bdev, sector_t capacity, int *info);
static int arcmsr_queue_command(struct scsi_cmnd *cmd,
					void (*done) (struct scsi_cmnd *));
static int arcmsr_probe(struct pci_dev *pdev,
				const struct pci_device_id *id);
static void arcmsr_remove(struct pci_dev *pdev);
static void arcmsr_shutdown(struct pci_dev *pdev);
static void arcmsr_iop_init(struct AdapterControlBlock *acb);
static void arcmsr_free_ccb_pool(struct AdapterControlBlock *acb);
static u32 arcmsr_disable_outbound_ints(struct AdapterControlBlock *acb);
static void arcmsr_stop_adapter_bgrb(struct AdapterControlBlock *acb);
static void arcmsr_flush_hba_cache(struct AdapterControlBlock *acb);
static void arcmsr_flush_hbb_cache(struct AdapterControlBlock *acb);
static void arcmsr_request_device_map(unsigned long pacb);
static void arcmsr_request_hba_device_map(struct AdapterControlBlock *acb);
static void arcmsr_request_hbb_device_map(struct AdapterControlBlock *acb);
static void arcmsr_message_isr_bh_fn(struct work_struct *work);
static void *arcmsr_get_firmware_spec(struct AdapterControlBlock *acb, int mode);
static void arcmsr_start_adapter_bgrb(struct AdapterControlBlock *acb);

static const char *arcmsr_info(struct Scsi_Host *);
static irqreturn_t arcmsr_interrupt(struct AdapterControlBlock *acb);
static int arcmsr_adjust_disk_queue_depth(struct scsi_device *sdev,
					  int queue_depth, int reason)
{
	if (reason != SCSI_QDEPTH_DEFAULT)
		return -EOPNOTSUPP;

	if (queue_depth > ARCMSR_MAX_CMD_PERLUN)
		queue_depth = ARCMSR_MAX_CMD_PERLUN;
	scsi_adjust_queue_depth(sdev, MSG_ORDERED_TAG, queue_depth);
	return queue_depth;
}

static struct scsi_host_template arcmsr_scsi_host_template = {
	.module			= THIS_MODULE,
	.name			= "ARCMSR ARECA SATA/SAS RAID Host Bus Adapter"
							ARCMSR_DRIVER_VERSION,
	.info			= arcmsr_info,
	.queuecommand		= arcmsr_queue_command,
	.eh_abort_handler	= arcmsr_abort,
	.eh_bus_reset_handler	= arcmsr_bus_reset,
	.bios_param		= arcmsr_bios_param,
	.change_queue_depth	= arcmsr_adjust_disk_queue_depth,
	.can_queue		= ARCMSR_MAX_OUTSTANDING_CMD,
	.this_id		= ARCMSR_SCSI_INITIATOR_ID,
	.sg_tablesize		= ARCMSR_MAX_SG_ENTRIES,
	.max_sectors    	= ARCMSR_MAX_XFER_SECTORS,
	.cmd_per_lun		= ARCMSR_MAX_CMD_PERLUN,
	.use_clustering		= ENABLE_CLUSTERING,
	.shost_attrs		= arcmsr_host_attrs,
};
static struct pci_device_id arcmsr_device_id_table[] = {
	{PCI_DEVICE(PCI_VENDOR_ID_ARECA, PCI_DEVICE_ID_ARECA_1110)},
	{PCI_DEVICE(PCI_VENDOR_ID_ARECA, PCI_DEVICE_ID_ARECA_1120)},
	{PCI_DEVICE(PCI_VENDOR_ID_ARECA, PCI_DEVICE_ID_ARECA_1130)},
	{PCI_DEVICE(PCI_VENDOR_ID_ARECA, PCI_DEVICE_ID_ARECA_1160)},
	{PCI_DEVICE(PCI_VENDOR_ID_ARECA, PCI_DEVICE_ID_ARECA_1170)},
	{PCI_DEVICE(PCI_VENDOR_ID_ARECA, PCI_DEVICE_ID_ARECA_1200)},
	{PCI_DEVICE(PCI_VENDOR_ID_ARECA, PCI_DEVICE_ID_ARECA_1201)},
	{PCI_DEVICE(PCI_VENDOR_ID_ARECA, PCI_DEVICE_ID_ARECA_1202)},
	{PCI_DEVICE(PCI_VENDOR_ID_ARECA, PCI_DEVICE_ID_ARECA_1210)},
	{PCI_DEVICE(PCI_VENDOR_ID_ARECA, PCI_DEVICE_ID_ARECA_1220)},
	{PCI_DEVICE(PCI_VENDOR_ID_ARECA, PCI_DEVICE_ID_ARECA_1230)},
	{PCI_DEVICE(PCI_VENDOR_ID_ARECA, PCI_DEVICE_ID_ARECA_1260)},
	{PCI_DEVICE(PCI_VENDOR_ID_ARECA, PCI_DEVICE_ID_ARECA_1270)},
	{PCI_DEVICE(PCI_VENDOR_ID_ARECA, PCI_DEVICE_ID_ARECA_1280)},
	{PCI_DEVICE(PCI_VENDOR_ID_ARECA, PCI_DEVICE_ID_ARECA_1380)},
	{PCI_DEVICE(PCI_VENDOR_ID_ARECA, PCI_DEVICE_ID_ARECA_1381)},
	{PCI_DEVICE(PCI_VENDOR_ID_ARECA, PCI_DEVICE_ID_ARECA_1680)},
	{PCI_DEVICE(PCI_VENDOR_ID_ARECA, PCI_DEVICE_ID_ARECA_1681)},
	{0, 0}, /* Terminating entry */
};
MODULE_DEVICE_TABLE(pci, arcmsr_device_id_table);
static struct pci_driver arcmsr_pci_driver = {
	.name			= "arcmsr",
	.id_table		= arcmsr_device_id_table,
	.probe			= arcmsr_probe,
	.remove			= arcmsr_remove,
	.shutdown		= arcmsr_shutdown,
};

static irqreturn_t arcmsr_do_interrupt(int irq, void *dev_id)
{
	irqreturn_t handle_state;
	struct AdapterControlBlock *acb = dev_id;

	spin_lock(acb->host->host_lock);
	handle_state = arcmsr_interrupt(acb);
	spin_unlock(acb->host->host_lock);

	return handle_state;
}

static int arcmsr_bios_param(struct scsi_device *sdev,
		struct block_device *bdev, sector_t capacity, int *geom)
{
	int ret, heads, sectors, cylinders, total_capacity;
	unsigned char *buffer;/* return copy of block device's partition table */

	buffer = scsi_bios_ptable(bdev);
	if (buffer) {
		ret = scsi_partsize(buffer, capacity, &geom[2], &geom[0], &geom[1]);
		kfree(buffer);
		if (ret != -1)
			return ret;
	}
	total_capacity = capacity;
	heads = 64;
	sectors = 32;
	cylinders = total_capacity / (heads * sectors);
	if (cylinders > 1024) {
		heads = 255;
		sectors = 63;
		cylinders = total_capacity / (heads * sectors);
	}
	geom[0] = heads;
	geom[1] = sectors;
	geom[2] = cylinders;
	return 0;
}

static void arcmsr_define_adapter_type(struct AdapterControlBlock *acb)
{
	struct pci_dev *pdev = acb->pdev;
	u16 dev_id;
	pci_read_config_word(pdev, PCI_DEVICE_ID, &dev_id);
	switch (dev_id) {
	case 0x1201 : {
		acb->adapter_type = ACB_ADAPTER_TYPE_B;
		}
		break;

	default : acb->adapter_type = ACB_ADAPTER_TYPE_A;
	}
}

static int arcmsr_alloc_ccb_pool(struct AdapterControlBlock *acb)
{

	switch (acb->adapter_type) {

	case ACB_ADAPTER_TYPE_A: {
		struct pci_dev *pdev = acb->pdev;
		void *dma_coherent;
		dma_addr_t dma_coherent_handle, dma_addr;
		struct CommandControlBlock *ccb_tmp;
		int i, j;

		acb->pmuA = ioremap(pci_resource_start(pdev, 0), pci_resource_len(pdev, 0));
		if (!acb->pmuA) {
			printk(KERN_NOTICE "arcmsr%d: memory mapping region fail \n",
							acb->host->host_no);
			return -ENOMEM;
		}

		dma_coherent = dma_alloc_coherent(&pdev->dev,
			ARCMSR_MAX_FREECCB_NUM *
			sizeof (struct CommandControlBlock) + 0x20,
			&dma_coherent_handle, GFP_KERNEL);

		if (!dma_coherent) {
			iounmap(acb->pmuA);
			return -ENOMEM;
		}

		acb->dma_coherent = dma_coherent;
		acb->dma_coherent_handle = dma_coherent_handle;

		if (((unsigned long)dma_coherent & 0x1F)) {
			dma_coherent = dma_coherent +
				(0x20 - ((unsigned long)dma_coherent & 0x1F));
			dma_coherent_handle = dma_coherent_handle +
				(0x20 - ((unsigned long)dma_coherent_handle & 0x1F));
		}

		dma_addr = dma_coherent_handle;
		ccb_tmp = (struct CommandControlBlock *)dma_coherent;
		for (i = 0; i < ARCMSR_MAX_FREECCB_NUM; i++) {
			ccb_tmp->cdb_shifted_phyaddr = dma_addr >> 5;
			ccb_tmp->acb = acb;
			acb->pccb_pool[i] = ccb_tmp;
			list_add_tail(&ccb_tmp->list, &acb->ccb_free_list);
			dma_addr = dma_addr + sizeof(struct CommandControlBlock);
			ccb_tmp++;
		}

		acb->vir2phy_offset = (unsigned long)ccb_tmp -(unsigned long)dma_addr;
		for (i = 0; i < ARCMSR_MAX_TARGETID; i++)
			for (j = 0; j < ARCMSR_MAX_TARGETLUN; j++)
				acb->devstate[i][j] = ARECA_RAID_GONE;
		}
		break;

	case ACB_ADAPTER_TYPE_B: {

		struct pci_dev *pdev = acb->pdev;
		struct MessageUnit_B *reg;
		void __iomem *mem_base0, *mem_base1;
		void *dma_coherent;
		dma_addr_t dma_coherent_handle, dma_addr;
		struct CommandControlBlock *ccb_tmp;
		int i, j;

		dma_coherent = dma_alloc_coherent(&pdev->dev,
			((ARCMSR_MAX_FREECCB_NUM *
			sizeof(struct CommandControlBlock) + 0x20) +
			sizeof(struct MessageUnit_B)),
			&dma_coherent_handle, GFP_KERNEL);
		if (!dma_coherent)
			return -ENOMEM;

		acb->dma_coherent = dma_coherent;
		acb->dma_coherent_handle = dma_coherent_handle;

		if (((unsigned long)dma_coherent & 0x1F)) {
			dma_coherent = dma_coherent +
				(0x20 - ((unsigned long)dma_coherent & 0x1F));
			dma_coherent_handle = dma_coherent_handle +
				(0x20 - ((unsigned long)dma_coherent_handle & 0x1F));
		}

		dma_addr = dma_coherent_handle;
		ccb_tmp = (struct CommandControlBlock *)dma_coherent;
		for (i = 0; i < ARCMSR_MAX_FREECCB_NUM; i++) {
			ccb_tmp->cdb_shifted_phyaddr = dma_addr >> 5;
			ccb_tmp->acb = acb;
			acb->pccb_pool[i] = ccb_tmp;
			list_add_tail(&ccb_tmp->list, &acb->ccb_free_list);
			dma_addr = dma_addr + sizeof(struct CommandControlBlock);
			ccb_tmp++;
		}

		reg = (struct MessageUnit_B *)(dma_coherent +
		ARCMSR_MAX_FREECCB_NUM * sizeof(struct CommandControlBlock));
		acb->pmuB = reg;
		mem_base0 = ioremap(pci_resource_start(pdev, 0),
					pci_resource_len(pdev, 0));
		if (!mem_base0)
			goto out;

		mem_base1 = ioremap(pci_resource_start(pdev, 2),
					pci_resource_len(pdev, 2));
		if (!mem_base1) {
			iounmap(mem_base0);
			goto out;
		}

		reg->drv2iop_doorbell_reg = mem_base0 + ARCMSR_DRV2IOP_DOORBELL;
		reg->drv2iop_doorbell_mask_reg = mem_base0 +
						ARCMSR_DRV2IOP_DOORBELL_MASK;
		reg->iop2drv_doorbell_reg = mem_base0 + ARCMSR_IOP2DRV_DOORBELL;
		reg->iop2drv_doorbell_mask_reg = mem_base0 +
						ARCMSR_IOP2DRV_DOORBELL_MASK;
		reg->ioctl_wbuffer_reg = mem_base1 + ARCMSR_IOCTL_WBUFFER;
		reg->ioctl_rbuffer_reg = mem_base1 + ARCMSR_IOCTL_RBUFFER;
		reg->msgcode_rwbuffer_reg = mem_base1 + ARCMSR_MSGCODE_RWBUFFER;

		acb->vir2phy_offset = (unsigned long)ccb_tmp -(unsigned long)dma_addr;
		for (i = 0; i < ARCMSR_MAX_TARGETID; i++)
			for (j = 0; j < ARCMSR_MAX_TARGETLUN; j++)
				acb->devstate[i][j] = ARECA_RAID_GOOD;
		}
		break;
	}
	return 0;

out:
	dma_free_coherent(&acb->pdev->dev,
		(ARCMSR_MAX_FREECCB_NUM * sizeof(struct CommandControlBlock) + 0x20 +
		sizeof(struct MessageUnit_B)), acb->dma_coherent, acb->dma_coherent_handle);
	return -ENOMEM;
}
static void arcmsr_message_isr_bh_fn(struct work_struct *work)
{
	struct AdapterControlBlock *acb = container_of(work, struct AdapterControlBlock, arcmsr_do_message_isr_bh);

	switch (acb->adapter_type) {
		case ACB_ADAPTER_TYPE_A: {

			struct MessageUnit_A __iomem *reg  = acb->pmuA;
			char *acb_dev_map = (char *)acb->device_map;
			uint32_t __iomem *signature = (uint32_t __iomem *) (&reg->message_rwbuffer[0]);
			char __iomem *devicemap = (char __iomem *) (&reg->message_rwbuffer[21]);
			int target, lun;
			struct scsi_device *psdev;
			char diff;

			atomic_inc(&acb->rq_map_token);
			if (readl(signature) == ARCMSR_SIGNATURE_GET_CONFIG) {
				for (target = 0; target < ARCMSR_MAX_TARGETID - 1; target++) {
					diff = (*acb_dev_map)^readb(devicemap);
					if (diff != 0) {
						char temp;
						*acb_dev_map = readb(devicemap);
						temp = *acb_dev_map;
						for (lun = 0; lun < ARCMSR_MAX_TARGETLUN; lun++) {
							if ((temp & 0x01) == 1 && (diff & 0x01) == 1) {
								scsi_add_device(acb->host, 0, target, lun);
							} else if ((temp & 0x01) == 0 && (diff & 0x01) == 1) {
								psdev = scsi_device_lookup(acb->host, 0, target, lun);
								if (psdev != NULL) {
									scsi_remove_device(psdev);
									scsi_device_put(psdev);
								}
							}
							temp >>= 1;
							diff >>= 1;
						}
					}
					devicemap++;
					acb_dev_map++;
				}
			}
			break;
		}

		case ACB_ADAPTER_TYPE_B: {
			struct MessageUnit_B *reg  = acb->pmuB;
			char *acb_dev_map = (char *)acb->device_map;
			uint32_t __iomem *signature = (uint32_t __iomem *)(&reg->msgcode_rwbuffer_reg[0]);
			char __iomem *devicemap = (char __iomem *)(&reg->msgcode_rwbuffer_reg[21]);
			int target, lun;
			struct scsi_device *psdev;
			char diff;

			atomic_inc(&acb->rq_map_token);
			if (readl(signature) == ARCMSR_SIGNATURE_GET_CONFIG) {
				for (target = 0; target < ARCMSR_MAX_TARGETID - 1; target++) {
					diff = (*acb_dev_map)^readb(devicemap);
					if (diff != 0) {
						char temp;
						*acb_dev_map = readb(devicemap);
						temp = *acb_dev_map;
						for (lun = 0; lun < ARCMSR_MAX_TARGETLUN; lun++) {
							if ((temp & 0x01) == 1 && (diff & 0x01) == 1) {
								scsi_add_device(acb->host, 0, target, lun);
							} else if ((temp & 0x01) == 0 && (diff & 0x01) == 1) {
								psdev = scsi_device_lookup(acb->host, 0, target, lun);
								if (psdev != NULL) {
									scsi_remove_device(psdev);
									scsi_device_put(psdev);
								}
							}
							temp >>= 1;
							diff >>= 1;
						}
					}
					devicemap++;
					acb_dev_map++;
				}
			}
		}
	}
}

static int arcmsr_probe(struct pci_dev *pdev,
	const struct pci_device_id *id)
{
	struct Scsi_Host *host;
	struct AdapterControlBlock *acb;
	uint8_t bus, dev_fun;
	int error;

	error = pci_enable_device(pdev);
	if (error)
		goto out;
	pci_set_master(pdev);

	host = scsi_host_alloc(&arcmsr_scsi_host_template,
			sizeof(struct AdapterControlBlock));
	if (!host) {
		error = -ENOMEM;
		goto out_disable_device;
	}
	acb = (struct AdapterControlBlock *)host->hostdata;
	memset(acb, 0, sizeof (struct AdapterControlBlock));

	error = pci_set_dma_mask(pdev, DMA_BIT_MASK(64));
	if (error) {
		error = pci_set_dma_mask(pdev, DMA_BIT_MASK(32));
		if (error) {
			printk(KERN_WARNING
			       "scsi%d: No suitable DMA mask available\n",
			       host->host_no);
			goto out_host_put;
		}
	}
	bus = pdev->bus->number;
	dev_fun = pdev->devfn;
	acb->host = host;
	acb->pdev = pdev;
	host->max_sectors = ARCMSR_MAX_XFER_SECTORS;
	host->max_lun = ARCMSR_MAX_TARGETLUN;
	host->max_id = ARCMSR_MAX_TARGETID;/*16:8*/
	host->max_cmd_len = 16;    /*this is issue of 64bit LBA, over 2T byte*/
	host->sg_tablesize = ARCMSR_MAX_SG_ENTRIES;
	host->can_queue = ARCMSR_MAX_FREECCB_NUM; /* max simultaneous cmds */
	host->cmd_per_lun = ARCMSR_MAX_CMD_PERLUN;
	host->this_id = ARCMSR_SCSI_INITIATOR_ID;
	host->unique_id = (bus << 8) | dev_fun;
	host->irq = pdev->irq;
	error = pci_request_regions(pdev, "arcmsr");
	if (error) {
		goto out_host_put;
	}
	arcmsr_define_adapter_type(acb);

	acb->acb_flags |= (ACB_F_MESSAGE_WQBUFFER_CLEARED |
			   ACB_F_MESSAGE_RQBUFFER_CLEARED |
			   ACB_F_MESSAGE_WQBUFFER_READED);
	acb->acb_flags &= ~ACB_F_SCSISTOPADAPTER;
	INIT_LIST_HEAD(&acb->ccb_free_list);
	INIT_WORK(&acb->arcmsr_do_message_isr_bh, arcmsr_message_isr_bh_fn);
	error = arcmsr_alloc_ccb_pool(acb);
	if (error)
		goto out_release_regions;

	arcmsr_iop_init(acb);
	error = request_irq(pdev->irq, arcmsr_do_interrupt,
			    IRQF_SHARED, "arcmsr", acb);
	if (error)
		goto out_free_ccb_pool;

	pci_set_drvdata(pdev, host);
	if (strncmp(acb->firm_version, "V1.42", 5) >= 0)
		host->max_sectors= ARCMSR_MAX_XFER_SECTORS_B;

	error = scsi_add_host(host, &pdev->dev);
	if (error)
		goto out_free_irq;

	error = arcmsr_alloc_sysfs_attr(acb);
	if (error)
		goto out_free_sysfs;

	scsi_scan_host(host);
	#ifdef CONFIG_SCSI_ARCMSR_AER
	pci_enable_pcie_error_reporting(pdev);
	#endif
	atomic_set(&acb->rq_map_token, 16);
	acb->fw_state = true;
	init_timer(&acb->eternal_timer);
	acb->eternal_timer.expires = jiffies + msecs_to_jiffies(10*HZ);
	acb->eternal_timer.data = (unsigned long) acb;
	acb->eternal_timer.function = &arcmsr_request_device_map;
	add_timer(&acb->eternal_timer);

	return 0;
 out_free_sysfs:
 out_free_irq:
	free_irq(pdev->irq, acb);
 out_free_ccb_pool:
	arcmsr_free_ccb_pool(acb);
 out_release_regions:
	pci_release_regions(pdev);
 out_host_put:
	scsi_host_put(host);
 out_disable_device:
	pci_disable_device(pdev);
 out:
	return error;
}

static uint8_t arcmsr_hba_wait_msgint_ready(struct AdapterControlBlock *acb)
{
	struct MessageUnit_A __iomem *reg = acb->pmuA;
	uint32_t Index;
	uint8_t Retries = 0x00;

	do {
		for (Index = 0; Index < 100; Index++) {
			if (readl(&reg->outbound_intstatus) &
					ARCMSR_MU_OUTBOUND_MESSAGE0_INT) {
				writel(ARCMSR_MU_OUTBOUND_MESSAGE0_INT,
					&reg->outbound_intstatus);
				return 0x00;
			}
			msleep(10);
		}/*max 1 seconds*/

	} while (Retries++ < 20);/*max 20 sec*/
	return 0xff;
}

static uint8_t arcmsr_hbb_wait_msgint_ready(struct AdapterControlBlock *acb)
{
	struct MessageUnit_B *reg = acb->pmuB;
	uint32_t Index;
	uint8_t Retries = 0x00;

	do {
		for (Index = 0; Index < 100; Index++) {
			if (readl(reg->iop2drv_doorbell_reg)
				& ARCMSR_IOP2DRV_MESSAGE_CMD_DONE) {
				writel(ARCMSR_MESSAGE_INT_CLEAR_PATTERN
					, reg->iop2drv_doorbell_reg);
				writel(ARCMSR_DRV2IOP_END_OF_INTERRUPT, reg->drv2iop_doorbell_reg);
				return 0x00;
			}
			msleep(10);
		}/*max 1 seconds*/

	} while (Retries++ < 20);/*max 20 sec*/
	return 0xff;
}

static uint8_t arcmsr_abort_hba_allcmd(struct AdapterControlBlock *acb)
{
	struct MessageUnit_A __iomem *reg = acb->pmuA;

	writel(ARCMSR_INBOUND_MESG0_ABORT_CMD, &reg->inbound_msgaddr0);
	if (arcmsr_hba_wait_msgint_ready(acb)) {
		printk(KERN_NOTICE
			"arcmsr%d: wait 'abort all outstanding command' timeout \n"
			, acb->host->host_no);
		return 0xff;
	}
	return 0x00;
}

static uint8_t arcmsr_abort_hbb_allcmd(struct AdapterControlBlock *acb)
{
	struct MessageUnit_B *reg = acb->pmuB;

	writel(ARCMSR_MESSAGE_ABORT_CMD, reg->drv2iop_doorbell_reg);
	if (arcmsr_hbb_wait_msgint_ready(acb)) {
		printk(KERN_NOTICE
			"arcmsr%d: wait 'abort all outstanding command' timeout \n"
			, acb->host->host_no);
		return 0xff;
	}
	return 0x00;
}

static uint8_t arcmsr_abort_allcmd(struct AdapterControlBlock *acb)
{
	uint8_t rtnval = 0;
	switch (acb->adapter_type) {
	case ACB_ADAPTER_TYPE_A: {
		rtnval = arcmsr_abort_hba_allcmd(acb);
		}
		break;

	case ACB_ADAPTER_TYPE_B: {
		rtnval = arcmsr_abort_hbb_allcmd(acb);
		}
	}
	return rtnval;
}

static void arcmsr_pci_unmap_dma(struct CommandControlBlock *ccb)
{
	struct scsi_cmnd *pcmd = ccb->pcmd;

	scsi_dma_unmap(pcmd);
}

static void arcmsr_ccb_complete(struct CommandControlBlock *ccb, int stand_flag)
{
	struct AdapterControlBlock *acb = ccb->acb;
	struct scsi_cmnd *pcmd = ccb->pcmd;

	arcmsr_pci_unmap_dma(ccb);
	if (stand_flag == 1)
		atomic_dec(&acb->ccboutstandingcount);
	ccb->startdone = ARCMSR_CCB_DONE;
	ccb->ccb_flags = 0;
	list_add_tail(&ccb->list, &acb->ccb_free_list);
	pcmd->scsi_done(pcmd);
}

static void arcmsr_flush_hba_cache(struct AdapterControlBlock *acb)
{
	struct MessageUnit_A __iomem *reg = acb->pmuA;
	int retry_count = 30;

	writel(ARCMSR_INBOUND_MESG0_FLUSH_CACHE, &reg->inbound_msgaddr0);
	do {
		if (!arcmsr_hba_wait_msgint_ready(acb))
			break;
		else {
			retry_count--;
			printk(KERN_NOTICE "arcmsr%d: wait 'flush adapter cache' \
			timeout, retry count down = %d \n", acb->host->host_no, retry_count);
		}
	} while (retry_count != 0);
}

static void arcmsr_flush_hbb_cache(struct AdapterControlBlock *acb)
{
	struct MessageUnit_B *reg = acb->pmuB;
	int retry_count = 30;

	writel(ARCMSR_MESSAGE_FLUSH_CACHE, reg->drv2iop_doorbell_reg);
	do {
		if (!arcmsr_hbb_wait_msgint_ready(acb))
			break;
		else {
			retry_count--;
			printk(KERN_NOTICE "arcmsr%d: wait 'flush adapter cache' \
			timeout,retry count down = %d \n", acb->host->host_no, retry_count);
		}
	} while (retry_count != 0);
}

static void arcmsr_flush_adapter_cache(struct AdapterControlBlock *acb)
{
	switch (acb->adapter_type) {

	case ACB_ADAPTER_TYPE_A: {
		arcmsr_flush_hba_cache(acb);
		}
		break;

	case ACB_ADAPTER_TYPE_B: {
		arcmsr_flush_hbb_cache(acb);
		}
	}
}

static void arcmsr_report_sense_info(struct CommandControlBlock *ccb)
{

	struct scsi_cmnd *pcmd = ccb->pcmd;
	struct SENSE_DATA *sensebuffer = (struct SENSE_DATA *)pcmd->sense_buffer;

	pcmd->result = DID_OK << 16;
	if (sensebuffer) {
		int sense_data_length =
			sizeof(struct SENSE_DATA) < SCSI_SENSE_BUFFERSIZE
			? sizeof(struct SENSE_DATA) : SCSI_SENSE_BUFFERSIZE;
		memset(sensebuffer, 0, SCSI_SENSE_BUFFERSIZE);
		memcpy(sensebuffer, ccb->arcmsr_cdb.SenseData, sense_data_length);
		sensebuffer->ErrorCode = SCSI_SENSE_CURRENT_ERRORS;
		sensebuffer->Valid = 1;
	}
}

static u32 arcmsr_disable_outbound_ints(struct AdapterControlBlock *acb)
{
	u32 orig_mask = 0;
	switch (acb->adapter_type) {

	case ACB_ADAPTER_TYPE_A : {
		struct MessageUnit_A __iomem *reg = acb->pmuA;
		orig_mask = readl(&reg->outbound_intmask);
		writel(orig_mask|ARCMSR_MU_OUTBOUND_ALL_INTMASKENABLE, \
						&reg->outbound_intmask);
		}
		break;

	case ACB_ADAPTER_TYPE_B : {
		struct MessageUnit_B *reg = acb->pmuB;
		orig_mask = readl(reg->iop2drv_doorbell_mask_reg);
		writel(0, reg->iop2drv_doorbell_mask_reg);
		}
		break;
	}
	return orig_mask;
}

static void arcmsr_report_ccb_state(struct AdapterControlBlock *acb, \
			struct CommandControlBlock *ccb, uint32_t flag_ccb)
{

	uint8_t id, lun;
	id = ccb->pcmd->device->id;
	lun = ccb->pcmd->device->lun;
	if (!(flag_ccb & ARCMSR_CCBREPLY_FLAG_ERROR)) {
		if (acb->devstate[id][lun] == ARECA_RAID_GONE)
			acb->devstate[id][lun] = ARECA_RAID_GOOD;
			ccb->pcmd->result = DID_OK << 16;
			arcmsr_ccb_complete(ccb, 1);
	} else {
		switch (ccb->arcmsr_cdb.DeviceStatus) {
		case ARCMSR_DEV_SELECT_TIMEOUT: {
			acb->devstate[id][lun] = ARECA_RAID_GONE;
			ccb->pcmd->result = DID_NO_CONNECT << 16;
			arcmsr_ccb_complete(ccb, 1);
			}
			break;

		case ARCMSR_DEV_ABORTED:

		case ARCMSR_DEV_INIT_FAIL: {
			acb->devstate[id][lun] = ARECA_RAID_GONE;
			ccb->pcmd->result = DID_BAD_TARGET << 16;
			arcmsr_ccb_complete(ccb, 1);
			}
			break;

		case ARCMSR_DEV_CHECK_CONDITION: {
			acb->devstate[id][lun] = ARECA_RAID_GOOD;
			arcmsr_report_sense_info(ccb);
			arcmsr_ccb_complete(ccb, 1);
			}
			break;

		default:
				printk(KERN_NOTICE
					"arcmsr%d: scsi id = %d lun = %d"
					" isr get command error done, "
					"but got unknown DeviceStatus = 0x%x \n"
					, acb->host->host_no
					, id
					, lun
					, ccb->arcmsr_cdb.DeviceStatus);
					acb->devstate[id][lun] = ARECA_RAID_GONE;
					ccb->pcmd->result = DID_NO_CONNECT << 16;
					arcmsr_ccb_complete(ccb, 1);
			break;
		}
	}
}

static void arcmsr_drain_donequeue(struct AdapterControlBlock *acb, uint32_t flag_ccb)

{
	struct CommandControlBlock *ccb;

	ccb = (struct CommandControlBlock *)(acb->vir2phy_offset + (flag_ccb << 5));
	if ((ccb->acb != acb) || (ccb->startdone != ARCMSR_CCB_START)) {
		if (ccb->startdone == ARCMSR_CCB_ABORTED) {
			struct scsi_cmnd *abortcmd = ccb->pcmd;
			if (abortcmd) {
				abortcmd->result |= DID_ABORT << 16;
				arcmsr_ccb_complete(ccb, 1);
				printk(KERN_NOTICE "arcmsr%d: ccb ='0x%p' \
				isr got aborted command \n", acb->host->host_no, ccb);
			}
		}
		printk(KERN_NOTICE "arcmsr%d: isr get an illegal ccb command \
				done acb = '0x%p'"
				"ccb = '0x%p' ccbacb = '0x%p' startdone = 0x%x"
				" ccboutstandingcount = %d \n"
				, acb->host->host_no
				, acb
				, ccb
				, ccb->acb
				, ccb->startdone
				, atomic_read(&acb->ccboutstandingcount));
		}
	else
	arcmsr_report_ccb_state(acb, ccb, flag_ccb);
}

static void arcmsr_done4abort_postqueue(struct AdapterControlBlock *acb)
{
	int i = 0;
	uint32_t flag_ccb;

	switch (acb->adapter_type) {

	case ACB_ADAPTER_TYPE_A: {
		struct MessageUnit_A __iomem *reg = acb->pmuA;
		uint32_t outbound_intstatus;
		outbound_intstatus = readl(&reg->outbound_intstatus) &
					acb->outbound_int_enable;
		/*clear and abort all outbound posted Q*/
		writel(outbound_intstatus, &reg->outbound_intstatus);/*clear interrupt*/
		while (((flag_ccb = readl(&reg->outbound_queueport)) != 0xFFFFFFFF)
				&& (i++ < ARCMSR_MAX_OUTSTANDING_CMD)) {
			arcmsr_drain_donequeue(acb, flag_ccb);
		}
		}
		break;

	case ACB_ADAPTER_TYPE_B: {
		struct MessageUnit_B *reg = acb->pmuB;
		/*clear all outbound posted Q*/
		for (i = 0; i < ARCMSR_MAX_HBB_POSTQUEUE; i++) {
			if ((flag_ccb = readl(&reg->done_qbuffer[i])) != 0) {
				writel(0, &reg->done_qbuffer[i]);
				arcmsr_drain_donequeue(acb, flag_ccb);
			}
			writel(0, &reg->post_qbuffer[i]);
		}
		reg->doneq_index = 0;
		reg->postq_index = 0;
		}
		break;
	}
}
static void arcmsr_remove(struct pci_dev *pdev)
{
	struct Scsi_Host *host = pci_get_drvdata(pdev);
	struct AdapterControlBlock *acb =
		(struct AdapterControlBlock *) host->hostdata;
	int poll_count = 0;
	arcmsr_free_sysfs_attr(acb);
	scsi_remove_host(host);
	flush_scheduled_work();
	del_timer_sync(&acb->eternal_timer);
	arcmsr_disable_outbound_ints(acb);
	arcmsr_stop_adapter_bgrb(acb);
	arcmsr_flush_adapter_cache(acb);
	acb->acb_flags |= ACB_F_SCSISTOPADAPTER;
	acb->acb_flags &= ~ACB_F_IOP_INITED;

	for (poll_count = 0; poll_count < ARCMSR_MAX_OUTSTANDING_CMD; poll_count++) {
		if (!atomic_read(&acb->ccboutstandingcount))
			break;
		arcmsr_interrupt(acb);/* FIXME: need spinlock */
		msleep(25);
	}

	if (atomic_read(&acb->ccboutstandingcount)) {
		int i;

		arcmsr_abort_allcmd(acb);
		arcmsr_done4abort_postqueue(acb);
		for (i = 0; i < ARCMSR_MAX_FREECCB_NUM; i++) {
			struct CommandControlBlock *ccb = acb->pccb_pool[i];
			if (ccb->startdone == ARCMSR_CCB_START) {
				ccb->startdone = ARCMSR_CCB_ABORTED;
				ccb->pcmd->result = DID_ABORT << 16;
				arcmsr_ccb_complete(ccb, 1);
			}
		}
	}

	free_irq(pdev->irq, acb);
	arcmsr_free_ccb_pool(acb);
	pci_release_regions(pdev);

	scsi_host_put(host);

	pci_disable_device(pdev);
	pci_set_drvdata(pdev, NULL);
}

static void arcmsr_shutdown(struct pci_dev *pdev)
{
	struct Scsi_Host *host = pci_get_drvdata(pdev);
	struct AdapterControlBlock *acb =
		(struct AdapterControlBlock *)host->hostdata;
	del_timer_sync(&acb->eternal_timer);
	arcmsr_disable_outbound_ints(acb);
	flush_scheduled_work();
	arcmsr_stop_adapter_bgrb(acb);
	arcmsr_flush_adapter_cache(acb);
}

static int arcmsr_module_init(void)
{
	int error = 0;

	error = pci_register_driver(&arcmsr_pci_driver);
	return error;
}

static void arcmsr_module_exit(void)
{
	pci_unregister_driver(&arcmsr_pci_driver);
}
module_init(arcmsr_module_init);
module_exit(arcmsr_module_exit);

static void arcmsr_enable_outbound_ints(struct AdapterControlBlock *acb,
						u32 intmask_org)
{
	u32 mask;

	switch (acb->adapter_type) {

	case ACB_ADAPTER_TYPE_A : {
		struct MessageUnit_A __iomem *reg = acb->pmuA;
		mask = intmask_org & ~(ARCMSR_MU_OUTBOUND_POSTQUEUE_INTMASKENABLE |
			     ARCMSR_MU_OUTBOUND_DOORBELL_INTMASKENABLE|
			     ARCMSR_MU_OUTBOUND_MESSAGE0_INTMASKENABLE);
		writel(mask, &reg->outbound_intmask);
		acb->outbound_int_enable = ~(intmask_org & mask) & 0x000000ff;
		}
		break;

	case ACB_ADAPTER_TYPE_B : {
		struct MessageUnit_B *reg = acb->pmuB;
		mask = intmask_org | (ARCMSR_IOP2DRV_DATA_WRITE_OK |
			ARCMSR_IOP2DRV_DATA_READ_OK |
			ARCMSR_IOP2DRV_CDB_DONE |
			ARCMSR_IOP2DRV_MESSAGE_CMD_DONE);
		writel(mask, reg->iop2drv_doorbell_mask_reg);
		acb->outbound_int_enable = (intmask_org | mask) & 0x0000000f;
		}
	}
}

static int arcmsr_build_ccb(struct AdapterControlBlock *acb,
	struct CommandControlBlock *ccb, struct scsi_cmnd *pcmd)
{
	struct ARCMSR_CDB *arcmsr_cdb = (struct ARCMSR_CDB *)&ccb->arcmsr_cdb;
	int8_t *psge = (int8_t *)&arcmsr_cdb->u;
	__le32 address_lo, address_hi;
	int arccdbsize = 0x30;
	int nseg;

	ccb->pcmd = pcmd;
	memset(arcmsr_cdb, 0, sizeof(struct ARCMSR_CDB));
	arcmsr_cdb->Bus = 0;
	arcmsr_cdb->TargetID = pcmd->device->id;
	arcmsr_cdb->LUN = pcmd->device->lun;
	arcmsr_cdb->Function = 1;
	arcmsr_cdb->CdbLength = (uint8_t)pcmd->cmd_len;
	arcmsr_cdb->Context = (unsigned long)arcmsr_cdb;
	memcpy(arcmsr_cdb->Cdb, pcmd->cmnd, pcmd->cmd_len);

	nseg = scsi_dma_map(pcmd);
	if (nseg > ARCMSR_MAX_SG_ENTRIES)
		return FAILED;
	BUG_ON(nseg < 0);

	if (nseg) {
		__le32 length;
		int i, cdb_sgcount = 0;
		struct scatterlist *sg;

		/* map stor port SG list to our iop SG List. */
		scsi_for_each_sg(pcmd, sg, nseg, i) {
			/* Get the physical address of the current data pointer */
			length = cpu_to_le32(sg_dma_len(sg));
			address_lo = cpu_to_le32(dma_addr_lo32(sg_dma_address(sg)));
			address_hi = cpu_to_le32(dma_addr_hi32(sg_dma_address(sg)));
			if (address_hi == 0) {
				struct SG32ENTRY *pdma_sg = (struct SG32ENTRY *)psge;

				pdma_sg->address = address_lo;
				pdma_sg->length = length;
				psge += sizeof (struct SG32ENTRY);
				arccdbsize += sizeof (struct SG32ENTRY);
			} else {
				struct SG64ENTRY *pdma_sg = (struct SG64ENTRY *)psge;

				pdma_sg->addresshigh = address_hi;
				pdma_sg->address = address_lo;
				pdma_sg->length = length|cpu_to_le32(IS_SG64_ADDR);
				psge += sizeof (struct SG64ENTRY);
				arccdbsize += sizeof (struct SG64ENTRY);
			}
			cdb_sgcount++;
		}
		arcmsr_cdb->sgcount = (uint8_t)cdb_sgcount;
		arcmsr_cdb->DataLength = scsi_bufflen(pcmd);
		if ( arccdbsize > 256)
			arcmsr_cdb->Flags |= ARCMSR_CDB_FLAG_SGL_BSIZE;
	}
	if (pcmd->sc_data_direction == DMA_TO_DEVICE ) {
		arcmsr_cdb->Flags |= ARCMSR_CDB_FLAG_WRITE;
		ccb->ccb_flags |= CCB_FLAG_WRITE;
	}
	return SUCCESS;
}

static void arcmsr_post_ccb(struct AdapterControlBlock *acb, struct CommandControlBlock *ccb)
{
	uint32_t cdb_shifted_phyaddr = ccb->cdb_shifted_phyaddr;
	struct ARCMSR_CDB *arcmsr_cdb = (struct ARCMSR_CDB *)&ccb->arcmsr_cdb;
	atomic_inc(&acb->ccboutstandingcount);
	ccb->startdone = ARCMSR_CCB_START;

	switch (acb->adapter_type) {
	case ACB_ADAPTER_TYPE_A: {
		struct MessageUnit_A __iomem *reg = acb->pmuA;

		if (arcmsr_cdb->Flags & ARCMSR_CDB_FLAG_SGL_BSIZE)
			writel(cdb_shifted_phyaddr | ARCMSR_CCBPOST_FLAG_SGL_BSIZE,
			&reg->inbound_queueport);
		else {
				writel(cdb_shifted_phyaddr, &reg->inbound_queueport);
		}
		}
		break;

	case ACB_ADAPTER_TYPE_B: {
		struct MessageUnit_B *reg = acb->pmuB;
		uint32_t ending_index, index = reg->postq_index;

		ending_index = ((index + 1) % ARCMSR_MAX_HBB_POSTQUEUE);
		writel(0, &reg->post_qbuffer[ending_index]);
		if (arcmsr_cdb->Flags & ARCMSR_CDB_FLAG_SGL_BSIZE) {
			writel(cdb_shifted_phyaddr | ARCMSR_CCBPOST_FLAG_SGL_BSIZE,\
						 &reg->post_qbuffer[index]);
		}
		else {
			writel(cdb_shifted_phyaddr, &reg->post_qbuffer[index]);
		}
		index++;
		index %= ARCMSR_MAX_HBB_POSTQUEUE;/*if last index number set it to 0 */
		reg->postq_index = index;
		writel(ARCMSR_DRV2IOP_CDB_POSTED, reg->drv2iop_doorbell_reg);
		}
		break;
	}
}

static void arcmsr_stop_hba_bgrb(struct AdapterControlBlock *acb)
{
	struct MessageUnit_A __iomem *reg = acb->pmuA;
	acb->acb_flags &= ~ACB_F_MSG_START_BGRB;
	writel(ARCMSR_INBOUND_MESG0_STOP_BGRB, &reg->inbound_msgaddr0);

	if (arcmsr_hba_wait_msgint_ready(acb)) {
		printk(KERN_NOTICE
			"arcmsr%d: wait 'stop adapter background rebulid' timeout \n"
			, acb->host->host_no);
	}
}

static void arcmsr_stop_hbb_bgrb(struct AdapterControlBlock *acb)
{
	struct MessageUnit_B *reg = acb->pmuB;
	acb->acb_flags &= ~ACB_F_MSG_START_BGRB;
	writel(ARCMSR_MESSAGE_STOP_BGRB, reg->drv2iop_doorbell_reg);

	if (arcmsr_hbb_wait_msgint_ready(acb)) {
		printk(KERN_NOTICE
			"arcmsr%d: wait 'stop adapter background rebulid' timeout \n"
			, acb->host->host_no);
	}
}

static void arcmsr_stop_adapter_bgrb(struct AdapterControlBlock *acb)
{
	switch (acb->adapter_type) {
	case ACB_ADAPTER_TYPE_A: {
		arcmsr_stop_hba_bgrb(acb);
		}
		break;

	case ACB_ADAPTER_TYPE_B: {
		arcmsr_stop_hbb_bgrb(acb);
		}
		break;
	}
}

static void arcmsr_free_ccb_pool(struct AdapterControlBlock *acb)
{
	switch (acb->adapter_type) {
	case ACB_ADAPTER_TYPE_A: {
		iounmap(acb->pmuA);
		dma_free_coherent(&acb->pdev->dev,
		ARCMSR_MAX_FREECCB_NUM * sizeof (struct CommandControlBlock) + 0x20,
		acb->dma_coherent,
		acb->dma_coherent_handle);
		break;
	}
	case ACB_ADAPTER_TYPE_B: {
		struct MessageUnit_B *reg = acb->pmuB;
		iounmap((u8 *)reg->drv2iop_doorbell_reg - ARCMSR_DRV2IOP_DOORBELL);
		iounmap((u8 *)reg->ioctl_wbuffer_reg - ARCMSR_IOCTL_WBUFFER);
		dma_free_coherent(&acb->pdev->dev,
		(ARCMSR_MAX_FREECCB_NUM * sizeof(struct CommandControlBlock) + 0x20 +
		sizeof(struct MessageUnit_B)), acb->dma_coherent, acb->dma_coherent_handle);
	}
	}

}

void arcmsr_iop_message_read(struct AdapterControlBlock *acb)
{
	switch (acb->adapter_type) {
	case ACB_ADAPTER_TYPE_A: {
		struct MessageUnit_A __iomem *reg = acb->pmuA;
		writel(ARCMSR_INBOUND_DRIVER_DATA_READ_OK, &reg->inbound_doorbell);
		}
		break;

	case ACB_ADAPTER_TYPE_B: {
		struct MessageUnit_B *reg = acb->pmuB;
		writel(ARCMSR_DRV2IOP_DATA_READ_OK, reg->drv2iop_doorbell_reg);
		}
		break;
	}
}

static void arcmsr_iop_message_wrote(struct AdapterControlBlock *acb)
{
	switch (acb->adapter_type) {
	case ACB_ADAPTER_TYPE_A: {
		struct MessageUnit_A __iomem *reg = acb->pmuA;
		/*
		** push inbound doorbell tell iop, driver data write ok
		** and wait reply on next hwinterrupt for next Qbuffer post
		*/
		writel(ARCMSR_INBOUND_DRIVER_DATA_WRITE_OK, &reg->inbound_doorbell);
		}
		break;

	case ACB_ADAPTER_TYPE_B: {
		struct MessageUnit_B *reg = acb->pmuB;
		/*
		** push inbound doorbell tell iop, driver data write ok
		** and wait reply on next hwinterrupt for next Qbuffer post
		*/
		writel(ARCMSR_DRV2IOP_DATA_WRITE_OK, reg->drv2iop_doorbell_reg);
		}
		break;
	}
}

struct QBUFFER __iomem *arcmsr_get_iop_rqbuffer(struct AdapterControlBlock *acb)
{
	struct QBUFFER __iomem *qbuffer = NULL;

	switch (acb->adapter_type) {

	case ACB_ADAPTER_TYPE_A: {
		struct MessageUnit_A __iomem *reg = acb->pmuA;
		qbuffer = (struct QBUFFER __iomem *)&reg->message_rbuffer;
		}
		break;

	case ACB_ADAPTER_TYPE_B: {
		struct MessageUnit_B *reg = acb->pmuB;
		qbuffer = (struct QBUFFER __iomem *)reg->ioctl_rbuffer_reg;
		}
		break;
	}
	return qbuffer;
}

static struct QBUFFER __iomem *arcmsr_get_iop_wqbuffer(struct AdapterControlBlock *acb)
{
	struct QBUFFER __iomem *pqbuffer = NULL;

	switch (acb->adapter_type) {

	case ACB_ADAPTER_TYPE_A: {
		struct MessageUnit_A __iomem *reg = acb->pmuA;
		pqbuffer = (struct QBUFFER __iomem *) &reg->message_wbuffer;
		}
		break;

	case ACB_ADAPTER_TYPE_B: {
		struct MessageUnit_B  *reg = acb->pmuB;
		pqbuffer = (struct QBUFFER __iomem *)reg->ioctl_wbuffer_reg;
		}
		break;
	}
	return pqbuffer;
}

static void arcmsr_iop2drv_data_wrote_handle(struct AdapterControlBlock *acb)
{
	struct QBUFFER __iomem *prbuffer;
	struct QBUFFER *pQbuffer;
	uint8_t __iomem *iop_data;
	int32_t my_empty_len, iop_len, rqbuf_firstindex, rqbuf_lastindex;

	rqbuf_lastindex = acb->rqbuf_lastindex;
	rqbuf_firstindex = acb->rqbuf_firstindex;
	prbuffer = arcmsr_get_iop_rqbuffer(acb);
	iop_data = (uint8_t __iomem *)prbuffer->data;
	iop_len = prbuffer->data_len;
	my_empty_len = (rqbuf_firstindex - rqbuf_lastindex -1)&(ARCMSR_MAX_QBUFFER -1);

	if (my_empty_len >= iop_len)
	{
		while (iop_len > 0) {
			pQbuffer = (struct QBUFFER *)&acb->rqbuffer[rqbuf_lastindex];
			memcpy(pQbuffer, iop_data,1);
			rqbuf_lastindex++;
			rqbuf_lastindex %= ARCMSR_MAX_QBUFFER;
			iop_data++;
			iop_len--;
		}
		acb->rqbuf_lastindex = rqbuf_lastindex;
		arcmsr_iop_message_read(acb);
	}

	else {
		acb->acb_flags |= ACB_F_IOPDATA_OVERFLOW;
	}
}

static void arcmsr_iop2drv_data_read_handle(struct AdapterControlBlock *acb)
{
	acb->acb_flags |= ACB_F_MESSAGE_WQBUFFER_READED;
	if (acb->wqbuf_firstindex != acb->wqbuf_lastindex) {
		uint8_t *pQbuffer;
		struct QBUFFER __iomem *pwbuffer;
		uint8_t __iomem *iop_data;
		int32_t allxfer_len = 0;

		acb->acb_flags &= (~ACB_F_MESSAGE_WQBUFFER_READED);
		pwbuffer = arcmsr_get_iop_wqbuffer(acb);
		iop_data = (uint8_t __iomem *)pwbuffer->data;

		while ((acb->wqbuf_firstindex != acb->wqbuf_lastindex) && \
							(allxfer_len < 124)) {
			pQbuffer = &acb->wqbuffer[acb->wqbuf_firstindex];
			memcpy(iop_data, pQbuffer, 1);
			acb->wqbuf_firstindex++;
			acb->wqbuf_firstindex %= ARCMSR_MAX_QBUFFER;
			iop_data++;
			allxfer_len++;
		}
		pwbuffer->data_len = allxfer_len;

		arcmsr_iop_message_wrote(acb);
	}

	if (acb->wqbuf_firstindex == acb->wqbuf_lastindex) {
		acb->acb_flags |= ACB_F_MESSAGE_WQBUFFER_CLEARED;
	}
}

static void arcmsr_hba_doorbell_isr(struct AdapterControlBlock *acb)
{
	uint32_t outbound_doorbell;
	struct MessageUnit_A __iomem *reg = acb->pmuA;

	outbound_doorbell = readl(&reg->outbound_doorbell);
	writel(outbound_doorbell, &reg->outbound_doorbell);
	if (outbound_doorbell & ARCMSR_OUTBOUND_IOP331_DATA_WRITE_OK) {
		arcmsr_iop2drv_data_wrote_handle(acb);
	}

	if (outbound_doorbell & ARCMSR_OUTBOUND_IOP331_DATA_READ_OK) 	{
		arcmsr_iop2drv_data_read_handle(acb);
	}
}

static void arcmsr_hba_postqueue_isr(struct AdapterControlBlock *acb)
{
	uint32_t flag_ccb;
	struct MessageUnit_A __iomem *reg = acb->pmuA;

	while ((flag_ccb = readl(&reg->outbound_queueport)) != 0xFFFFFFFF) {
		arcmsr_drain_donequeue(acb, flag_ccb);
	}
}

static void arcmsr_hbb_postqueue_isr(struct AdapterControlBlock *acb)
{
	uint32_t index;
	uint32_t flag_ccb;
	struct MessageUnit_B *reg = acb->pmuB;

	index = reg->doneq_index;

	while ((flag_ccb = readl(&reg->done_qbuffer[index])) != 0) {
		writel(0, &reg->done_qbuffer[index]);
		arcmsr_drain_donequeue(acb, flag_ccb);
		index++;
		index %= ARCMSR_MAX_HBB_POSTQUEUE;
		reg->doneq_index = index;
	}
}
static void arcmsr_hba_message_isr(struct AdapterControlBlock *acb)
{
	struct MessageUnit_A *reg  = acb->pmuA;

	/*clear interrupt and message state*/
	writel(ARCMSR_MU_OUTBOUND_MESSAGE0_INT, &reg->outbound_intstatus);
	schedule_work(&acb->arcmsr_do_message_isr_bh);
}
static void arcmsr_hbb_message_isr(struct AdapterControlBlock *acb)
{
	struct MessageUnit_B *reg  = acb->pmuB;

	/*clear interrupt and message state*/
	writel(ARCMSR_MESSAGE_INT_CLEAR_PATTERN, reg->iop2drv_doorbell_reg);
	schedule_work(&acb->arcmsr_do_message_isr_bh);
}
static int arcmsr_handle_hba_isr(struct AdapterControlBlock *acb)
{
	uint32_t outbound_intstatus;
	struct MessageUnit_A __iomem *reg = acb->pmuA;

	outbound_intstatus = readl(&reg->outbound_intstatus) &
							acb->outbound_int_enable;
	if (!(outbound_intstatus & ARCMSR_MU_OUTBOUND_HANDLE_INT))	{
		return 1;
	}
	writel(outbound_intstatus, &reg->outbound_intstatus);
	if (outbound_intstatus & ARCMSR_MU_OUTBOUND_DOORBELL_INT)	{
		arcmsr_hba_doorbell_isr(acb);
	}
	if (outbound_intstatus & ARCMSR_MU_OUTBOUND_POSTQUEUE_INT) {
		arcmsr_hba_postqueue_isr(acb);
	}
	if (outbound_intstatus & ARCMSR_MU_OUTBOUND_MESSAGE0_INT) 	{
		/* messenger of "driver to iop commands" */
		arcmsr_hba_message_isr(acb);
	}
	return 0;
}

static int arcmsr_handle_hbb_isr(struct AdapterControlBlock *acb)
{
	uint32_t outbound_doorbell;
	struct MessageUnit_B *reg = acb->pmuB;

	outbound_doorbell = readl(reg->iop2drv_doorbell_reg) &
							acb->outbound_int_enable;
	if (!outbound_doorbell)
		return 1;

	writel(~outbound_doorbell, reg->iop2drv_doorbell_reg);
	/*in case the last action of doorbell interrupt clearance is cached,
	this action can push HW to write down the clear bit*/
	readl(reg->iop2drv_doorbell_reg);
	writel(ARCMSR_DRV2IOP_END_OF_INTERRUPT, reg->drv2iop_doorbell_reg);
	if (outbound_doorbell & ARCMSR_IOP2DRV_DATA_WRITE_OK) 	{
		arcmsr_iop2drv_data_wrote_handle(acb);
	}
	if (outbound_doorbell & ARCMSR_IOP2DRV_DATA_READ_OK) {
		arcmsr_iop2drv_data_read_handle(acb);
	}
	if (outbound_doorbell & ARCMSR_IOP2DRV_CDB_DONE) {
		arcmsr_hbb_postqueue_isr(acb);
	}
	if (outbound_doorbell & ARCMSR_IOP2DRV_MESSAGE_CMD_DONE) {
		/* messenger of "driver to iop commands" */
		arcmsr_hbb_message_isr(acb);
	}

	return 0;
}

static irqreturn_t arcmsr_interrupt(struct AdapterControlBlock *acb)
{
	switch (acb->adapter_type) {
	case ACB_ADAPTER_TYPE_A: {
		if (arcmsr_handle_hba_isr(acb)) {
			return IRQ_NONE;
		}
		}
		break;

	case ACB_ADAPTER_TYPE_B: {
		if (arcmsr_handle_hbb_isr(acb)) {
			return IRQ_NONE;
		}
		}
		break;
	}
	return IRQ_HANDLED;
}

static void arcmsr_iop_parking(struct AdapterControlBlock *acb)
{
	if (acb) {
		/* stop adapter background rebuild */
		if (acb->acb_flags & ACB_F_MSG_START_BGRB) {
			uint32_t intmask_org;
			acb->acb_flags &= ~ACB_F_MSG_START_BGRB;
			intmask_org = arcmsr_disable_outbound_ints(acb);
			arcmsr_stop_adapter_bgrb(acb);
			arcmsr_flush_adapter_cache(acb);
			arcmsr_enable_outbound_ints(acb, intmask_org);
		}
	}
}

void arcmsr_post_ioctldata2iop(struct AdapterControlBlock *acb)
{
	int32_t wqbuf_firstindex, wqbuf_lastindex;
	uint8_t *pQbuffer;
	struct QBUFFER __iomem *pwbuffer;
	uint8_t __iomem *iop_data;
	int32_t allxfer_len = 0;

	pwbuffer = arcmsr_get_iop_wqbuffer(acb);
	iop_data = (uint8_t __iomem *)pwbuffer->data;
	if (acb->acb_flags & ACB_F_MESSAGE_WQBUFFER_READED) {
		acb->acb_flags &= (~ACB_F_MESSAGE_WQBUFFER_READED);
		wqbuf_firstindex = acb->wqbuf_firstindex;
		wqbuf_lastindex = acb->wqbuf_lastindex;
		while ((wqbuf_firstindex != wqbuf_lastindex) && (allxfer_len < 124)) {
			pQbuffer = &acb->wqbuffer[wqbuf_firstindex];
			memcpy(iop_data, pQbuffer, 1);
			wqbuf_firstindex++;
			wqbuf_firstindex %= ARCMSR_MAX_QBUFFER;
			iop_data++;
			allxfer_len++;
		}
		acb->wqbuf_firstindex = wqbuf_firstindex;
		pwbuffer->data_len = allxfer_len;
		arcmsr_iop_message_wrote(acb);
	}
}

static int arcmsr_iop_message_xfer(struct AdapterControlBlock *acb,
					struct scsi_cmnd *cmd)
{
	struct CMD_MESSAGE_FIELD *pcmdmessagefld;
	int retvalue = 0, transfer_len = 0;
	char *buffer;
	struct scatterlist *sg;
	uint32_t controlcode = (uint32_t ) cmd->cmnd[5] << 24 |
						(uint32_t ) cmd->cmnd[6] << 16 |
						(uint32_t ) cmd->cmnd[7] << 8  |
						(uint32_t ) cmd->cmnd[8];
						/* 4 bytes: Areca io control code */

	sg = scsi_sglist(cmd);
	buffer = kmap_atomic(sg_page(sg), KM_IRQ0) + sg->offset;
	if (scsi_sg_count(cmd) > 1) {
		retvalue = ARCMSR_MESSAGE_FAIL;
		goto message_out;
	}
	transfer_len += sg->length;

	if (transfer_len > sizeof(struct CMD_MESSAGE_FIELD)) {
		retvalue = ARCMSR_MESSAGE_FAIL;
		goto message_out;
	}
	pcmdmessagefld = (struct CMD_MESSAGE_FIELD *) buffer;
	switch(controlcode) {

	case ARCMSR_MESSAGE_READ_RQBUFFER: {
		unsigned char *ver_addr;
		uint8_t *pQbuffer, *ptmpQbuffer;
		int32_t allxfer_len = 0;

		ver_addr = kmalloc(1032, GFP_ATOMIC);
		if (!ver_addr) {
			retvalue = ARCMSR_MESSAGE_FAIL;
			goto message_out;
		}

		if (!acb->fw_state) {
			pcmdmessagefld->cmdmessage.ReturnCode =
			ARCMSR_MESSAGE_RETURNCODE_BUS_HANG_ON;
			goto message_out;
		}

		ptmpQbuffer = ver_addr;
		while ((acb->rqbuf_firstindex != acb->rqbuf_lastindex)
			&& (allxfer_len < 1031)) {
			pQbuffer = &acb->rqbuffer[acb->rqbuf_firstindex];
			memcpy(ptmpQbuffer, pQbuffer, 1);
			acb->rqbuf_firstindex++;
			acb->rqbuf_firstindex %= ARCMSR_MAX_QBUFFER;
			ptmpQbuffer++;
			allxfer_len++;
		}
		if (acb->acb_flags & ACB_F_IOPDATA_OVERFLOW) {

			struct QBUFFER __iomem *prbuffer;
			uint8_t __iomem *iop_data;
			int32_t iop_len;

			acb->acb_flags &= ~ACB_F_IOPDATA_OVERFLOW;
			prbuffer = arcmsr_get_iop_rqbuffer(acb);
			iop_data = prbuffer->data;
			iop_len = readl(&prbuffer->data_len);
			while (iop_len > 0) {
				acb->rqbuffer[acb->rqbuf_lastindex] = readb(iop_data);
				acb->rqbuf_lastindex++;
				acb->rqbuf_lastindex %= ARCMSR_MAX_QBUFFER;
				iop_data++;
				iop_len--;
			}
			arcmsr_iop_message_read(acb);
		}
		memcpy(pcmdmessagefld->messagedatabuffer, ver_addr, allxfer_len);
		pcmdmessagefld->cmdmessage.Length = allxfer_len;
		pcmdmessagefld->cmdmessage.ReturnCode = ARCMSR_MESSAGE_RETURNCODE_OK;
		kfree(ver_addr);
		}
		break;

	case ARCMSR_MESSAGE_WRITE_WQBUFFER: {
		unsigned char *ver_addr;
		int32_t my_empty_len, user_len, wqbuf_firstindex, wqbuf_lastindex;
		uint8_t *pQbuffer, *ptmpuserbuffer;

		ver_addr = kmalloc(1032, GFP_ATOMIC);
		if (!ver_addr) {
			retvalue = ARCMSR_MESSAGE_FAIL;
			goto message_out;
		}
		if (!acb->fw_state) {
			pcmdmessagefld->cmdmessage.ReturnCode =
			ARCMSR_MESSAGE_RETURNCODE_BUS_HANG_ON;
			goto message_out;
		}

		ptmpuserbuffer = ver_addr;
		user_len = pcmdmessagefld->cmdmessage.Length;
		memcpy(ptmpuserbuffer, pcmdmessagefld->messagedatabuffer, user_len);
		wqbuf_lastindex = acb->wqbuf_lastindex;
		wqbuf_firstindex = acb->wqbuf_firstindex;
		if (wqbuf_lastindex != wqbuf_firstindex) {
			struct SENSE_DATA *sensebuffer =
				(struct SENSE_DATA *)cmd->sense_buffer;
			arcmsr_post_ioctldata2iop(acb);
			/* has error report sensedata */
			sensebuffer->ErrorCode = 0x70;
			sensebuffer->SenseKey = ILLEGAL_REQUEST;
			sensebuffer->AdditionalSenseLength = 0x0A;
			sensebuffer->AdditionalSenseCode = 0x20;
			sensebuffer->Valid = 1;
			retvalue = ARCMSR_MESSAGE_FAIL;
		} else {
			my_empty_len = (wqbuf_firstindex-wqbuf_lastindex - 1)
				&(ARCMSR_MAX_QBUFFER - 1);
			if (my_empty_len >= user_len) {
				while (user_len > 0) {
					pQbuffer =
					&acb->wqbuffer[acb->wqbuf_lastindex];
					memcpy(pQbuffer, ptmpuserbuffer, 1);
					acb->wqbuf_lastindex++;
					acb->wqbuf_lastindex %= ARCMSR_MAX_QBUFFER;
					ptmpuserbuffer++;
					user_len--;
				}
				if (acb->acb_flags & ACB_F_MESSAGE_WQBUFFER_CLEARED) {
					acb->acb_flags &=
						~ACB_F_MESSAGE_WQBUFFER_CLEARED;
					arcmsr_post_ioctldata2iop(acb);
				}
			} else {
				/* has error report sensedata */
				struct SENSE_DATA *sensebuffer =
					(struct SENSE_DATA *)cmd->sense_buffer;
				sensebuffer->ErrorCode = 0x70;
				sensebuffer->SenseKey = ILLEGAL_REQUEST;
				sensebuffer->AdditionalSenseLength = 0x0A;
				sensebuffer->AdditionalSenseCode = 0x20;
				sensebuffer->Valid = 1;
				retvalue = ARCMSR_MESSAGE_FAIL;
			}
			}
			kfree(ver_addr);
		}
		break;

	case ARCMSR_MESSAGE_CLEAR_RQBUFFER: {
		uint8_t *pQbuffer = acb->rqbuffer;
		if (!acb->fw_state) {
			pcmdmessagefld->cmdmessage.ReturnCode =
			ARCMSR_MESSAGE_RETURNCODE_BUS_HANG_ON;
			goto message_out;
		}

		if (acb->acb_flags & ACB_F_IOPDATA_OVERFLOW) {
			acb->acb_flags &= ~ACB_F_IOPDATA_OVERFLOW;
			arcmsr_iop_message_read(acb);
		}
		acb->acb_flags |= ACB_F_MESSAGE_RQBUFFER_CLEARED;
		acb->rqbuf_firstindex = 0;
		acb->rqbuf_lastindex = 0;
		memset(pQbuffer, 0, ARCMSR_MAX_QBUFFER);
		pcmdmessagefld->cmdmessage.ReturnCode = ARCMSR_MESSAGE_RETURNCODE_OK;
		}
		break;

	case ARCMSR_MESSAGE_CLEAR_WQBUFFER: {
		uint8_t *pQbuffer = acb->wqbuffer;
		if (!acb->fw_state) {
			pcmdmessagefld->cmdmessage.ReturnCode =
			ARCMSR_MESSAGE_RETURNCODE_BUS_HANG_ON;
			goto message_out;
		}

		if (acb->acb_flags & ACB_F_IOPDATA_OVERFLOW) {
			acb->acb_flags &= ~ACB_F_IOPDATA_OVERFLOW;
			arcmsr_iop_message_read(acb);
		}
		acb->acb_flags |=
			(ACB_F_MESSAGE_WQBUFFER_CLEARED |
				ACB_F_MESSAGE_WQBUFFER_READED);
		acb->wqbuf_firstindex = 0;
		acb->wqbuf_lastindex = 0;
		memset(pQbuffer, 0, ARCMSR_MAX_QBUFFER);
		pcmdmessagefld->cmdmessage.ReturnCode =
			ARCMSR_MESSAGE_RETURNCODE_OK;
		}
		break;

	case ARCMSR_MESSAGE_CLEAR_ALLQBUFFER: {
		uint8_t *pQbuffer;
		if (!acb->fw_state) {
			pcmdmessagefld->cmdmessage.ReturnCode =
			ARCMSR_MESSAGE_RETURNCODE_BUS_HANG_ON;
			goto message_out;
		}

		if (acb->acb_flags & ACB_F_IOPDATA_OVERFLOW) {
			acb->acb_flags &= ~ACB_F_IOPDATA_OVERFLOW;
			arcmsr_iop_message_read(acb);
		}
		acb->acb_flags |=
			(ACB_F_MESSAGE_WQBUFFER_CLEARED
			| ACB_F_MESSAGE_RQBUFFER_CLEARED
			| ACB_F_MESSAGE_WQBUFFER_READED);
		acb->rqbuf_firstindex = 0;
		acb->rqbuf_lastindex = 0;
		acb->wqbuf_firstindex = 0;
		acb->wqbuf_lastindex = 0;
		pQbuffer = acb->rqbuffer;
		memset(pQbuffer, 0, sizeof(struct QBUFFER));
		pQbuffer = acb->wqbuffer;
		memset(pQbuffer, 0, sizeof(struct QBUFFER));
		pcmdmessagefld->cmdmessage.ReturnCode = ARCMSR_MESSAGE_RETURNCODE_OK;
		}
		break;

	case ARCMSR_MESSAGE_RETURN_CODE_3F: {
		if (!acb->fw_state) {
			pcmdmessagefld->cmdmessage.ReturnCode =
			ARCMSR_MESSAGE_RETURNCODE_BUS_HANG_ON;
			goto message_out;
		}
		pcmdmessagefld->cmdmessage.ReturnCode = ARCMSR_MESSAGE_RETURNCODE_3F;
		}
		break;

	case ARCMSR_MESSAGE_SAY_HELLO: {
		int8_t *hello_string = "Hello! I am ARCMSR";
		if (!acb->fw_state) {
			pcmdmessagefld->cmdmessage.ReturnCode =
			ARCMSR_MESSAGE_RETURNCODE_BUS_HANG_ON;
			goto message_out;
		}
		memcpy(pcmdmessagefld->messagedatabuffer, hello_string
			, (int16_t)strlen(hello_string));
		pcmdmessagefld->cmdmessage.ReturnCode = ARCMSR_MESSAGE_RETURNCODE_OK;
		}
		break;

	case ARCMSR_MESSAGE_SAY_GOODBYE:
		if (!acb->fw_state) {
			pcmdmessagefld->cmdmessage.ReturnCode =
			ARCMSR_MESSAGE_RETURNCODE_BUS_HANG_ON;
			goto message_out;
		}
		arcmsr_iop_parking(acb);
		break;

	case ARCMSR_MESSAGE_FLUSH_ADAPTER_CACHE:
		if (!acb->fw_state) {
			pcmdmessagefld->cmdmessage.ReturnCode =
			ARCMSR_MESSAGE_RETURNCODE_BUS_HANG_ON;
			goto message_out;
		}
		arcmsr_flush_adapter_cache(acb);
		break;

	default:
		retvalue = ARCMSR_MESSAGE_FAIL;
	}
	message_out:
	sg = scsi_sglist(cmd);
	kunmap_atomic(buffer - sg->offset, KM_IRQ0);
	return retvalue;
}

static struct CommandControlBlock *arcmsr_get_freeccb(struct AdapterControlBlock *acb)
{
	struct list_head *head = &acb->ccb_free_list;
	struct CommandControlBlock *ccb = NULL;

	if (!list_empty(head)) {
		ccb = list_entry(head->next, struct CommandControlBlock, list);
		list_del(head->next);
	}
	return ccb;
}

static void arcmsr_handle_virtual_command(struct AdapterControlBlock *acb,
		struct scsi_cmnd *cmd)
{
	switch (cmd->cmnd[0]) {
	case INQUIRY: {
		unsigned char inqdata[36];
		char *buffer;
		struct scatterlist *sg;

		if (cmd->device->lun) {
			cmd->result = (DID_TIME_OUT << 16);
			cmd->scsi_done(cmd);
			return;
		}
		inqdata[0] = TYPE_PROCESSOR;
		/* Periph Qualifier & Periph Dev Type */
		inqdata[1] = 0;
		/* rem media bit & Dev Type Modifier */
		inqdata[2] = 0;
		/* ISO, ECMA, & ANSI versions */
		inqdata[4] = 31;
		/* length of additional data */
		strncpy(&inqdata[8], "Areca   ", 8);
		/* Vendor Identification */
		strncpy(&inqdata[16], "RAID controller ", 16);
		/* Product Identification */
		strncpy(&inqdata[32], "R001", 4); /* Product Revision */

		sg = scsi_sglist(cmd);
		buffer = kmap_atomic(sg_page(sg), KM_IRQ0) + sg->offset;

		memcpy(buffer, inqdata, sizeof(inqdata));
		sg = scsi_sglist(cmd);
		kunmap_atomic(buffer - sg->offset, KM_IRQ0);

		cmd->scsi_done(cmd);
	}
	break;
	case WRITE_BUFFER:
	case READ_BUFFER: {
		if (arcmsr_iop_message_xfer(acb, cmd))
			cmd->result = (DID_ERROR << 16);
		cmd->scsi_done(cmd);
	}
	break;
	default:
		cmd->scsi_done(cmd);
	}
}

static int arcmsr_queue_command(struct scsi_cmnd *cmd,
	void (* done)(struct scsi_cmnd *))
{
	struct Scsi_Host *host = cmd->device->host;
	struct AdapterControlBlock *acb = (struct AdapterControlBlock *) host->hostdata;
	struct CommandControlBlock *ccb;
	int target = cmd->device->id;
	int lun = cmd->device->lun;
	uint8_t scsicmd = cmd->cmnd[0];
	cmd->scsi_done = done;
	cmd->host_scribble = NULL;
	cmd->result = 0;

	if ((scsicmd == SYNCHRONIZE_CACHE) || (scsicmd == SEND_DIAGNOSTIC)) {
		if (acb->devstate[target][lun] == ARECA_RAID_GONE) {
			cmd->result = (DID_NO_CONNECT << 16);
		}
		cmd->scsi_done(cmd);
		return 0;
	}

	if (acb->acb_flags & ACB_F_BUS_RESET) {
		switch (acb->adapter_type) {
			case ACB_ADAPTER_TYPE_A: {
				struct MessageUnit_A __iomem *reg = acb->pmuA;
				uint32_t intmask_org, outbound_doorbell;

				if ((readl(&reg->outbound_msgaddr1) &
					ARCMSR_OUTBOUND_MESG1_FIRMWARE_OK) == 0) {
					printk(KERN_NOTICE "arcmsr%d: bus reset and return busy\n",
						acb->host->host_no);
		return SCSI_MLQUEUE_HOST_BUSY;
	}

				acb->acb_flags &= ~ACB_F_FIRMWARE_TRAP;
				printk(KERN_NOTICE "arcmsr%d: hardware bus reset and reset ok\n",
					acb->host->host_no);
				/* disable all outbound interrupt */
				intmask_org = arcmsr_disable_outbound_ints(acb);
				arcmsr_get_firmware_spec(acb, 1);
				/*start background rebuild*/
				arcmsr_start_adapter_bgrb(acb);
				/* clear Qbuffer if door bell ringed */
				outbound_doorbell = readl(&reg->outbound_doorbell);
				/*clear interrupt */
				writel(outbound_doorbell, &reg->outbound_doorbell);
				writel(ARCMSR_INBOUND_DRIVER_DATA_READ_OK,
					&reg->inbound_doorbell);
				/* enable outbound Post Queue,outbound doorbell Interrupt */
				arcmsr_enable_outbound_ints(acb, intmask_org);
				acb->acb_flags |= ACB_F_IOP_INITED;
				acb->acb_flags &= ~ACB_F_BUS_RESET;
			}
			break;
			case ACB_ADAPTER_TYPE_B: {
			}
		}
	}

	if (target == 16) {
		/* virtual device for iop message transfer */
		arcmsr_handle_virtual_command(acb, cmd);
		return 0;
	}
	if (acb->devstate[target][lun] == ARECA_RAID_GONE) {
		uint8_t block_cmd;

		block_cmd = cmd->cmnd[0] & 0x0f;
		if (block_cmd == 0x08 || block_cmd == 0x0a) {
			printk(KERN_NOTICE
				"arcmsr%d: block 'read/write'"
				"command with gone raid volume"
				" Cmd = %2x, TargetId = %d, Lun = %d \n"
				, acb->host->host_no
				, cmd->cmnd[0]
				, target, lun);
			cmd->result = (DID_NO_CONNECT << 16);
			cmd->scsi_done(cmd);
			return 0;
		}
	}
	if (atomic_read(&acb->ccboutstandingcount) >=
			ARCMSR_MAX_OUTSTANDING_CMD)
		return SCSI_MLQUEUE_HOST_BUSY;

	ccb = arcmsr_get_freeccb(acb);
	if (!ccb)
		return SCSI_MLQUEUE_HOST_BUSY;
	if ( arcmsr_build_ccb( acb, ccb, cmd ) == FAILED ) {
		cmd->result = (DID_ERROR << 16) | (RESERVATION_CONFLICT << 1);
		cmd->scsi_done(cmd);
		return 0;
	}
	arcmsr_post_ccb(acb, ccb);
	return 0;
}

static void *arcmsr_get_hba_config(struct AdapterControlBlock *acb, int mode)
{
	struct MessageUnit_A __iomem *reg = acb->pmuA;
	char *acb_firm_model = acb->firm_model;
	char *acb_firm_version = acb->firm_version;
	char *acb_device_map = acb->device_map;
	char __iomem *iop_firm_model = (char __iomem *)(&reg->message_rwbuffer[15]);
	char __iomem *iop_firm_version = (char __iomem *)(&reg->message_rwbuffer[17]);
	char __iomem *iop_device_map = (char __iomem *) (&reg->message_rwbuffer[21]);
	int count;

	writel(ARCMSR_INBOUND_MESG0_GET_CONFIG, &reg->inbound_msgaddr0);
	if (arcmsr_hba_wait_msgint_ready(acb)) {
		printk(KERN_NOTICE "arcmsr%d: wait 'get adapter firmware \
			miscellaneous data' timeout \n", acb->host->host_no);
		return NULL;
	}

	if (mode == 1) {
	count = 8;
	while (count) {
		*acb_firm_model = readb(iop_firm_model);
		acb_firm_model++;
		iop_firm_model++;
		count--;
	}

	count = 16;
	while (count) {
		*acb_firm_version = readb(iop_firm_version);
		acb_firm_version++;
		iop_firm_version++;
		count--;
	}

		count = 16;
		while (count) {
			*acb_device_map = readb(iop_device_map);
			acb_device_map++;
			iop_device_map++;
			count--;
		}

	printk(KERN_INFO 	"ARECA RAID ADAPTER%d: FIRMWARE VERSION %s \n"
		, acb->host->host_no
		, acb->firm_version);
		acb->signature = readl(&reg->message_rwbuffer[0]);
	acb->firm_request_len = readl(&reg->message_rwbuffer[1]);
	acb->firm_numbers_queue = readl(&reg->message_rwbuffer[2]);
	acb->firm_sdram_size = readl(&reg->message_rwbuffer[3]);
	acb->firm_hd_channels = readl(&reg->message_rwbuffer[4]);
}
	return reg->message_rwbuffer;
}
static void __iomem *arcmsr_get_hbb_config(struct AdapterControlBlock *acb, int mode)
{
	struct MessageUnit_B *reg = acb->pmuB;
	uint32_t __iomem *lrwbuffer = reg->msgcode_rwbuffer_reg;
	char *acb_firm_model = acb->firm_model;
	char *acb_firm_version = acb->firm_version;
	char *acb_device_map = acb->device_map;
	char __iomem *iop_firm_model = (char __iomem *)(&lrwbuffer[15]);
	/*firm_model,15,60-67*/
	char __iomem *iop_firm_version = (char __iomem *)(&lrwbuffer[17]);
	/*firm_version,17,68-83*/
	char __iomem *iop_device_map = (char __iomem *) (&lrwbuffer[21]);
	/*firm_version,21,84-99*/
	int count;

	writel(ARCMSR_MESSAGE_GET_CONFIG, reg->drv2iop_doorbell_reg);
	if (arcmsr_hbb_wait_msgint_ready(acb)) {
		printk(KERN_NOTICE "arcmsr%d: wait 'get adapter firmware \
			miscellaneous data' timeout \n", acb->host->host_no);
		return NULL;
	}

	if (mode == 1) {
	count = 8;
	while (count)
	{
		*acb_firm_model = readb(iop_firm_model);
		acb_firm_model++;
		iop_firm_model++;
		count--;
	}

	count = 16;
	while (count)
	{
		*acb_firm_version = readb(iop_firm_version);
		acb_firm_version++;
		iop_firm_version++;
		count--;
	}

		count = 16;
		while (count) {
			*acb_device_map = readb(iop_device_map);
			acb_device_map++;
			iop_device_map++;
			count--;
		}

	printk(KERN_INFO "ARECA RAID ADAPTER%d: FIRMWARE VERSION %s \n",
			acb->host->host_no,
			acb->firm_version);

		acb->signature = readl(lrwbuffer++);
		/*firm_signature,1,00-03*/
	acb->firm_request_len = readl(lrwbuffer++);
	/*firm_request_len,1,04-07*/
	acb->firm_numbers_queue = readl(lrwbuffer++);
	/*firm_numbers_queue,2,08-11*/
	acb->firm_sdram_size = readl(lrwbuffer++);
	/*firm_sdram_size,3,12-15*/
	acb->firm_hd_channels = readl(lrwbuffer);
	/*firm_ide_channels,4,16-19*/
}
	return reg->msgcode_rwbuffer_reg;
}
static void *arcmsr_get_firmware_spec(struct AdapterControlBlock *acb, int mode)
{
	void *rtnval = 0;
	switch (acb->adapter_type) {
	case ACB_ADAPTER_TYPE_A: {
		rtnval = arcmsr_get_hba_config(acb, mode);
		}
		break;

	case ACB_ADAPTER_TYPE_B: {
		rtnval = arcmsr_get_hbb_config(acb, mode);
		}
		break;
	}
	return rtnval;
}

static void arcmsr_polling_hba_ccbdone(struct AdapterControlBlock *acb,
	struct CommandControlBlock *poll_ccb)
{
	struct MessageUnit_A __iomem *reg = acb->pmuA;
	struct CommandControlBlock *ccb;
	uint32_t flag_ccb, outbound_intstatus, poll_ccb_done = 0, poll_count = 0;

	polling_hba_ccb_retry:
	poll_count++;
	outbound_intstatus = readl(&reg->outbound_intstatus) & acb->outbound_int_enable;
	writel(outbound_intstatus, &reg->outbound_intstatus);/*clear interrupt*/
	while (1) {
		if ((flag_ccb = readl(&reg->outbound_queueport)) == 0xFFFFFFFF) {
			if (poll_ccb_done)
				break;
			else {
				msleep(25);
				if (poll_count > 100)
					break;
				goto polling_hba_ccb_retry;
			}
		}
		ccb = (struct CommandControlBlock *)(acb->vir2phy_offset + (flag_ccb << 5));
		poll_ccb_done = (ccb == poll_ccb) ? 1:0;
		if ((ccb->acb != acb) || (ccb->startdone != ARCMSR_CCB_START)) {
			if ((ccb->startdone == ARCMSR_CCB_ABORTED) || (ccb == poll_ccb)) {
				printk(KERN_NOTICE "arcmsr%d: scsi id = %d lun = %d ccb = '0x%p'"
					" poll command abort successfully \n"
					, acb->host->host_no
					, ccb->pcmd->device->id
					, ccb->pcmd->device->lun
					, ccb);
				ccb->pcmd->result = DID_ABORT << 16;
				arcmsr_ccb_complete(ccb, 1);
				poll_ccb_done = 1;
				continue;
			}
			printk(KERN_NOTICE "arcmsr%d: polling get an illegal ccb"
				" command done ccb = '0x%p'"
				"ccboutstandingcount = %d \n"
				, acb->host->host_no
				, ccb
				, atomic_read(&acb->ccboutstandingcount));
			continue;
		}
		arcmsr_report_ccb_state(acb, ccb, flag_ccb);
	}
}

static void arcmsr_polling_hbb_ccbdone(struct AdapterControlBlock *acb,
					struct CommandControlBlock *poll_ccb)
{
		struct MessageUnit_B *reg = acb->pmuB;
		struct CommandControlBlock *ccb;
		uint32_t flag_ccb, poll_ccb_done = 0, poll_count = 0;
		int index;

	polling_hbb_ccb_retry:
		poll_count++;
		/* clear doorbell interrupt */
		writel(ARCMSR_DOORBELL_INT_CLEAR_PATTERN, reg->iop2drv_doorbell_reg);
		while (1) {
			index = reg->doneq_index;
			if ((flag_ccb = readl(&reg->done_qbuffer[index])) == 0) {
				if (poll_ccb_done)
					break;
				else {
					msleep(25);
					if (poll_count > 100)
						break;
					goto polling_hbb_ccb_retry;
				}
			}
			writel(0, &reg->done_qbuffer[index]);
			index++;
			/*if last index number set it to 0 */
			index %= ARCMSR_MAX_HBB_POSTQUEUE;
			reg->doneq_index = index;
			/* check ifcommand done with no error*/
			ccb = (struct CommandControlBlock *)\
      (acb->vir2phy_offset + (flag_ccb << 5));/*frame must be 32 bytes aligned*/
			poll_ccb_done = (ccb == poll_ccb) ? 1:0;
			if ((ccb->acb != acb) || (ccb->startdone != ARCMSR_CCB_START)) {
				if ((ccb->startdone == ARCMSR_CCB_ABORTED) || (ccb == poll_ccb)) {
					printk(KERN_NOTICE "arcmsr%d: \
		scsi id = %d lun = %d ccb = '0x%p' poll command abort successfully \n"
						,acb->host->host_no
						,ccb->pcmd->device->id
						,ccb->pcmd->device->lun
						,ccb);
					ccb->pcmd->result = DID_ABORT << 16;
					arcmsr_ccb_complete(ccb, 1);
					continue;
				}
				printk(KERN_NOTICE "arcmsr%d: polling get an illegal ccb"
					" command done ccb = '0x%p'"
					"ccboutstandingcount = %d \n"
					, acb->host->host_no
					, ccb
					, atomic_read(&acb->ccboutstandingcount));
				continue;
			}
			arcmsr_report_ccb_state(acb, ccb, flag_ccb);
		}	/*drain reply FIFO*/
}

static void arcmsr_polling_ccbdone(struct AdapterControlBlock *acb,
					struct CommandControlBlock *poll_ccb)
{
	switch (acb->adapter_type) {

	case ACB_ADAPTER_TYPE_A: {
		arcmsr_polling_hba_ccbdone(acb,poll_ccb);
		}
		break;

	case ACB_ADAPTER_TYPE_B: {
		arcmsr_polling_hbb_ccbdone(acb,poll_ccb);
		}
	}
}

static int arcmsr_iop_confirm(struct AdapterControlBlock *acb)
{
	uint32_t cdb_phyaddr, ccb_phyaddr_hi32;
	dma_addr_t dma_coherent_handle;
	/*
	********************************************************************
	** here we need to tell iop 331 our freeccb.HighPart
	** if freeccb.HighPart is not zero
	********************************************************************
	*/
	dma_coherent_handle = acb->dma_coherent_handle;
	cdb_phyaddr = (uint32_t)(dma_coherent_handle);
	ccb_phyaddr_hi32 = (uint32_t)((cdb_phyaddr >> 16) >> 16);
	/*
	***********************************************************************
	**    if adapter type B, set window of "post command Q"
	***********************************************************************
	*/
	switch (acb->adapter_type) {

	case ACB_ADAPTER_TYPE_A: {
		if (ccb_phyaddr_hi32 != 0) {
			struct MessageUnit_A __iomem *reg = acb->pmuA;
			uint32_t intmask_org;
			intmask_org = arcmsr_disable_outbound_ints(acb);
			writel(ARCMSR_SIGNATURE_SET_CONFIG, \
						&reg->message_rwbuffer[0]);
			writel(ccb_phyaddr_hi32, &reg->message_rwbuffer[1]);
			writel(ARCMSR_INBOUND_MESG0_SET_CONFIG, \
							&reg->inbound_msgaddr0);
			if (arcmsr_hba_wait_msgint_ready(acb)) {
				printk(KERN_NOTICE "arcmsr%d: ""set ccb high \
				part physical address timeout\n",
				acb->host->host_no);
				return 1;
			}
			arcmsr_enable_outbound_ints(acb, intmask_org);
		}
		}
		break;

	case ACB_ADAPTER_TYPE_B: {
		unsigned long post_queue_phyaddr;
		uint32_t __iomem *rwbuffer;

		struct MessageUnit_B *reg = acb->pmuB;
		uint32_t intmask_org;
		intmask_org = arcmsr_disable_outbound_ints(acb);
		reg->postq_index = 0;
		reg->doneq_index = 0;
		writel(ARCMSR_MESSAGE_SET_POST_WINDOW, reg->drv2iop_doorbell_reg);
		if (arcmsr_hbb_wait_msgint_ready(acb)) {
			printk(KERN_NOTICE "arcmsr%d:can not set diver mode\n", \
				acb->host->host_no);
			return 1;
		}
		post_queue_phyaddr = cdb_phyaddr + ARCMSR_MAX_FREECCB_NUM * \
		sizeof(struct CommandControlBlock) + offsetof(struct MessageUnit_B, post_qbuffer) ;
		rwbuffer = reg->msgcode_rwbuffer_reg;
		/* driver "set config" signature */
		writel(ARCMSR_SIGNATURE_SET_CONFIG, rwbuffer++);
		/* normal should be zero */
		writel(ccb_phyaddr_hi32, rwbuffer++);
		/* postQ size (256 + 8)*4	 */
		writel(post_queue_phyaddr, rwbuffer++);
		/* doneQ size (256 + 8)*4	 */
		writel(post_queue_phyaddr + 1056, rwbuffer++);
		/* ccb maxQ size must be --> [(256 + 8)*4]*/
		writel(1056, rwbuffer);

		writel(ARCMSR_MESSAGE_SET_CONFIG, reg->drv2iop_doorbell_reg);
		if (arcmsr_hbb_wait_msgint_ready(acb)) {
			printk(KERN_NOTICE "arcmsr%d: 'set command Q window' \
			timeout \n",acb->host->host_no);
			return 1;
		}

		writel(ARCMSR_MESSAGE_START_DRIVER_MODE, reg->drv2iop_doorbell_reg);
		if (arcmsr_hbb_wait_msgint_ready(acb)) {
			printk(KERN_NOTICE "arcmsr%d: 'can not set diver mode \n"\
			,acb->host->host_no);
			return 1;
		}
		arcmsr_enable_outbound_ints(acb, intmask_org);
		}
		break;
	}
	return 0;
}

static void arcmsr_wait_firmware_ready(struct AdapterControlBlock *acb)
{
	uint32_t firmware_state = 0;

	switch (acb->adapter_type) {

	case ACB_ADAPTER_TYPE_A: {
		struct MessageUnit_A __iomem *reg = acb->pmuA;
		do {
			firmware_state = readl(&reg->outbound_msgaddr1);
		} while ((firmware_state & ARCMSR_OUTBOUND_MESG1_FIRMWARE_OK) == 0);
		}
		break;

	case ACB_ADAPTER_TYPE_B: {
		struct MessageUnit_B *reg = acb->pmuB;
		do {
			firmware_state = readl(reg->iop2drv_doorbell_reg);
		} while ((firmware_state & ARCMSR_MESSAGE_FIRMWARE_OK) == 0);
		writel(ARCMSR_DRV2IOP_END_OF_INTERRUPT, reg->drv2iop_doorbell_reg);
		}
		break;
	}
}

static void arcmsr_request_hba_device_map(struct AdapterControlBlock *acb)
{
	struct MessageUnit_A __iomem *reg = acb->pmuA;

	if (unlikely(atomic_read(&acb->rq_map_token) == 0)) {
		acb->fw_state = false;
	} else {
	/*to prevent rq_map_token from changing by other interrupt, then
	avoid the dead-lock*/
		acb->fw_state = true;
		atomic_dec(&acb->rq_map_token);
		if (!(acb->fw_state) ||
			(acb->ante_token_value == atomic_read(&acb->rq_map_token))) {
			atomic_set(&acb->rq_map_token, 16);
		}
		acb->ante_token_value = atomic_read(&acb->rq_map_token);
		writel(ARCMSR_INBOUND_MESG0_GET_CONFIG, &reg->inbound_msgaddr0);
	}
	mod_timer(&acb->eternal_timer, jiffies + msecs_to_jiffies(6000));
	return;
}

static void arcmsr_request_hbb_device_map(struct AdapterControlBlock *acb)
{
	struct MessageUnit_B __iomem *reg = acb->pmuB;

	if (unlikely(atomic_read(&acb->rq_map_token) == 0)) {
		acb->fw_state = false;
	} else {
	/*to prevent rq_map_token from changing by other interrupt, then
	avoid the dead-lock*/
		acb->fw_state = true;
		atomic_dec(&acb->rq_map_token);
		if (!(acb->fw_state) ||
			(acb->ante_token_value == atomic_read(&acb->rq_map_token))) {
			atomic_set(&acb->rq_map_token, 16);
		}
		acb->ante_token_value = atomic_read(&acb->rq_map_token);
		writel(ARCMSR_MESSAGE_GET_CONFIG, reg->drv2iop_doorbell_reg);
	}
	mod_timer(&acb->eternal_timer, jiffies + msecs_to_jiffies(6000));
	return;
}

static void arcmsr_request_device_map(unsigned long pacb)
{
	struct AdapterControlBlock *acb = (struct AdapterControlBlock *)pacb;

	switch (acb->adapter_type) {
		case ACB_ADAPTER_TYPE_A: {
			arcmsr_request_hba_device_map(acb);
		}
		break;
		case ACB_ADAPTER_TYPE_B: {
			arcmsr_request_hbb_device_map(acb);
		}
		break;
	}
}

static void arcmsr_start_hba_bgrb(struct AdapterControlBlock *acb)
{
	struct MessageUnit_A __iomem *reg = acb->pmuA;
	acb->acb_flags |= ACB_F_MSG_START_BGRB;
	writel(ARCMSR_INBOUND_MESG0_START_BGRB, &reg->inbound_msgaddr0);
	if (arcmsr_hba_wait_msgint_ready(acb)) {
		printk(KERN_NOTICE "arcmsr%d: wait 'start adapter background \
				rebulid' timeout \n", acb->host->host_no);
	}
}

static void arcmsr_start_hbb_bgrb(struct AdapterControlBlock *acb)
{
	struct MessageUnit_B *reg = acb->pmuB;
	acb->acb_flags |= ACB_F_MSG_START_BGRB;
	writel(ARCMSR_MESSAGE_START_BGRB, reg->drv2iop_doorbell_reg);
	if (arcmsr_hbb_wait_msgint_ready(acb)) {
		printk(KERN_NOTICE "arcmsr%d: wait 'start adapter background \
				rebulid' timeout \n",acb->host->host_no);
	}
}

static void arcmsr_start_adapter_bgrb(struct AdapterControlBlock *acb)
{
	switch (acb->adapter_type) {
	case ACB_ADAPTER_TYPE_A:
		arcmsr_start_hba_bgrb(acb);
		break;
	case ACB_ADAPTER_TYPE_B:
		arcmsr_start_hbb_bgrb(acb);
		break;
	}
}

static void arcmsr_clear_doorbell_queue_buffer(struct AdapterControlBlock *acb)
{
	switch (acb->adapter_type) {
	case ACB_ADAPTER_TYPE_A: {
		struct MessageUnit_A __iomem *reg = acb->pmuA;
		uint32_t outbound_doorbell;
		/* empty doorbell Qbuffer if door bell ringed */
		outbound_doorbell = readl(&reg->outbound_doorbell);
		/*clear doorbell interrupt */
		writel(outbound_doorbell, &reg->outbound_doorbell);
		writel(ARCMSR_INBOUND_DRIVER_DATA_READ_OK, &reg->inbound_doorbell);
		}
		break;

	case ACB_ADAPTER_TYPE_B: {
		struct MessageUnit_B *reg = acb->pmuB;
		/*clear interrupt and message state*/
		writel(ARCMSR_MESSAGE_INT_CLEAR_PATTERN, reg->iop2drv_doorbell_reg);
		writel(ARCMSR_DRV2IOP_DATA_READ_OK, reg->drv2iop_doorbell_reg);
		/* let IOP know data has been read */
		}
		break;
	}
}

static void arcmsr_enable_eoi_mode(struct AdapterControlBlock *acb)
{
	switch (acb->adapter_type) {
	case ACB_ADAPTER_TYPE_A:
		return;
	case ACB_ADAPTER_TYPE_B:
		{
			struct MessageUnit_B *reg = acb->pmuB;
			writel(ARCMSR_MESSAGE_ACTIVE_EOI_MODE, reg->drv2iop_doorbell_reg);
			if(arcmsr_hbb_wait_msgint_ready(acb)) {
				printk(KERN_NOTICE "ARCMSR IOP enables EOI_MODE TIMEOUT");
				return;
			}
		}
		break;
	}
	return;
}

static void arcmsr_hardware_reset(struct AdapterControlBlock *acb)
{
	uint8_t value[64];
	int i;

	/* backup pci config data */
	for (i = 0; i < 64; i++) {
		pci_read_config_byte(acb->pdev, i, &value[i]);
	}
	/* hardware reset signal */
	pci_write_config_byte(acb->pdev, 0x84, 0x20);
	msleep(1000);
	/* write back pci config data */
	for (i = 0; i < 64; i++) {
		pci_write_config_byte(acb->pdev, i, value[i]);
	}
	msleep(1000);
	return;
}
#ifdef CONFIG_SCSI_ARCMSR_RESET
	int arcmsr_sleep_for_bus_reset(struct scsi_cmnd *cmd)
	{
			struct Scsi_Host *shost = NULL;
			spinlock_t *host_lock = NULL;
			int i, isleep;

			shost = cmd->device->host;
			host_lock = shost->host_lock;

			printk(KERN_NOTICE "Host %d bus reset over, sleep %d seconds (busy %d, can queue %d) ...........\n",
					shost->host_no, sleeptime, shost->host_busy, shost->can_queue);
			isleep = sleeptime / 10;
			spin_unlock_irq(host_lock);
			if (isleep > 0) {
				for (i = 0; i < isleep; i++) {
					msleep(10000);
					printk(KERN_NOTICE "^%d^\n", i);
				}
			}

			isleep = sleeptime % 10;
			if (isleep > 0) {
				msleep(isleep * 1000);
				printk(KERN_NOTICE "^v^\n");
			}
			spin_lock_irq(host_lock);
			printk(KERN_NOTICE "***** wake up *****\n");
			return 0;
	}
#endif
static void arcmsr_iop_init(struct AdapterControlBlock *acb)
{
	uint32_t intmask_org;

       /* disable all outbound interrupt */
       intmask_org = arcmsr_disable_outbound_ints(acb);
	arcmsr_wait_firmware_ready(acb);
	arcmsr_iop_confirm(acb);
	arcmsr_get_firmware_spec(acb, 1);
	/*start background rebuild*/
	arcmsr_start_adapter_bgrb(acb);
	/* empty doorbell Qbuffer if door bell ringed */
	arcmsr_clear_doorbell_queue_buffer(acb);
	arcmsr_enable_eoi_mode(acb);
	/* enable outbound Post Queue,outbound doorbell Interrupt */
	arcmsr_enable_outbound_ints(acb, intmask_org);
	acb->acb_flags |= ACB_F_IOP_INITED;
}

static uint8_t arcmsr_iop_reset(struct AdapterControlBlock *acb)
{
	struct CommandControlBlock *ccb;
	uint32_t intmask_org;
	uint8_t rtnval = 0x00;
	int i = 0;

	if (atomic_read(&acb->ccboutstandingcount) != 0) {
		/* disable all outbound interrupt */
		intmask_org = arcmsr_disable_outbound_ints(acb);
		/* talk to iop 331 outstanding command aborted */
		rtnval = arcmsr_abort_allcmd(acb);
		/* wait for 3 sec for all command aborted*/
		ssleep(3);
		/* clear all outbound posted Q */
		arcmsr_done4abort_postqueue(acb);
		for (i = 0; i < ARCMSR_MAX_FREECCB_NUM; i++) {
			ccb = acb->pccb_pool[i];
			if (ccb->startdone == ARCMSR_CCB_START) {
				arcmsr_ccb_complete(ccb, 1);
			}
		}
		atomic_set(&acb->ccboutstandingcount, 0);
		/* enable all outbound interrupt */
		arcmsr_enable_outbound_ints(acb, intmask_org);
		return rtnval;
	}
	return rtnval;
}

static int arcmsr_bus_reset(struct scsi_cmnd *cmd)
{
	struct AdapterControlBlock *acb =
		(struct AdapterControlBlock *)cmd->device->host->hostdata;
	int retry = 0;

	if (acb->acb_flags & ACB_F_BUS_RESET)
		return SUCCESS;

	printk(KERN_NOTICE "arcmsr%d: bus reset ..... \n", acb->adapter_index);
	acb->acb_flags |= ACB_F_BUS_RESET;
	acb->num_resets++;
	while (atomic_read(&acb->ccboutstandingcount) != 0 && retry < 4) {
		arcmsr_interrupt(acb);
		retry++;
	}

	if (arcmsr_iop_reset(acb)) {
		switch (acb->adapter_type) {
		case ACB_ADAPTER_TYPE_A: {
			printk(KERN_NOTICE "arcmsr%d: do hardware bus reset, num_resets = %d num_aborts = %d \n",
				acb->adapter_index, acb->num_resets, acb->num_aborts);
			arcmsr_hardware_reset(acb);
			acb->acb_flags |= ACB_F_FIRMWARE_TRAP;
			acb->acb_flags &= ~ACB_F_IOP_INITED;
			#ifdef CONFIG_SCSI_ARCMSR_RESET
			struct MessageUnit_A __iomem *reg = acb->pmuA;
			uint32_t intmask_org, outbound_doorbell;
			int retry_count = 0;
sleep_again:
			arcmsr_sleep_for_bus_reset(cmd);
			if ((readl(&reg->outbound_msgaddr1) &
			ARCMSR_OUTBOUND_MESG1_FIRMWARE_OK) == 0) {
			printk(KERN_NOTICE "arcmsr%d: hardware bus reset and return busy, retry=%d \n",
			acb->host->host_no, retry_count);
			if (retry_count > retrycount) {
				printk(KERN_NOTICE "arcmsr%d: hardware bus reset and return busy, retry aborted \n",
				acb->host->host_no);
			return SUCCESS;
			}
			retry_count++;
			goto sleep_again;
			}
			acb->acb_flags &= ~ACB_F_FIRMWARE_TRAP;
			acb->acb_flags |= ACB_F_IOP_INITED;
			acb->acb_flags &= ~ACB_F_BUS_RESET;
			printk(KERN_NOTICE "arcmsr%d: hardware bus reset and reset ok \n",
				acb->host->host_no);
			/* disable all outbound interrupt */
			intmask_org = arcmsr_disable_outbound_ints(acb);
			arcmsr_get_firmware_spec(acb, 1);
			/*start	background rebuild*/
			arcmsr_start_adapter_bgrb(acb);
			/* clear Qbuffer if door bell ringed */
			outbound_doorbell = readl(&reg->outbound_doorbell);
			writel(outbound_doorbell, &reg->outbound_doorbell); /*clear interrupt */
			writel(ARCMSR_INBOUND_DRIVER_DATA_READ_OK, &reg->inbound_doorbell);
			/* enable outbound Post Queue,outbound doorbell Interrupt */
			arcmsr_enable_outbound_ints(acb, intmask_org);
			atomic_set(&acb->rq_map_token, 16);
			init_timer(&acb->eternal_timer);
			acb->eternal_timer.expires = jiffies + msecs_to_jiffies(20*HZ);
			acb->eternal_timer.data = (unsigned long) acb;
			acb->eternal_timer.function = &arcmsr_request_device_map;
			add_timer(&acb->eternal_timer);
			#endif
		}
			break;
		case ACB_ADAPTER_TYPE_B: {
	}
		}
	} else {
	acb->acb_flags &= ~ACB_F_BUS_RESET;
	}
	return SUCCESS;
}

static void arcmsr_abort_one_cmd(struct AdapterControlBlock *acb,
		struct CommandControlBlock *ccb)
{
	u32 intmask;

	ccb->startdone = ARCMSR_CCB_ABORTED;

	/*
	** Wait for 3 sec for all command done.
	*/
	ssleep(3);

	intmask = arcmsr_disable_outbound_ints(acb);
	arcmsr_polling_ccbdone(acb, ccb);
	arcmsr_enable_outbound_ints(acb, intmask);
}

static int arcmsr_abort(struct scsi_cmnd *cmd)
{
	struct AdapterControlBlock *acb =
		(struct AdapterControlBlock *)cmd->device->host->hostdata;
	int i = 0;

	printk(KERN_NOTICE
		"arcmsr%d: abort device command of scsi id = %d lun = %d \n",
		acb->host->host_no, cmd->device->id, cmd->device->lun);
	acb->num_aborts++;
	/*
	************************************************
	** the all interrupt service routine is locked
	** we need to handle it as soon as possible and exit
	************************************************
	*/
	if (!atomic_read(&acb->ccboutstandingcount))
		return SUCCESS;

	for (i = 0; i < ARCMSR_MAX_FREECCB_NUM; i++) {
		struct CommandControlBlock *ccb = acb->pccb_pool[i];
		if (ccb->startdone == ARCMSR_CCB_START && ccb->pcmd == cmd) {
			arcmsr_abort_one_cmd(acb, ccb);
			break;
		}
	}

	return SUCCESS;
}

static const char *arcmsr_info(struct Scsi_Host *host)
{
	struct AdapterControlBlock *acb =
		(struct AdapterControlBlock *) host->hostdata;
	static char buf[256];
	char *type;
	int raid6 = 1;

	switch (acb->pdev->device) {
	case PCI_DEVICE_ID_ARECA_1110:
	case PCI_DEVICE_ID_ARECA_1200:
	case PCI_DEVICE_ID_ARECA_1202:
	case PCI_DEVICE_ID_ARECA_1210:
		raid6 = 0;
		/*FALLTHRU*/
	case PCI_DEVICE_ID_ARECA_1120:
	case PCI_DEVICE_ID_ARECA_1130:
	case PCI_DEVICE_ID_ARECA_1160:
	case PCI_DEVICE_ID_ARECA_1170:
	case PCI_DEVICE_ID_ARECA_1201:
	case PCI_DEVICE_ID_ARECA_1220:
	case PCI_DEVICE_ID_ARECA_1230:
	case PCI_DEVICE_ID_ARECA_1260:
	case PCI_DEVICE_ID_ARECA_1270:
	case PCI_DEVICE_ID_ARECA_1280:
		type = "SATA";
		break;
	case PCI_DEVICE_ID_ARECA_1380:
	case PCI_DEVICE_ID_ARECA_1381:
	case PCI_DEVICE_ID_ARECA_1680:
	case PCI_DEVICE_ID_ARECA_1681:
		type = "SAS";
		break;
	default:
		type = "X-TYPE";
		break;
	}
	sprintf(buf, "Areca %s Host Adapter RAID Controller%s\n %s",
			type, raid6 ? "( RAID6 capable)" : "",
			ARCMSR_DRIVER_VERSION);
	return buf;
}
