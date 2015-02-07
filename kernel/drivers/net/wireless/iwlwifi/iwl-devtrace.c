

#include <linux/module.h>

/* sparse doesn't like tracepoint macros */
#ifndef __CHECKER__
#include "iwl-dev.h"

#define CREATE_TRACE_POINTS
#include "iwl-devtrace.h"

EXPORT_TRACEPOINT_SYMBOL(iwlwifi_dev_iowrite8);
EXPORT_TRACEPOINT_SYMBOL(iwlwifi_dev_ioread32);
EXPORT_TRACEPOINT_SYMBOL(iwlwifi_dev_iowrite32);
EXPORT_TRACEPOINT_SYMBOL(iwlwifi_dev_rx);
EXPORT_TRACEPOINT_SYMBOL(iwlwifi_dev_tx);
EXPORT_TRACEPOINT_SYMBOL(iwlwifi_dev_ucode_event);
EXPORT_TRACEPOINT_SYMBOL(iwlwifi_dev_ucode_error);
EXPORT_TRACEPOINT_SYMBOL(iwlwifi_dev_ucode_cont_event);
EXPORT_TRACEPOINT_SYMBOL(iwlwifi_dev_ucode_wrap_event);
#endif
