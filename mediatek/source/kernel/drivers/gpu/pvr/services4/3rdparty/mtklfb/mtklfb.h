

#ifndef __MTKLFB_H__
#define __MTKLFB_H__

#include <linux/version.h>

#include <asm/atomic.h>

#include <linux/kernel.h>
#include <linux/console.h>
#include <linux/fb.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/notifier.h>
#include <linux/mutex.h>

#ifdef CONFIG_HAS_EARLYSUSPEND
#include <linux/earlysuspend.h>
#endif

#define unref__ __attribute__ ((unused))

typedef void *       MTKLFB_HANDLE;

typedef bool MTKLFB_BOOL, *MTKLFB_PBOOL;
#define	MTKLFB_FALSE false
#define MTKLFB_TRUE true

typedef	atomic_t	MTKLFB_ATOMIC_BOOL;

typedef atomic_t	MTKLFB_ATOMIC_INT;

typedef struct MTKLFB_BUFFER_TAG
{
	struct MTKLFB_BUFFER_TAG	*psNext;
	struct MTKLFB_DEVINFO_TAG	*psDevInfo;

	struct work_struct sWork;

	
	unsigned long		     	ulYOffset;

	
	
	IMG_SYS_PHYADDR              	sSysAddr;
	IMG_CPU_VIRTADDR             	sCPUVAddr;
	PVRSRV_SYNC_DATA            	*psSyncData;

	MTKLFB_HANDLE      		hCmdComplete;
	unsigned long    		ulSwapInterval;
} MTKLFB_BUFFER;

typedef struct MTKLFB_SWAPCHAIN_TAG
{
	
	unsigned int			uiSwapChainID;

	
	unsigned long       		ulBufferCount;

	
	MTKLFB_BUFFER     		*psBuffer;

	
	struct workqueue_struct   	*psWorkQueue;

	
	MTKLFB_BOOL			bNotVSynced;

	
	int				iBlankEvents;

	
	unsigned int            	uiFBDevID;
} MTKLFB_SWAPCHAIN;

typedef struct MTKLFB_FBINFO_TAG
{
	unsigned long       ulFBSize;
	unsigned long       ulBufferSize;
	unsigned long       ulRoundedBufferSize;
	unsigned long       ulWidth;
	unsigned long       ulHeight;
	unsigned long       ulByteStride;
	unsigned long       ulPhysicalWidthmm;
	unsigned long       ulPhysicalHeightmm;

	
	
	IMG_SYS_PHYADDR     sSysAddr;
	IMG_CPU_VIRTADDR    sCPUVAddr;

	
	PVRSRV_PIXEL_FORMAT ePixelFormat;
}MTKLFB_FBINFO;

typedef struct MTKLFB_DEVINFO_TAG
{
	
	unsigned int            uiFBDevID;

	
	unsigned int            uiPVRDevID;

	
	struct mutex		sCreateSwapChainMutex;

	
	MTKLFB_BUFFER          sSystemBuffer;

	
	PVRSRV_DC_DISP2SRV_KMJTABLE	sPVRJTable;
	
	
	PVRSRV_DC_SRV2DISP_KMJTABLE	sDCJTable;

	
	MTKLFB_FBINFO          sFBInfo;

	
	MTKLFB_SWAPCHAIN      *psSwapChain;

	
	unsigned int		uiSwapChainID;

	
	MTKLFB_ATOMIC_BOOL     sFlushCommands;

	
	struct fb_info         *psLINFBInfo;

	
	struct notifier_block   sLINNotifBlock;

	
	

	
	IMG_DEV_VIRTADDR	sDisplayDevVAddr;

	DISPLAY_INFO            sDisplayInfo;

	
	DISPLAY_FORMAT          sDisplayFormat;
	
	
	DISPLAY_DIMS            sDisplayDim;

	
	MTKLFB_ATOMIC_BOOL	sBlanked;

	
	MTKLFB_ATOMIC_INT	sBlankEvents;

#ifdef CONFIG_HAS_EARLYSUSPEND
	
	MTKLFB_ATOMIC_BOOL	sEarlySuspendFlag;

	struct early_suspend    sEarlySuspend;
#endif

#if defined(SUPPORT_DRI_DRM)
	MTKLFB_ATOMIC_BOOL     sLeaveVT;
#endif

}  MTKLFB_DEVINFO;

#define	MTKLFB_PAGE_SIZE 4096

#ifdef	DEBUG
#define	DEBUG_PRINTK(x) printk x
#else
#define	DEBUG_PRINTK(x)
#endif

#define DISPLAY_DEVICE_NAME "PowerVR MTK Linux Display Driver"
#define	DRVNAME	"mtklfb"
#define	DEVNAME	DRVNAME
#define	DRIVER_PREFIX DRVNAME

