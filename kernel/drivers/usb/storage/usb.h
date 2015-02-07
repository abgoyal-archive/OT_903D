

#ifndef _USB_H_
#define _USB_H_

#include <linux/usb.h>
#include <linux/usb_usual.h>
#include <linux/blkdev.h>
#include <linux/completion.h>
#include <linux/mutex.h>
#include <scsi/scsi_host.h>

struct us_data;
struct scsi_cmnd;


struct us_unusual_dev {
	const char* vendorName;
	const char* productName;
	__u8  useProtocol;
	__u8  useTransport;
	int (*initFunction)(struct us_data *);
};


/* Dynamic bitflag definitions (us->dflags): used in set_bit() etc. */
#define US_FLIDX_URB_ACTIVE	0	/* current_urb is in use    */
#define US_FLIDX_SG_ACTIVE	1	/* current_sg is in use     */
#define US_FLIDX_ABORTING	2	/* abort is in progress     */
#define US_FLIDX_DISCONNECTING	3	/* disconnect in progress   */
#define US_FLIDX_RESETTING	4	/* device reset in progress */
#define US_FLIDX_TIMED_OUT	5	/* SCSI midlayer timed out  */
#define US_FLIDX_DONT_SCAN	6	/* don't scan (disconnect)  */

#define USB_STOR_STRING_LEN 32


#define US_IOBUF_SIZE		64	/* Size of the DMA-mapped I/O buffer */
#define US_SENSE_SIZE		18	/* Size of the autosense data buffer */

typedef int (*trans_cmnd)(struct scsi_cmnd *, struct us_data*);
typedef int (*trans_reset)(struct us_data*);
typedef void (*proto_cmnd)(struct scsi_cmnd*, struct us_data*);
typedef void (*extra_data_destructor)(void *);	/* extra data destructor */
typedef void (*pm_hook)(struct us_data *, int);	/* power management hook */

#define US_SUSPEND	0
#define US_RESUME	1

/* we allocate one of these for every device that we remember */
struct us_data {
	/* The device we're working with
	 * It's important to note:
	 *    (o) you must hold dev_mutex to change pusb_dev
	 */
	struct mutex		dev_mutex;	 /* protect pusb_dev */
	struct usb_device	*pusb_dev;	 /* this usb_device */
	struct usb_interface	*pusb_intf;	 /* this interface */
	struct us_unusual_dev   *unusual_dev;	 /* device-filter entry     */
	unsigned long		fflags;		 /* fixed flags from filter */
	unsigned long		dflags;		 /* dynamic atomic bitflags */
	unsigned int		send_bulk_pipe;	 /* cached pipe values */
	unsigned int		recv_bulk_pipe;
	unsigned int		send_ctrl_pipe;
	unsigned int		recv_ctrl_pipe;
	unsigned int		recv_intr_pipe;

	/* information about the device */
	char			*transport_name;
	char			*protocol_name;
	__le32			bcs_signature;
	u8			subclass;
	u8			protocol;
	u8			max_lun;

	u8			ifnum;		 /* interface number   */
	u8			ep_bInterval;	 /* interrupt interval */ 

	/* function pointers for this device */
	trans_cmnd		transport;	 /* transport function	   */
	trans_reset		transport_reset; /* transport device reset */
	proto_cmnd		proto_handler;	 /* protocol handler	   */

	/* SCSI interfaces */
	struct scsi_cmnd	*srb;		 /* current srb		*/
	unsigned int		tag;		 /* current dCBWTag	*/
	char			scsi_name[32];	 /* scsi_host name	*/

	/* control and bulk communications data */
	struct urb		*current_urb;	 /* USB requests	 */
	struct usb_ctrlrequest	*cr;		 /* control requests	 */
	struct usb_sg_request	current_sg;	 /* scatter-gather req.  */
	unsigned char		*iobuf;		 /* I/O buffer		 */
	dma_addr_t		iobuf_dma;	 /* buffer DMA addresses */
	struct task_struct	*ctl_thread;	 /* the control thread   */

	/* mutual exclusion and synchronization structures */
	struct completion	cmnd_ready;	 /* to sleep thread on	    */
	struct completion	notify;		 /* thread begin/end	    */
	wait_queue_head_t	delay_wait;	 /* wait during scan, reset */
	struct completion	scanning_done;	 /* wait for scan thread    */

	/* subdriver information */
	void			*extra;		 /* Any extra data          */
	extra_data_destructor	extra_destructor;/* extra data destructor   */
#ifdef CONFIG_PM
	pm_hook			suspend_resume_hook;
#endif

	/* hacks for READ CAPACITY bug handling */
	int			use_last_sector_hacks;
	int			last_sector_retries;
};

/* Convert between us_data and the corresponding Scsi_Host */
static inline struct Scsi_Host *us_to_host(struct us_data *us) {
	return container_of((void *) us, struct Scsi_Host, hostdata);
}
static inline struct us_data *host_to_us(struct Scsi_Host *host) {
	return (struct us_data *) host->hostdata;
}

/* Function to fill an inquiry response. See usb.c for details */
extern void fill_inquiry_response(struct us_data *us,
	unsigned char *data, unsigned int data_len);

#define scsi_unlock(host)	spin_unlock_irq(host->host_lock)
#define scsi_lock(host)		spin_lock_irq(host->host_lock)

/* General routines provided by the usb-storage standard core */
#ifdef CONFIG_PM
extern int usb_stor_suspend(struct usb_interface *iface, pm_message_t message);
extern int usb_stor_resume(struct usb_interface *iface);
extern int usb_stor_reset_resume(struct usb_interface *iface);
#else
#define usb_stor_suspend	NULL
#define usb_stor_resume		NULL
#define usb_stor_reset_resume	NULL
#endif

extern int usb_stor_pre_reset(struct usb_interface *iface);
extern int usb_stor_post_reset(struct usb_interface *iface);

extern int usb_stor_probe1(struct us_data **pus,
		struct usb_interface *intf,
		const struct usb_device_id *id,
		struct us_unusual_dev *unusual_dev);
extern int usb_stor_probe2(struct us_data *us);
extern void usb_stor_disconnect(struct usb_interface *intf);

#endif
