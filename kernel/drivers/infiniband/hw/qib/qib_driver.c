

#include <linux/spinlock.h>
#include <linux/pci.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/netdevice.h>
#include <linux/vmalloc.h>

#include "qib.h"

const char ib_qib_version[] = QIB_IDSTR "\n";

DEFINE_SPINLOCK(qib_devs_lock);
LIST_HEAD(qib_dev_list);
DEFINE_MUTEX(qib_mutex);	/* general driver use */

unsigned qib_ibmtu;
module_param_named(ibmtu, qib_ibmtu, uint, S_IRUGO);
MODULE_PARM_DESC(ibmtu, "Set max IB MTU (0=2KB, 1=256, 2=512, ... 5=4096");

unsigned qib_compat_ddr_negotiate = 1;
module_param_named(compat_ddr_negotiate, qib_compat_ddr_negotiate, uint,
		   S_IWUSR | S_IRUGO);
MODULE_PARM_DESC(compat_ddr_negotiate,
		 "Attempt pre-IBTA 1.2 DDR speed negotiation");

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("QLogic <support@qlogic.com>");
MODULE_DESCRIPTION("QLogic IB driver");

#define QIB_PIO_MAXIBHDR 128

struct qlogic_ib_stats qib_stats;

const char *qib_get_unit_name(int unit)
{
	static char iname[16];

	snprintf(iname, sizeof iname, "infinipath%u", unit);
	return iname;
}

int qib_count_active_units(void)
{
	struct qib_devdata *dd;
	struct qib_pportdata *ppd;
	unsigned long flags;
	int pidx, nunits_active = 0;

	spin_lock_irqsave(&qib_devs_lock, flags);
	list_for_each_entry(dd, &qib_dev_list, list) {
		if (!(dd->flags & QIB_PRESENT) || !dd->kregbase)
			continue;
		for (pidx = 0; pidx < dd->num_pports; ++pidx) {
			ppd = dd->pport + pidx;
			if (ppd->lid && (ppd->lflags & (QIBL_LINKINIT |
					 QIBL_LINKARMED | QIBL_LINKACTIVE))) {
				nunits_active++;
				break;
			}
		}
	}
	spin_unlock_irqrestore(&qib_devs_lock, flags);
	return nunits_active;
}

int qib_count_units(int *npresentp, int *nupp)
{
	int nunits = 0, npresent = 0, nup = 0;
	struct qib_devdata *dd;
	unsigned long flags;
	int pidx;
	struct qib_pportdata *ppd;

	spin_lock_irqsave(&qib_devs_lock, flags);

	list_for_each_entry(dd, &qib_dev_list, list) {
		nunits++;
		if ((dd->flags & QIB_PRESENT) && dd->kregbase)
			npresent++;
		for (pidx = 0; pidx < dd->num_pports; ++pidx) {
			ppd = dd->pport + pidx;
			if (ppd->lid && (ppd->lflags & (QIBL_LINKINIT |
					 QIBL_LINKARMED | QIBL_LINKACTIVE)))
				nup++;
		}
	}

	spin_unlock_irqrestore(&qib_devs_lock, flags);

	if (npresentp)
		*npresentp = npresent;
	if (nupp)
		*nupp = nup;

	return nunits;
}

int qib_wait_linkstate(struct qib_pportdata *ppd, u32 state, int msecs)
{
	int ret;
	unsigned long flags;

	spin_lock_irqsave(&ppd->lflags_lock, flags);
	if (ppd->state_wanted) {
		spin_unlock_irqrestore(&ppd->lflags_lock, flags);
		ret = -EBUSY;
		goto bail;
	}
	ppd->state_wanted = state;
	spin_unlock_irqrestore(&ppd->lflags_lock, flags);
	wait_event_interruptible_timeout(ppd->state_wait,
					 (ppd->lflags & state),
					 msecs_to_jiffies(msecs));
	spin_lock_irqsave(&ppd->lflags_lock, flags);
	ppd->state_wanted = 0;
	spin_unlock_irqrestore(&ppd->lflags_lock, flags);

	if (!(ppd->lflags & state))
		ret = -ETIMEDOUT;
	else
		ret = 0;
bail:
	return ret;
}

