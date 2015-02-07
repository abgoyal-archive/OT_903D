#ifndef _XT_SECMARK_H_target
#define _XT_SECMARK_H_target

#define SECMARK_MODE_SEL	0x01		/* SELinux */
#define SECMARK_SELCTX_MAX	256

struct xt_secmark_target_selinux_info {
	u_int32_t selsid;
	char selctx[SECMARK_SELCTX_MAX];
};

struct xt_secmark_target_info {
	u_int8_t mode;
	union {
		struct xt_secmark_target_selinux_info sel;
	} u;
};

#endif /*_XT_SECMARK_H_target */
