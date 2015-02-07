

#ifndef __BFA_DEFS_VF_H__
#define __BFA_DEFS_VF_H__

#include <bfa_os_inc.h>
#include <defs/bfa_defs_port.h>
#include <protocol/types.h>

enum bfa_vf_state {
	BFA_VF_UNINIT    = 0,	/*  fabric is not yet initialized */
	BFA_VF_LINK_DOWN = 1,	/*  link is down */
	BFA_VF_FLOGI     = 2,	/*  flogi is in progress */
	BFA_VF_AUTH      = 3,	/*  authentication in progress */
	BFA_VF_NOFABRIC  = 4,	/*  fabric is not present */
	BFA_VF_ONLINE    = 5,	/*  login to fabric is complete */
	BFA_VF_EVFP      = 6,	/*  EVFP is in progress */
	BFA_VF_ISOLATED  = 7,	/*  port isolated due to vf_id mismatch */
};

struct bfa_vf_stats_s {
	u32        flogi_sent;	/*  Num FLOGIs sent 		*/
	u32        flogi_rsp_err;	/*  FLOGI response errors 	*/
	u32        flogi_acc_err;	/*  FLOGI accept errors 	*/
	u32        flogi_accepts;	/*  FLOGI accepts received 	*/
	u32        flogi_rejects;	/*  FLOGI rejects received 	*/
	u32        flogi_unknown_rsp; /*  Unknown responses for FLOGI */
	u32        flogi_alloc_wait; /*  Allocation waits prior to
					   * sending FLOGI
					   */
	u32        flogi_rcvd;	/*  FLOGIs received */
	u32        flogi_rejected;	/*  Incoming FLOGIs rejected */
	u32        fabric_onlines;	/*  Internal fabric online
					 * notification sent to other
					 * modules
					 */
	u32        fabric_offlines; /*  Internal fabric offline
					  * notification sent to other
					  * modules
					  */
	u32        resvd;
};

struct bfa_vf_attr_s {
	enum bfa_vf_state  state;		/*  VF state */
	u32        rsvd;
	wwn_t           fabric_name;	/*  fabric name */
};

#endif /* __BFA_DEFS_VF_H__ */