int qib_set_linkstate(struct qib_pportdata *ppd, u8 newstate)
{
	u32 lstate;
	int ret;
	struct qib_devdata *dd = ppd->dd;
	unsigned long flags;

	switch (newstate) {
	case QIB_IB_LINKDOWN_ONLY:
		dd->f_set_ib_cfg(ppd, QIB_IB_CFG_LSTATE,
				 IB_LINKCMD_DOWN | IB_LINKINITCMD_NOP);
		/* don't wait */
		ret = 0;
		goto bail;

	case QIB_IB_LINKDOWN:
		dd->f_set_ib_cfg(ppd, QIB_IB_CFG_LSTATE,
				 IB_LINKCMD_DOWN | IB_LINKINITCMD_POLL);
		/* don't wait */
		ret = 0;
		goto bail;

	case QIB_IB_LINKDOWN_SLEEP:
		dd->f_set_ib_cfg(ppd, QIB_IB_CFG_LSTATE,
				 IB_LINKCMD_DOWN | IB_LINKINITCMD_SLEEP);
		/* don't wait */
		ret = 0;
		goto bail;

	case QIB_IB_LINKDOWN_DISABLE:
		dd->f_set_ib_cfg(ppd, QIB_IB_CFG_LSTATE,
				 IB_LINKCMD_DOWN | IB_LINKINITCMD_DISABLE);
		/* don't wait */
		ret = 0;
		goto bail;

	case QIB_IB_LINKARM:
		if (ppd->lflags & QIBL_LINKARMED) {
			ret = 0;
			goto bail;
		}
		if (!(ppd->lflags & (QIBL_LINKINIT | QIBL_LINKACTIVE))) {
			ret = -EINVAL;
			goto bail;
		}
		/*
		 * Since the port can be ACTIVE when we ask for ARMED,
		 * clear QIBL_LINKV so we can wait for a transition.
		 * If the link isn't ARMED, then something else happened
		 * and there is no point waiting for ARMED.
		 */
		spin_lock_irqsave(&ppd->lflags_lock, flags);
		ppd->lflags &= ~QIBL_LINKV;
		spin_unlock_irqrestore(&ppd->lflags_lock, flags);
		dd->f_set_ib_cfg(ppd, QIB_IB_CFG_LSTATE,
				 IB_LINKCMD_ARMED | IB_LINKINITCMD_NOP);
		lstate = QIBL_LINKV;
		break;

	case QIB_IB_LINKACTIVE:
		if (ppd->lflags & QIBL_LINKACTIVE) {
			ret = 0;
			goto bail;
		}
		if (!(ppd->lflags & QIBL_LINKARMED)) {
			ret = -EINVAL;
			goto bail;
		}
		dd->f_set_ib_cfg(ppd, QIB_IB_CFG_LSTATE,
				 IB_LINKCMD_ACTIVE | IB_LINKINITCMD_NOP);
		lstate = QIBL_LINKACTIVE;
		break;

	default:
		ret = -EINVAL;
		goto bail;
	}
	ret = qib_wait_linkstate(ppd, lstate, 10);

bail:
	return ret;
}

static inline void *qib_get_egrbuf(const struct qib_ctxtdata *rcd, u32 etail)
{
	const u32 chunk = etail / rcd->rcvegrbufs_perchunk;
	const u32 idx =  etail % rcd->rcvegrbufs_perchunk;

	return rcd->rcvegrbuf[chunk] + idx * rcd->dd->rcvegrbufsize;
}

static u32 qib_rcv_hdrerr(struct qib_pportdata *ppd, u32 ctxt,
			  u32 eflags, u32 l, u32 etail, __le32 *rhf_addr,
			  struct qib_message_header *hdr)
{
	u32 ret = 0;

	if (eflags & (QLOGIC_IB_RHF_H_ICRCERR | QLOGIC_IB_RHF_H_VCRCERR))
		ret = 1;
	return ret;
}

