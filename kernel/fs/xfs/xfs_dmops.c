
#include "xfs.h"
#include "xfs_fs.h"
#include "xfs_types.h"
#include "xfs_log.h"
#include "xfs_trans.h"
#include "xfs_sb.h"
#include "xfs_dmapi.h"
#include "xfs_inum.h"
#include "xfs_ag.h"
#include "xfs_mount.h"


static struct xfs_dmops xfs_dmcore_stub = {
	.xfs_send_data		= (xfs_send_data_t)fs_nosys,
	.xfs_send_mmap		= (xfs_send_mmap_t)fs_noerr,
	.xfs_send_destroy	= (xfs_send_destroy_t)fs_nosys,
	.xfs_send_namesp	= (xfs_send_namesp_t)fs_nosys,
	.xfs_send_mount		= (xfs_send_mount_t)fs_nosys,
	.xfs_send_unmount	= (xfs_send_unmount_t)fs_noerr,
};

int
xfs_dmops_get(struct xfs_mount *mp)
{
	if (mp->m_flags & XFS_MOUNT_DMAPI) {
		cmn_err(CE_WARN,
			"XFS: dmapi support not available in this kernel.");
		return EINVAL;
	}

	mp->m_dm_ops = &xfs_dmcore_stub;
	return 0;
}

void
xfs_dmops_put(struct xfs_mount *mp)
{
}