typedef enum _MTKLFB_ERROR_
{
	MTKLFB_OK                             =  0,
	MTKLFB_ERROR_GENERIC                  =  1,
	MTKLFB_ERROR_OUT_OF_MEMORY            =  2,
	MTKLFB_ERROR_TOO_FEW_BUFFERS          =  3,
	MTKLFB_ERROR_INVALID_PARAMS           =  4,
	MTKLFB_ERROR_INIT_FAILURE             =  5,
	MTKLFB_ERROR_CANT_REGISTER_CALLBACK   =  6,
	MTKLFB_ERROR_INVALID_DEVICE           =  7,
	MTKLFB_ERROR_DEVICE_REGISTER_FAILED   =  8,
	MTKLFB_ERROR_SET_UPDATE_MODE_FAILED   =  9
} MTKLFB_ERROR;

typedef enum _MTKLFB_UPDATE_MODE_
{
	MTKLFB_UPDATE_MODE_UNDEFINED			= 0,
	MTKLFB_UPDATE_MODE_MANUAL			= 1,
	MTKLFB_UPDATE_MODE_AUTO			= 2,
	MTKLFB_UPDATE_MODE_DISABLED			= 3
} MTKLFB_UPDATE_MODE;

#ifndef UNREFERENCED_PARAMETER
#define	UNREFERENCED_PARAMETER(param) (param) = (param)
#endif

MTKLFB_ERROR MTKLFBInit(void);
MTKLFB_ERROR MTKLFBDeInit(void);

MTKLFB_DEVINFO *MTKLFBGetDevInfoPtr(unsigned uiFBDevID);
unsigned MTKLFBMaxFBDevIDPlusOne(void);
void *MTKLFBAllocKernelMem(unsigned long ulSize);
void MTKLFBFreeKernelMem(void *pvMem);
MTKLFB_ERROR MTKLFBGetLibFuncAddr(char *szFunctionName, PFN_DC_GET_PVRJTABLE *ppfnFuncTable);
MTKLFB_ERROR MTKLFBCreateSwapQueue (MTKLFB_SWAPCHAIN *psSwapChain);
void MTKLFBDestroySwapQueue(MTKLFB_SWAPCHAIN *psSwapChain);
void MTKLFBInitBufferForSwap(MTKLFB_BUFFER *psBuffer);
void MTKLFBSwapHandler(MTKLFB_BUFFER *psBuffer);
void MTKLFBQueueBufferForSwap(MTKLFB_SWAPCHAIN *psSwapChain, MTKLFB_BUFFER *psBuffer);
void MTKLFBFlip(MTKLFB_DEVINFO *psDevInfo, MTKLFB_BUFFER *psBuffer);
MTKLFB_UPDATE_MODE MTKLFBGetUpdateMode(MTKLFB_DEVINFO *psDevInfo);
MTKLFB_BOOL MTKLFBSetUpdateMode(MTKLFB_DEVINFO *psDevInfo, MTKLFB_UPDATE_MODE eMode);
MTKLFB_BOOL MTKLFBWaitForVSync(MTKLFB_DEVINFO *psDevInfo);
MTKLFB_BOOL MTKLFBManualSync(MTKLFB_DEVINFO *psDevInfo);
MTKLFB_BOOL MTKLFBCheckModeAndSync(MTKLFB_DEVINFO *psDevInfo);
MTKLFB_ERROR MTKLFBUnblankDisplay(MTKLFB_DEVINFO *psDevInfo);
MTKLFB_ERROR MTKLFBEnableLFBEventNotification(MTKLFB_DEVINFO *psDevInfo);
MTKLFB_ERROR MTKLFBDisableLFBEventNotification(MTKLFB_DEVINFO *psDevInfo);
void MTKLFBCreateSwapChainLockInit(MTKLFB_DEVINFO *psDevInfo);
void MTKLFBCreateSwapChainLockDeInit(MTKLFB_DEVINFO *psDevInfo);
void MTKLFBCreateSwapChainLock(MTKLFB_DEVINFO *psDevInfo);
void MTKLFBCreateSwapChainUnLock(MTKLFB_DEVINFO *psDevInfo);
void MTKLFBAtomicBoolInit(MTKLFB_ATOMIC_BOOL *psAtomic, MTKLFB_BOOL bVal);
void MTKLFBAtomicBoolDeInit(MTKLFB_ATOMIC_BOOL *psAtomic);
void MTKLFBAtomicBoolSet(MTKLFB_ATOMIC_BOOL *psAtomic, MTKLFB_BOOL bVal);
MTKLFB_BOOL MTKLFBAtomicBoolRead(MTKLFB_ATOMIC_BOOL *psAtomic);
void MTKLFBAtomicIntInit(MTKLFB_ATOMIC_INT *psAtomic, int iVal);
void MTKLFBAtomicIntDeInit(MTKLFB_ATOMIC_INT *psAtomic);
void MTKLFBAtomicIntSet(MTKLFB_ATOMIC_INT *psAtomic, int iVal);
int MTKLFBAtomicIntRead(MTKLFB_ATOMIC_INT *psAtomic);
void MTKLFBAtomicIntInc(MTKLFB_ATOMIC_INT *psAtomic);

#endif 