u32 qib_kreceive(struct qib_ctxtdata *rcd, u32 *llic, u32 *npkts)
{
	struct qib_devdata *dd = rcd->dd;
	struct qib_pportdata *ppd = rcd->ppd;
	__le32 *rhf_addr;
	void *ebuf;
	const u32 rsize = dd->rcvhdrentsize;        /* words */
	const u32 maxcnt = dd->rcvhdrcnt * rsize;   /* words */
	u32 etail = -1, l, hdrqtail;
	struct qib_message_header *hdr;
	u32 eflags, etype, tlen, i = 0, updegr = 0, crcs = 0;
	int last;
	u64 lval;
	struct qib_qp *qp, *nqp;

	l = rcd->head;
	rhf_addr = (__le32 *) rcd->rcvhdrq + l + dd->rhf_offset;
	if (dd->flags & QIB_NODMA_RTAIL) {
		u32 seq = qib_hdrget_seq(rhf_addr);
		if (seq != rcd->seq_cnt)
			goto bail;
		hdrqtail = 0;
	} else {
		hdrqtail = qib_get_rcvhdrtail(rcd);
		if (l == hdrqtail)
			goto bail;
		smp_rmb();  /* prevent speculative reads of dma'ed hdrq */
	}

	for (last = 0, i = 1; !last; i += !last) {
		hdr = dd->f_get_msgheader(dd, rhf_addr);
		eflags = qib_hdrget_err_flags(rhf_addr);
		etype = qib_hdrget_rcv_type(rhf_addr);
		/* total length */
		tlen = qib_hdrget_length_in_bytes(rhf_addr);
		ebuf = NULL;
		if ((dd->flags & QIB_NODMA_RTAIL) ?
		    qib_hdrget_use_egr_buf(rhf_addr) :
		    (etype != RCVHQ_RCV_TYPE_EXPECTED)) {
			etail = qib_hdrget_index(rhf_addr);
			updegr = 1;
			if (tlen > sizeof(*hdr) ||
			    etype >= RCVHQ_RCV_TYPE_NON_KD)
				ebuf = qib_get_egrbuf(rcd, etail);
		}
		if (!eflags) {
			u16 lrh_len = be16_to_cpu(hdr->lrh[2]) << 2;

			if (lrh_len != tlen) {
				qib_stats.sps_lenerrs++;
				goto move_along;
			}
		}
		if (etype == RCVHQ_RCV_TYPE_NON_KD && !eflags &&
		    ebuf == NULL &&
		    tlen > (dd->rcvhdrentsize - 2 + 1 -
				qib_hdrget_offset(rhf_addr)) << 2) {
			goto move_along;
		}

		/*
		 * Both tiderr and qibhdrerr are set for all plain IB
		 * packets; only qibhdrerr should be set.
		 */
		if (unlikely(eflags))
			crcs += qib_rcv_hdrerr(ppd, rcd->ctxt, eflags, l,
					       etail, rhf_addr, hdr);
		else if (etype == RCVHQ_RCV_TYPE_NON_KD) {
			qib_ib_rcv(rcd, hdr, ebuf, tlen);
			if (crcs)
				crcs--;
			else if (llic && *llic)
				--*llic;
		}
move_along:
		l += rsize;
		if (l >= maxcnt)
			l = 0;
		rhf_addr = (__le32 *) rcd->rcvhdrq + l + dd->rhf_offset;
		if (dd->flags & QIB_NODMA_RTAIL) {
			u32 seq = qib_hdrget_seq(rhf_addr);

			if (++rcd->seq_cnt > 13)
				rcd->seq_cnt = 1;
			if (seq != rcd->seq_cnt)
				last = 1;
		} else if (l == hdrqtail)
			last = 1;
		/*
		 * Update head regs etc., every 16 packets, if not last pkt,
		 * to help prevent rcvhdrq overflows, when many packets
		 * are processed and queue is nearly full.
		 * Don't request an interrupt for intermediate updates.
		 */
		lval = l;
		if (!last && !(i & 0xf)) {
			dd->f_update_usrhead(rcd, lval, updegr, etail);
			updegr = 0;
		}
	}

	rcd->head = l;
	rcd->pkt_count += i;

	/*
	 * Iterate over all QPs waiting to respond.
	 * The list won't change since the IRQ is only run on one CPU.
	 */
	list_for_each_entry_safe(qp, nqp, &rcd->qp_wait_list, rspwait) {
		list_del_init(&qp->rspwait);
		if (qp->r_flags & QIB_R_RSP_NAK) {
			qp->r_flags &= ~QIB_R_RSP_NAK;
			qib_send_rc_ack(qp);
		}
		if (qp->r_flags & QIB_R_RSP_SEND) {
			unsigned long flags;

			qp->r_flags &= ~QIB_R_RSP_SEND;
			spin_lock_irqsave(&qp->s_lock, flags);
			if (ib_qib_state_ops[qp->state] &
					QIB_PROCESS_OR_FLUSH_SEND)
				qib_schedule_send(qp);
			spin_unlock_irqrestore(&qp->s_lock, flags);
		}
		if (atomic_dec_and_test(&qp->refcount))
			wake_up(&qp->wait);
	}

bail:
	/* Report number of packets consumed */
	if (npkts)
		*npkts = i;

	/*
	 * Always write head at end, and setup rcv interrupt, even
	 * if no packets were processed.
	 */
	lval = (u64)rcd->head | dd->rhdrhead_intr_off;
	dd->f_update_usrhead(rcd, lval, updegr, etail);
	return crcs;
}

int qib_set_mtu(struct qib_pportdata *ppd, u16 arg)
{
	u32 piosize;
	int ret, chk;

	if (arg != 256 && arg != 512 && arg != 1024 && arg != 2048 &&
	    arg != 4096) {
		ret = -EINVAL;
		goto bail;
	}
	chk = ib_mtu_enum_to_int(qib_ibmtu);
	if (chk > 0 && arg > chk) {
		ret = -EINVAL;
		goto bail;
	}

	piosize = ppd->ibmaxlen;
	ppd->ibmtu = arg;

	if (arg >= (piosize - QIB_PIO_MAXIBHDR)) {
		/* Only if it's not the initial value (or reset to it) */
		if (piosize != ppd->init_ibmaxlen) {
			if (arg > piosize && arg <= ppd->init_ibmaxlen)
				piosize = ppd->init_ibmaxlen - 2 * sizeof(u32);
			ppd->ibmaxlen = piosize;
		}
	} else if ((arg + QIB_PIO_MAXIBHDR) != ppd->ibmaxlen) {
		piosize = arg + QIB_PIO_MAXIBHDR - 2 * sizeof(u32);
		ppd->ibmaxlen = piosize;
	}

	ppd->dd->f_set_ib_cfg(ppd, QIB_IB_CFG_MTU, 0);

	ret = 0;

bail:
	return ret;
}

int qib_set_lid(struct qib_pportdata *ppd, u32 lid, u8 lmc)
{
	struct qib_devdata *dd = ppd->dd;
	ppd->lid = lid;
	ppd->lmc = lmc;

	dd->f_set_ib_cfg(ppd, QIB_IB_CFG_LIDLMC,
			 lid | (~((1U << lmc) - 1)) << 16);

	qib_devinfo(dd->pcidev, "IB%u:%u got a lid: 0x%x\n",
		    dd->unit, ppd->port, lid);

	return 0;
}

#define LED_OVER_FREQ_SHIFT 8
#define LED_OVER_FREQ_MASK (0xFF<<LED_OVER_FREQ_SHIFT)
/* Below is "non-zero" to force override, but both actual LEDs are off */
#define LED_OVER_BOTH_OFF (8)

static void qib_run_led_override(unsigned long opaque)
{
	struct qib_pportdata *ppd = (struct qib_pportdata *)opaque;
	struct qib_devdata *dd = ppd->dd;
	int timeoff;
	int ph_idx;

	if (!(dd->flags & QIB_INITTED))
		return;

	ph_idx = ppd->led_override_phase++ & 1;
	ppd->led_override = ppd->led_override_vals[ph_idx];
	timeoff = ppd->led_override_timeoff;

	dd->f_setextled(ppd, 1);
	/*
	 * don't re-fire the timer if user asked for it to be off; we let
	 * it fire one more time after they turn it off to simplify
	 */
	if (ppd->led_override_vals[0] || ppd->led_override_vals[1])
		mod_timer(&ppd->led_override_timer, jiffies + timeoff);
}

void qib_set_led_override(struct qib_pportdata *ppd, unsigned int val)
{
	struct qib_devdata *dd = ppd->dd;
	int timeoff, freq;

	if (!(dd->flags & QIB_INITTED))
		return;

	/* First check if we are blinking. If not, use 1HZ polling */
	timeoff = HZ;
	freq = (val & LED_OVER_FREQ_MASK) >> LED_OVER_FREQ_SHIFT;

	if (freq) {
		/* For blink, set each phase from one nybble of val */
		ppd->led_override_vals[0] = val & 0xF;
		ppd->led_override_vals[1] = (val >> 4) & 0xF;
		timeoff = (HZ << 4)/freq;
	} else {
		/* Non-blink set both phases the same. */
		ppd->led_override_vals[0] = val & 0xF;
		ppd->led_override_vals[1] = val & 0xF;
	}
	ppd->led_override_timeoff = timeoff;

	/*
	 * If the timer has not already been started, do so. Use a "quick"
	 * timeout so the function will be called soon, to look at our request.
	 */
	if (atomic_inc_return(&ppd->led_override_timer_active) == 1) {
		/* Need to start timer */
		init_timer(&ppd->led_override_timer);
		ppd->led_override_timer.function = qib_run_led_override;
		ppd->led_override_timer.data = (unsigned long) ppd;
		ppd->led_override_timer.expires = jiffies + 1;
		add_timer(&ppd->led_override_timer);
	} else {
		if (ppd->led_override_vals[0] || ppd->led_override_vals[1])
			mod_timer(&ppd->led_override_timer, jiffies + 1);
		atomic_dec(&ppd->led_override_timer_active);
	}
}

int qib_reset_device(int unit)
{
	int ret, i;
	struct qib_devdata *dd = qib_lookup(unit);
	struct qib_pportdata *ppd;
	unsigned long flags;
	int pidx;

	if (!dd) {
		ret = -ENODEV;
		goto bail;
	}

	qib_devinfo(dd->pcidev, "Reset on unit %u requested\n", unit);

	if (!dd->kregbase || !(dd->flags & QIB_PRESENT)) {
		qib_devinfo(dd->pcidev, "Invalid unit number %u or "
			    "not initialized or not present\n", unit);
		ret = -ENXIO;
		goto bail;
	}

	spin_lock_irqsave(&dd->uctxt_lock, flags);
	if (dd->rcd)
		for (i = dd->first_user_ctxt; i < dd->cfgctxts; i++) {
			if (!dd->rcd[i] || !dd->rcd[i]->cnt)
				continue;
			spin_unlock_irqrestore(&dd->uctxt_lock, flags);
			ret = -EBUSY;
			goto bail;
		}
	spin_unlock_irqrestore(&dd->uctxt_lock, flags);

	for (pidx = 0; pidx < dd->num_pports; ++pidx) {
		ppd = dd->pport + pidx;
		if (atomic_read(&ppd->led_override_timer_active)) {
			/* Need to stop LED timer, _then_ shut off LEDs */
			del_timer_sync(&ppd->led_override_timer);
			atomic_set(&ppd->led_override_timer_active, 0);
		}

		/* Shut off LEDs after we are sure timer is not running */
		ppd->led_override = LED_OVER_BOTH_OFF;
		dd->f_setextled(ppd, 0);
		if (dd->flags & QIB_HAS_SEND_DMA)
			qib_teardown_sdma(ppd);
	}

	ret = dd->f_reset(dd);
	if (ret == 1)
		ret = qib_init(dd, 1);
	else
		ret = -EAGAIN;
	if (ret)
		qib_dev_err(dd, "Reinitialize unit %u after "
			    "reset failed with %d\n", unit, ret);
	else
		qib_devinfo(dd->pcidev, "Reinitialized unit %u after "
			    "resetting\n", unit);

bail:
	return ret;
}