
#ifndef __DDRAW_INCLUDED__
#define __DDRAW_INCLUDED__
#if defined( _WIN32 )  && !defined( _NO_COM )
#define COM_NO_WINDOWS_H
#include <objbase.h>
#else
#define IUnknown            void
#define CO_E_NOTINITIALIZED 0x800401F0L
#endif

#define _FACDD  0x876
#define MAKE_DDHRESULT( code )  MAKE_HRESULT( 1, _FACDD, code )

#ifdef __cplusplus
extern "C" {
#endif

#if defined( _WIN32 ) && !defined( _NO_COM )
DEFINE_GUID( CLSID_DirectDraw,                  0xD7B70EE0,0x4340,0x11CF,0xB0,0x63,0x00,0x20,0xAF,0xC2,0xCD,0x35 );
DEFINE_GUID( CLSID_DirectDrawClipper,           0x593817A0,0x7DB3,0x11CF,0xA2,0xDE,0x00,0xAA,0x00,0xb9,0x33,0x56 );
DEFINE_GUID( IID_IDirectDraw,                   0x6C14DB80,0xA733,0x11CE,0xA5,0x21,0x00,0x20,0xAF,0x0B,0xE5,0x60 );
DEFINE_GUID( IID_IDirectDraw2,                  0xB3A6F3E0,0x2B43,0x11CF,0xA2,0xDE,0x00,0xAA,0x00,0xB9,0x33,0x56 );
DEFINE_GUID( IID_IDirectDrawSurface,            0x6C14DB81,0xA733,0x11CE,0xA5,0x21,0x00,0x20,0xAF,0x0B,0xE5,0x60 );
DEFINE_GUID( IID_IDirectDrawSurface2,           0x57805885,0x6eec,0x11cf,0x94,0x41,0xa8,0x23,0x03,0xc1,0x0e,0x27 );

DEFINE_GUID( IID_IDirectDrawPalette,            0x6C14DB84,0xA733,0x11CE,0xA5,0x21,0x00,0x20,0xAF,0x0B,0xE5,0x60 );
DEFINE_GUID( IID_IDirectDrawClipper,            0x6C14DB85,0xA733,0x11CE,0xA5,0x21,0x00,0x20,0xAF,0x0B,0xE5,0x60 );
                 
#endif


struct IDirectDraw;
struct IDirectDrawSurface;
struct IDirectDrawPalette;
struct IDirectDrawClipper;

typedef struct IDirectDraw                      FAR *LPDIRECTDRAW;
typedef struct IDirectDraw2                     FAR *LPDIRECTDRAW2;
typedef struct IDirectDrawSurface               FAR *LPDIRECTDRAWSURFACE;
typedef struct IDirectDrawSurface2              FAR *LPDIRECTDRAWSURFACE2;

typedef struct IDirectDrawPalette               FAR *LPDIRECTDRAWPALETTE;
typedef struct IDirectDrawClipper               FAR *LPDIRECTDRAWCLIPPER;

typedef struct _DDFXROP                 FAR *LPDDFXROP;
typedef struct _DDSURFACEDESC           FAR *LPDDSURFACEDESC;

#if (defined (WIN32) || defined( _WIN32 ) ) && !defined( _NO_COM )
//#if defined( _WIN32 ) && !defined( _NO_ENUM )
    typedef BOOL (FAR PASCAL * LPDDENUMCALLBACKA)(GUID FAR *, LPSTR, LPSTR, LPVOID);
    typedef BOOL (FAR PASCAL * LPDDENUMCALLBACKW)(GUID FAR *, LPWSTR, LPWSTR, LPVOID);
    extern HRESULT WINAPI DirectDrawEnumerateW( LPDDENUMCALLBACKW lpCallback, LPVOID lpContext );
    extern HRESULT WINAPI DirectDrawEnumerateA( LPDDENUMCALLBACKA lpCallback, LPVOID lpContext );
    #ifdef UNICODE
        typedef LPDDENUMCALLBACKW       LPDDENUMCALLBACK;
        #define DirectDrawEnumerate     DirectDrawEnumerateW
    #else
        typedef LPDDENUMCALLBACKA       LPDDENUMCALLBACK;
        #define DirectDrawEnumerate     DirectDrawEnumerateA
    #endif
    extern HRESULT WINAPI DirectDrawCreate( GUID FAR *lpGUID, LPDIRECTDRAW FAR *lplpDD, IUnknown FAR *pUnkOuter );
    extern HRESULT WINAPI DirectDrawCreateClipper( DWORD dwFlags, LPDIRECTDRAWCLIPPER FAR *lplpDDClipper, IUnknown FAR *pUnkOuter );
    #ifdef WINNT
        //This is the user-mode entry stub to the kernel mode procedure.
        extern HRESULT NtDirectDrawCreate( GUID FAR *lpGUID, HANDLE *lplpDD, IUnknown FAR *pUnkOuter );
    #endif
#endif

#define REGSTR_KEY_DDHW_DESCRIPTION     "Description"
#define REGSTR_KEY_DDHW_DRIVERNAME      "DriverName"
#define REGSTR_PATH_DDHW                "Hardware\\DirectDrawDrivers"

#define DDCREATE_HARDWAREONLY           0x00000001l
#define DDCREATE_EMULATIONONLY          0x00000002l

#ifdef WINNT
typedef long HRESULT;
#endif

//#ifndef WINNT
typedef HRESULT (FAR PASCAL * LPDDENUMMODESCALLBACK)(LPDDSURFACEDESC, LPVOID);
typedef HRESULT (FAR PASCAL * LPDDENUMSURFACESCALLBACK)(LPDIRECTDRAWSURFACE, LPDDSURFACEDESC, LPVOID);
//#endif
typedef struct _DDCOLORKEY
{
    DWORD       dwColorSpaceLowValue;   // low boundary of color space that is to 
                                        // be treated as Color Key, inclusive
    DWORD       dwColorSpaceHighValue;  // high boundary of color space that is 
                                        // to be treated as Color Key, inclusive
} DDCOLORKEY;

typedef DDCOLORKEY FAR* LPDDCOLORKEY;

typedef struct _DDBLTFX
{
    DWORD       dwSize;                         // size of structure
    DWORD       dwDDFX;                         // FX operations
    DWORD       dwROP;                          // Win32 raster operations
    DWORD       dwDDROP;                        // Raster operations new for DirectDraw
    DWORD       dwRotationAngle;                // Rotation angle for blt
    DWORD       dwZBufferOpCode;                // ZBuffer compares
    DWORD       dwZBufferLow;                   // Low limit of Z buffer
    DWORD       dwZBufferHigh;                  // High limit of Z buffer
    DWORD       dwZBufferBaseDest;              // Destination base value
    DWORD       dwZDestConstBitDepth;           // Bit depth used to specify Z constant for destination
    union
    {
        DWORD   dwZDestConst;                   // Constant to use as Z buffer for dest
        LPDIRECTDRAWSURFACE lpDDSZBufferDest;   // Surface to use as Z buffer for dest
    };
    DWORD       dwZSrcConstBitDepth;            // Bit depth used to specify Z constant for source
    union
    {
        DWORD   dwZSrcConst;                    // Constant to use as Z buffer for src
        LPDIRECTDRAWSURFACE lpDDSZBufferSrc;    // Surface to use as Z buffer for src
    };
    DWORD       dwAlphaEdgeBlendBitDepth;       // Bit depth used to specify constant for alpha edge blend
    DWORD       dwAlphaEdgeBlend;               // Alpha for edge blending
    DWORD       dwReserved;
    DWORD       dwAlphaDestConstBitDepth;       // Bit depth used to specify alpha constant for destination
    union
    {
        DWORD   dwAlphaDestConst;               // Constant to use as Alpha Channel
        LPDIRECTDRAWSURFACE lpDDSAlphaDest;     // Surface to use as Alpha Channel
    };
    DWORD       dwAlphaSrcConstBitDepth;        // Bit depth used to specify alpha constant for source
    union
    {
        DWORD   dwAlphaSrcConst;                // Constant to use as Alpha Channel
        LPDIRECTDRAWSURFACE lpDDSAlphaSrc;      // Surface to use as Alpha Channel
    };
    union
    {
        DWORD   dwFillColor;                    // color in RGB or Palettized
        DWORD   dwFillDepth;                    // depth value for z-buffer
        LPDIRECTDRAWSURFACE lpDDSPattern;       // Surface to use as pattern
    };
    DDCOLORKEY  ddckDestColorkey;               // DestColorkey override
    DDCOLORKEY  ddckSrcColorkey;                // SrcColorkey override
} DDBLTFX;

typedef DDBLTFX FAR* LPDDBLTFX;


typedef struct _DDSCAPS
{
    DWORD       dwCaps;         // capabilities of surface wanted
} DDSCAPS;

typedef DDSCAPS FAR* LPDDSCAPS; 

#define DD_ROP_SPACE            (256/32)        // space required to store ROP array

typedef struct _DDCAPS
{
    DWORD       dwSize;                 // size of the DDDRIVERCAPS structure
    DWORD       dwCaps;                 // driver specific capabilities
    DWORD       dwCaps2;                // more driver specific capabilites
    DWORD       dwCKeyCaps;             // color key capabilities of the surface
    DWORD       dwFXCaps;               // driver specific stretching and effects capabilites
    DWORD       dwFXAlphaCaps;          // alpha driver specific capabilities
    DWORD       dwPalCaps;              // palette capabilities
    DWORD       dwSVCaps;               // stereo vision capabilities
    DWORD       dwAlphaBltConstBitDepths;       // DDBD_2,4,8
    DWORD       dwAlphaBltPixelBitDepths;       // DDBD_1,2,4,8
    DWORD       dwAlphaBltSurfaceBitDepths;     // DDBD_1,2,4,8
    DWORD       dwAlphaOverlayConstBitDepths;   // DDBD_2,4,8
    DWORD       dwAlphaOverlayPixelBitDepths;   // DDBD_1,2,4,8
    DWORD       dwAlphaOverlaySurfaceBitDepths; // DDBD_1,2,4,8
    DWORD       dwZBufferBitDepths;             // DDBD_8,16,24,32
    DWORD       dwVidMemTotal;          // total amount of video memory
    DWORD       dwVidMemFree;           // amount of free video memory
    DWORD       dwMaxVisibleOverlays;   // maximum number of visible overlays
    DWORD       dwCurrVisibleOverlays;  // current number of visible overlays
    DWORD       dwNumFourCCCodes;       // number of four cc codes
    DWORD       dwAlignBoundarySrc;     // source rectangle alignment
    DWORD       dwAlignSizeSrc;         // source rectangle byte size
    DWORD       dwAlignBoundaryDest;    // dest rectangle alignment
    DWORD       dwAlignSizeDest;        // dest rectangle byte size
    DWORD       dwAlignStrideAlign;     // stride alignment
    DWORD       dwRops[DD_ROP_SPACE];   // ROPS supported
    DDSCAPS     ddsCaps;                // DDSCAPS structure has all the general capabilities
    DWORD       dwMinOverlayStretch;    // minimum overlay stretch factor multiplied by 1000, eg 1000 == 1.0, 1300 == 1.3
    DWORD       dwMaxOverlayStretch;    // maximum overlay stretch factor multiplied by 1000, eg 1000 == 1.0, 1300 == 1.3
    DWORD       dwMinLiveVideoStretch;  // minimum live video stretch factor multiplied by 1000, eg 1000 == 1.0, 1300 == 1.3
    DWORD       dwMaxLiveVideoStretch;  // maximum live video stretch factor multiplied by 1000, eg 1000 == 1.0, 1300 == 1.3
    DWORD       dwMinHwCodecStretch;    // minimum hardware codec stretch factor multiplied by 1000, eg 1000 == 1.0, 1300 == 1.3
    DWORD       dwMaxHwCodecStretch;    // maximum hardware codec stretch factor multiplied by 1000, eg 1000 == 1.0, 1300 == 1.3
    DWORD       dwReserved1;            // reserved
    DWORD       dwReserved2;            // reserved
    DWORD       dwReserved3;            // reserved
    DWORD       dwSVBCaps;              // driver specific capabilities for System->Vmem blts
    DWORD       dwSVBCKeyCaps;          // driver color key capabilities for System->Vmem blts
    DWORD       dwSVBFXCaps;            // driver FX capabilities for System->Vmem blts
    DWORD       dwSVBRops[DD_ROP_SPACE];// ROPS supported for System->Vmem blts
    DWORD       dwVSBCaps;              // driver specific capabilities for Vmem->System blts
    DWORD       dwVSBCKeyCaps;          // driver color key capabilities for Vmem->System blts
    DWORD       dwVSBFXCaps;            // driver FX capabilities for Vmem->System blts
    DWORD       dwVSBRops[DD_ROP_SPACE];// ROPS supported for Vmem->System blts
    DWORD       dwSSBCaps;              // driver specific capabilities for System->System blts
    DWORD       dwSSBCKeyCaps;          // driver color key capabilities for System->System blts
    DWORD       dwSSBFXCaps;            // driver FX capabilities for System->System blts
    DWORD       dwSSBRops[DD_ROP_SPACE];// ROPS supported for System->System blts
    DWORD       dwReserved4;            // reserved
    DWORD       dwReserved5;            // reserved
    DWORD       dwReserved6;            // reserved
} DDCAPS;

typedef DDCAPS FAR* LPDDCAPS;



typedef struct _DDPIXELFORMAT
{
    DWORD       dwSize;                 // size of structure
    DWORD       dwFlags;                // pixel format flags
    DWORD       dwFourCC;               // (FOURCC code)
    union
    {
        DWORD   dwRGBBitCount;          // how many bits per pixel (BD_4,8,16,24,32)
        DWORD   dwYUVBitCount;          // how many bits per pixel (BD_4,8,16,24,32)
        DWORD   dwZBufferBitDepth;      // how many bits for z buffers (BD_8,16,24,32)
        DWORD   dwAlphaBitDepth;        // how many bits for alpha channels (BD_1,2,4,8)
    };
    union
    {
        DWORD   dwRBitMask;             // mask for red bit
        DWORD   dwYBitMask;             // mask for Y bits
    };
    union
    {
        DWORD   dwGBitMask;             // mask for green bits
        DWORD   dwUBitMask;             // mask for U bits
    };
    union
    {
        DWORD   dwBBitMask;             // mask for blue bits
        DWORD   dwVBitMask;             // mask for V bits
    };
    union
    {
        DWORD   dwRGBAlphaBitMask;      // mask for alpha channel
        DWORD   dwYUVAlphaBitMask;      // mask for alpha channel
    };
} DDPIXELFORMAT;

typedef DDPIXELFORMAT FAR* LPDDPIXELFORMAT;

typedef struct _DDOVERLAYFX
{
    DWORD       dwSize;                         // size of structure
    DWORD       dwAlphaEdgeBlendBitDepth;       // Bit depth used to specify constant for alpha edge blend
    DWORD       dwAlphaEdgeBlend;               // Constant to use as alpha for edge blend
    DWORD       dwReserved;
    DWORD       dwAlphaDestConstBitDepth;       // Bit depth used to specify alpha constant for destination
    union
    {
        DWORD   dwAlphaDestConst;               // Constant to use as alpha channel for dest
        LPDIRECTDRAWSURFACE lpDDSAlphaDest;     // Surface to use as alpha channel for dest
    };
    DWORD       dwAlphaSrcConstBitDepth;        // Bit depth used to specify alpha constant for source
    union
    {
        DWORD   dwAlphaSrcConst;                // Constant to use as alpha channel for src
        LPDIRECTDRAWSURFACE lpDDSAlphaSrc;      // Surface to use as alpha channel for src
    };
    DDCOLORKEY  dckDestColorkey;                // DestColorkey override
    DDCOLORKEY  dckSrcColorkey;                 // DestColorkey override
    DWORD       dwDDFX;                         // Overlay FX
    DWORD       dwFlags;                        // flags
} DDOVERLAYFX;

typedef DDOVERLAYFX FAR *LPDDOVERLAYFX;

typedef struct _DDBLTBATCH
{
    LPRECT              lprDest;
    LPDIRECTDRAWSURFACE lpDDSSrc;
    LPRECT              lprSrc;
    DWORD               dwFlags;
    LPDDBLTFX           lpDDBltFx;
} DDBLTBATCH;

typedef DDBLTBATCH FAR * LPDDBLTBATCH;

typedef DWORD   (FAR PASCAL *LPCLIPPERCALLBACK)(LPDIRECTDRAWCLIPPER lpDDClipper, HWND hWnd, DWORD code, LPVOID lpContext );
#ifdef STREAMING
typedef DWORD   (FAR PASCAL *LPSURFACESTREAMINGCALLBACK)(DWORD);
#endif



#if defined( _WIN32 ) && !defined( _NO_COM )
#undef INTERFACE
#define INTERFACE IDirectDraw
DECLARE_INTERFACE_( IDirectDraw, IUnknown )
{
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID FAR * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;
    /*** IDirectDraw methods ***/
    STDMETHOD(Compact)(THIS) PURE;
    STDMETHOD(CreateClipper)(THIS_ DWORD, LPDIRECTDRAWCLIPPER FAR*, IUnknown FAR * ) PURE;
    STDMETHOD(CreatePalette)(THIS_ DWORD, LPPALETTEENTRY, LPDIRECTDRAWPALETTE FAR*, IUnknown FAR * ) PURE;
    STDMETHOD(CreateSurface)(THIS_  LPDDSURFACEDESC, LPDIRECTDRAWSURFACE FAR *, IUnknown FAR *) PURE;
    STDMETHOD(DuplicateSurface)( THIS_ LPDIRECTDRAWSURFACE, LPDIRECTDRAWSURFACE FAR * ) PURE;
    STDMETHOD(EnumDisplayModes)( THIS_ DWORD, LPDDSURFACEDESC, LPVOID, LPDDENUMMODESCALLBACK ) PURE;
    STDMETHOD(EnumSurfaces)(THIS_ DWORD, LPDDSURFACEDESC, LPVOID,LPDDENUMSURFACESCALLBACK ) PURE;
    STDMETHOD(FlipToGDISurface)(THIS) PURE;
    STDMETHOD(GetCaps)( THIS_ LPDDCAPS, LPDDCAPS) PURE;
    STDMETHOD(GetDisplayMode)( THIS_ LPDDSURFACEDESC) PURE;
    STDMETHOD(GetFourCCCodes)(THIS_  LPDWORD, LPDWORD ) PURE;
    STDMETHOD(GetGDISurface)(THIS_ LPDIRECTDRAWSURFACE FAR *) PURE;
    STDMETHOD(GetMonitorFrequency)(THIS_ LPDWORD) PURE;
    STDMETHOD(GetScanLine)(THIS_ LPDWORD) PURE;
    STDMETHOD(GetVerticalBlankStatus)(THIS_ LPBOOL ) PURE;
    STDMETHOD(Initialize)(THIS_ GUID FAR *) PURE;
    STDMETHOD(RestoreDisplayMode)(THIS) PURE;
    STDMETHOD(SetCooperativeLevel)(THIS_ HWND, DWORD) PURE;
    STDMETHOD(SetDisplayMode)(THIS_ DWORD, DWORD,DWORD) PURE;
    STDMETHOD(WaitForVerticalBlank)(THIS_ DWORD, HANDLE ) PURE;
};

#if !defined(__cplusplus) || defined(CINTERFACE)
#define IDirectDraw_QueryInterface(p, a, b)         (p)->lpVtbl->QueryInterface(p, a, b)
#define IDirectDraw_AddRef(p)                       (p)->lpVtbl->AddRef(p)
#define IDirectDraw_Release(p)                      (p)->lpVtbl->Release(p)
#define IDirectDraw_Compact(p)                      (p)->lpVtbl->Compact(p)
#define IDirectDraw_CreateClipper(p, a, b, c)       (p)->lpVtbl->CreateClipper(p, a, b, c)
#define IDirectDraw_CreatePalette(p, a, b, c, d)    (p)->lpVtbl->CreatePalette(p, a, b, c, d)
#define IDirectDraw_CreateSurface(p, a, b, c)       (p)->lpVtbl->CreateSurface(p, a, b, c)
#define IDirectDraw_DuplicateSurface(p, a, b)       (p)->lpVtbl->DuplicateSurface(p, a, b)
#define IDirectDraw_EnumDisplayModes(p, a, b, c, d) (p)->lpVtbl->EnumDisplayModes(p, a, b, c, d)
#define IDirectDraw_EnumSurfaces(p, a, b, c, d)     (p)->lpVtbl->EnumSurfaces(p, a, b, c, d)
#define IDirectDraw_FlipToGDISurface(p)             (p)->lpVtbl->FlipToGDISurface(p)
#define IDirectDraw_GetCaps(p, a, b)                (p)->lpVtbl->GetCaps(p, a, b)
#define IDirectDraw_GetDisplayMode(p, a)            (p)->lpVtbl->GetDisplayMode(p, a)
#define IDirectDraw_GetFourCCCodes(p, a, b)         (p)->lpVtbl->GetFourCCCodes(p, a, b)
#define IDirectDraw_GetGDISurface(p, a)             (p)->lpVtbl->GetGDISurface(p, a)
#define IDirectDraw_GetMonitorFrequency(p, a)       (p)->lpVtbl->GetMonitorFrequency(p, a)
#define IDirectDraw_GetScanLine(p, a)               (p)->lpVtbl->GetScanLine(p, a)
#define IDirectDraw_GetVerticalBlankStatus(p, a)    (p)->lpVtbl->GetVerticalBlankStatus(p, a)
#define IDirectDraw_Initialize(p, a)                (p)->lpVtbl->Initialize(p, a)
#define IDirectDraw_RestoreDisplayMode(p)           (p)->lpVtbl->RestoreDisplayMode(p)
#define IDirectDraw_SetCooperativeLevel(p, a, b)    (p)->lpVtbl->SetCooperativeLevel(p, a, b)
#define IDirectDraw_SetDisplayMode(p, a, b, c)      (p)->lpVtbl->SetDisplayMode(p, a, b, c)
#define IDirectDraw_WaitForVerticalBlank(p, a, b)   (p)->lpVtbl->WaitForVerticalBlank(p, a, b)
#endif

#endif

#if defined( _WIN32 ) && !defined( _NO_COM )
#undef INTERFACE
#define INTERFACE IDirectDraw2
DECLARE_INTERFACE_( IDirectDraw2, IUnknown )
{
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID FAR * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;
    /*** IDirectDraw methods ***/
    STDMETHOD(Compact)(THIS) PURE;
    STDMETHOD(CreateClipper)(THIS_ DWORD, LPDIRECTDRAWCLIPPER FAR*, IUnknown FAR * ) PURE;
    STDMETHOD(CreatePalette)(THIS_ DWORD, LPPALETTEENTRY, LPDIRECTDRAWPALETTE FAR*, IUnknown FAR * ) PURE;
    STDMETHOD(CreateSurface)(THIS_  LPDDSURFACEDESC, LPDIRECTDRAWSURFACE FAR *, IUnknown FAR *) PURE;
    STDMETHOD(DuplicateSurface)( THIS_ LPDIRECTDRAWSURFACE, LPDIRECTDRAWSURFACE FAR * ) PURE;
    STDMETHOD(EnumDisplayModes)( THIS_ DWORD, LPDDSURFACEDESC, LPVOID, LPDDENUMMODESCALLBACK ) PURE;
    STDMETHOD(EnumSurfaces)(THIS_ DWORD, LPDDSURFACEDESC, LPVOID,LPDDENUMSURFACESCALLBACK ) PURE;
    STDMETHOD(FlipToGDISurface)(THIS) PURE;
    STDMETHOD(GetCaps)( THIS_ LPDDCAPS, LPDDCAPS) PURE;
    STDMETHOD(GetDisplayMode)( THIS_ LPDDSURFACEDESC) PURE;
    STDMETHOD(GetFourCCCodes)(THIS_  LPDWORD, LPDWORD ) PURE;
    STDMETHOD(GetGDISurface)(THIS_ LPDIRECTDRAWSURFACE FAR *) PURE;
    STDMETHOD(GetMonitorFrequency)(THIS_ LPDWORD) PURE;
    STDMETHOD(GetScanLine)(THIS_ LPDWORD) PURE;
    STDMETHOD(GetVerticalBlankStatus)(THIS_ LPBOOL ) PURE;
    STDMETHOD(Initialize)(THIS_ GUID FAR *) PURE;
    STDMETHOD(RestoreDisplayMode)(THIS) PURE;
    STDMETHOD(SetCooperativeLevel)(THIS_ HWND, DWORD) PURE;
    STDMETHOD(SetDisplayMode)(THIS_ DWORD, DWORD,DWORD, DWORD, DWORD) PURE;
    STDMETHOD(WaitForVerticalBlank)(THIS_ DWORD, HANDLE ) PURE;
    /*** Added in the v2 interface ***/
    STDMETHOD(GetAvailableVidMem)(THIS_ LPDDSCAPS, LPDWORD, LPDWORD) PURE;
};
#if !defined(__cplusplus) || defined(CINTERFACE)
#define IDirectDraw2_QueryInterface(p, a, b)         (p)->lpVtbl->QueryInterface(p, a, b)
#define IDirectDraw2_AddRef(p)                       (p)->lpVtbl->AddRef(p)
#define IDirectDraw2_Release(p)                      (p)->lpVtbl->Release(p)
#define IDirectDraw2_Compact(p)                      (p)->lpVtbl->Compact(p)
#define IDirectDraw2_CreateClipper(p, a, b, c)       (p)->lpVtbl->CreateClipper(p, a, b, c)
#define IDirectDraw2_CreatePalette(p, a, b, c, d)    (p)->lpVtbl->CreatePalette(p, a, b, c, d)
#define IDirectDraw2_CreateSurface(p, a, b, c)       (p)->lpVtbl->CreateSurface(p, a, b, c)
#define IDirectDraw2_DuplicateSurface(p, a, b)       (p)->lpVtbl->DuplicateSurface(p, a, b)
#define IDirectDraw2_EnumDisplayModes(p, a, b, c, d) (p)->lpVtbl->EnumDisplayModes(p, a, b, c, d)
#define IDirectDraw2_EnumSurfaces(p, a, b, c, d)     (p)->lpVtbl->EnumSurfaces(p, a, b, c, d)
#define IDirectDraw2_FlipToGDISurface(p)             (p)->lpVtbl->FlipToGDISurface(p)
#define IDirectDraw2_GetCaps(p, a, b)                (p)->lpVtbl->GetCaps(p, a, b)
#define IDirectDraw2_GetDisplayMode(p, a)            (p)->lpVtbl->GetDisplayMode(p, a)
#define IDirectDraw2_GetFourCCCodes(p, a, b)         (p)->lpVtbl->GetFourCCCodes(p, a, b)
#define IDirectDraw2_GetGDISurface(p, a)             (p)->lpVtbl->GetGDISurface(p, a)
#define IDirectDraw2_GetMonitorFrequency(p, a)       (p)->lpVtbl->GetMonitorFrequency(p, a)
#define IDirectDraw2_GetScanLine(p, a)               (p)->lpVtbl->GetScanLine(p, a)
#define IDirectDraw2_GetVerticalBlankStatus(p, a)    (p)->lpVtbl->GetVerticalBlankStatus(p, a)
#define IDirectDraw2_Initialize(p, a)                (p)->lpVtbl->Initialize(p, a)
#define IDirectDraw2_RestoreDisplayMode(p)           (p)->lpVtbl->RestoreDisplayMode(p)
#define IDirectDraw2_SetCooperativeLevel(p, a, b)    (p)->lpVtbl->SetCooperativeLevel(p, a, b)
#define IDirectDraw2_SetDisplayMode(p, a, b, c, d)   (p)->lpVtbl->SetDisplayMode(p, a, b, c, d)
#define IDirectDraw2_WaitForVerticalBlank(p, a, b)   (p)->lpVtbl->WaitForVerticalBlank(p, a, b)
#define IDirectDraw2_GetAvailableVidMem(p, a, b, c)  (p)->lpVtbl->GetAvailableVidMem(p, a, b, c)
#endif

#endif

#if defined( _WIN32 ) && !defined( _NO_COM )
#undef INTERFACE
#define INTERFACE IDirectDrawPalette
DECLARE_INTERFACE_( IDirectDrawPalette, IUnknown )
{
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID FAR * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;
    /*** IDirectDrawPalette methods ***/
    STDMETHOD(GetCaps)(THIS_ LPDWORD) PURE;
    STDMETHOD(GetEntries)(THIS_ DWORD,DWORD,DWORD,LPPALETTEENTRY) PURE;
    STDMETHOD(Initialize)(THIS_ LPDIRECTDRAW, DWORD, LPPALETTEENTRY) PURE;
    STDMETHOD(SetEntries)(THIS_ DWORD,DWORD,DWORD,LPPALETTEENTRY) PURE;
};

#if !defined(__cplusplus) || defined(CINTERFACE)
#define IDirectDrawPalette_QueryInterface(p, a, b)      (p)->lpVtbl->QueryInterface(p, a, b)
#define IDirectDrawPalette_AddRef(p)                    (p)->lpVtbl->AddRef(p)
#define IDirectDrawPalette_Release(p)                   (p)->lpVtbl->Release(p)
#define IDirectDrawPalette_GetCaps(p, a)                (p)->lpVtbl->GetCaps(p, a)
#define IDirectDrawPalette_GetEntries(p, a, b, c, d)    (p)->lpVtbl->GetEntries(p, a, b, c, d)
#define IDirectDrawPalette_Initialize(p, a, b, c)       (p)->lpVtbl->Initialize(p, a, b, c)
#define IDirectDrawPalette_SetEntries(p, a, b, c, d)    (p)->lpVtbl->SetEntries(p, a, b, c, d)
#endif

#endif

#if defined( _WIN32 ) && !defined( _NO_COM )
#undef INTERFACE
#define INTERFACE IDirectDrawClipper
DECLARE_INTERFACE_( IDirectDrawClipper, IUnknown )
{
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID FAR * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;
    /*** IDirectDrawClipper methods ***/
    STDMETHOD(GetClipList)(THIS_ LPRECT, LPRGNDATA, LPDWORD) PURE;
    STDMETHOD(GetHWnd)(THIS_ HWND FAR *) PURE;
    STDMETHOD(Initialize)(THIS_ LPDIRECTDRAW, DWORD) PURE;
    STDMETHOD(IsClipListChanged)(THIS_ BOOL FAR *) PURE;
    STDMETHOD(SetClipList)(THIS_ LPRGNDATA,DWORD) PURE;
    STDMETHOD(SetHWnd)(THIS_ DWORD, HWND ) PURE;
};

#if !defined(__cplusplus) || defined(CINTERFACE)
#define IDirectDrawClipper_QueryInterface(p, a, b)  (p)->lpVtbl->QueryInterface(p, a, b)
#define IDirectDrawClipper_AddRef(p)                (p)->lpVtbl->AddRef(p)
#define IDirectDrawClipper_Release(p)               (p)->lpVtbl->Release(p)
#define IDirectDrawClipper_GetClipList(p, a, b, c)  (p)->lpVtbl->GetClipList(p, a, b, c)
#define IDirectDrawClipper_GetHWnd(p, a)            (p)->lpVtbl->GetHWnd(p, a)
#define IDirectDrawClipper_Initialize(p, a, b)      (p)->lpVtbl->Initialize(p, a, b)
#define IDirectDrawClipper_IsClipListChanged(p, a)  (p)->lpVtbl->IsClipListChanged(p, a)
#define IDirectDrawClipper_SetClipList(p, a, b)     (p)->lpVtbl->SetClipList(p, a, b)
#define IDirectDrawClipper_SetHWnd(p, a, b)         (p)->lpVtbl->SetHWnd(p, a, b)
#endif

#endif

#if defined( _WIN32 ) && !defined( _NO_COM )
#undef INTERFACE
#define INTERFACE IDirectDrawSurface
DECLARE_INTERFACE_( IDirectDrawSurface, IUnknown )
{
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID FAR * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;
    /*** IDirectDrawSurface methods ***/
    STDMETHOD(AddAttachedSurface)(THIS_ LPDIRECTDRAWSURFACE) PURE;
    STDMETHOD(AddOverlayDirtyRect)(THIS_ LPRECT) PURE;
    STDMETHOD(Blt)(THIS_ LPRECT,LPDIRECTDRAWSURFACE, LPRECT,DWORD, LPDDBLTFX) PURE;
    STDMETHOD(BltBatch)(THIS_ LPDDBLTBATCH, DWORD, DWORD ) PURE;
    STDMETHOD(BltFast)(THIS_ DWORD,DWORD,LPDIRECTDRAWSURFACE, LPRECT,DWORD) PURE;
    STDMETHOD(DeleteAttachedSurface)(THIS_ DWORD,LPDIRECTDRAWSURFACE) PURE;
    STDMETHOD(EnumAttachedSurfaces)(THIS_ LPVOID,LPDDENUMSURFACESCALLBACK) PURE;
    STDMETHOD(EnumOverlayZOrders)(THIS_ DWORD,LPVOID,LPDDENUMSURFACESCALLBACK) PURE;
    STDMETHOD(Flip)(THIS_ LPDIRECTDRAWSURFACE, DWORD) PURE;
    STDMETHOD(GetAttachedSurface)(THIS_ LPDDSCAPS, LPDIRECTDRAWSURFACE FAR *) PURE;
    STDMETHOD(GetBltStatus)(THIS_ DWORD) PURE;
    STDMETHOD(GetCaps)(THIS_ LPDDSCAPS) PURE;
    STDMETHOD(GetClipper)(THIS_ LPDIRECTDRAWCLIPPER FAR*) PURE;
    STDMETHOD(GetColorKey)(THIS_ DWORD, LPDDCOLORKEY) PURE;
    STDMETHOD(GetDC)(THIS_ HDC FAR *) PURE;
    STDMETHOD(GetFlipStatus)(THIS_ DWORD) PURE;
    STDMETHOD(GetOverlayPosition)(THIS_ LPLONG, LPLONG ) PURE;
    STDMETHOD(GetPalette)(THIS_ LPDIRECTDRAWPALETTE FAR*) PURE;
    STDMETHOD(GetPixelFormat)(THIS_ LPDDPIXELFORMAT) PURE;
    STDMETHOD(GetSurfaceDesc)(THIS_ LPDDSURFACEDESC) PURE;
    STDMETHOD(Initialize)(THIS_ LPDIRECTDRAW, LPDDSURFACEDESC) PURE;
    STDMETHOD(IsLost)(THIS) PURE;
    STDMETHOD(Lock)(THIS_ LPRECT,LPDDSURFACEDESC,DWORD,HANDLE) PURE;
    STDMETHOD(ReleaseDC)(THIS_ HDC) PURE;
    STDMETHOD(Restore)(THIS) PURE;
    STDMETHOD(SetClipper)(THIS_ LPDIRECTDRAWCLIPPER) PURE;
    STDMETHOD(SetColorKey)(THIS_ DWORD, LPDDCOLORKEY) PURE;
    STDMETHOD(SetOverlayPosition)(THIS_ LONG, LONG ) PURE;
    STDMETHOD(SetPalette)(THIS_ LPDIRECTDRAWPALETTE) PURE;
    STDMETHOD(Unlock)(THIS_ LPVOID) PURE;
    STDMETHOD(UpdateOverlay)(THIS_ LPRECT, LPDIRECTDRAWSURFACE,LPRECT,DWORD, LPDDOVERLAYFX) PURE;
    STDMETHOD(UpdateOverlayDisplay)(THIS_ DWORD) PURE;
    STDMETHOD(UpdateOverlayZOrder)(THIS_ DWORD, LPDIRECTDRAWSURFACE) PURE;
};

#if !defined(__cplusplus) || defined(CINTERFACE)
#define IDirectDrawSurface_QueryInterface(p,a,b)        (p)->lpVtbl->QueryInterface(p,a,b)
#define IDirectDrawSurface_AddRef(p)                    (p)->lpVtbl->AddRef(p)
#define IDirectDrawSurface_Release(p)                   (p)->lpVtbl->Release(p)
#define IDirectDrawSurface_AddAttachedSurface(p,a)      (p)->lpVtbl->AddAttachedSurface(p,a)
#define IDirectDrawSurface_AddOverlayDirtyRect(p,a)     (p)->lpVtbl->AddOverlayDirtyRect(p,a)
#define IDirectDrawSurface_Blt(p,a,b,c,d,e)             (p)->lpVtbl->Blt(p,a,b,c,d,e)
#define IDirectDrawSurface_BltBatch(p,a,b,c)            (p)->lpVtbl->BltBatch(p,a,b,c)
#define IDirectDrawSurface_BltFast(p,a,b,c,d,e)         (p)->lpVtbl->BltFast(p,a,b,c,d,e)
#define IDirectDrawSurface_DeleteAttachedSurface(p,a,b) (p)->lpVtbl->DeleteAttachedSurface(p,a,b)
#define IDirectDrawSurface_EnumAttachedSurfaces(p,a,b)  (p)->lpVtbl->EnumAttachedSurfaces(p,a,b)
#define IDirectDrawSurface_EnumOverlayZOrders(p,a,b,c)  (p)->lpVtbl->EnumOverlayZOrders(p,a,b,c)
#define IDirectDrawSurface_Flip(p,a,b)                  (p)->lpVtbl->Flip(p,a,b)
#define IDirectDrawSurface_GetAttachedSurface(p,a,b)    (p)->lpVtbl->GetAttachedSurface(p,a,b)
#define IDirectDrawSurface_GetBltStatus(p,a)            (p)->lpVtbl->GetBltStatus(p,a)
#define IDirectDrawSurface_GetCaps(p,b)                 (p)->lpVtbl->GetCaps(p,b)
#define IDirectDrawSurface_GetClipper(p,a)              (p)->lpVtbl->GetClipper(p,a)
#define IDirectDrawSurface_GetColorKey(p,a,b)           (p)->lpVtbl->GetColorKey(p,a,b)
#define IDirectDrawSurface_GetDC(p,a)                   (p)->lpVtbl->GetDC(p,a)
#define IDirectDrawSurface_GetFlipStatus(p,a)           (p)->lpVtbl->GetFlipStatus(p,a)
#define IDirectDrawSurface_GetOverlayPosition(p,a,b)    (p)->lpVtbl->GetOverlayPosition(p,a,b)
#define IDirectDrawSurface_GetPalette(p,a)              (p)->lpVtbl->GetPalette(p,a)
#define IDirectDrawSurface_GetPixelFormat(p,a)          (p)->lpVtbl->GetPixelFormat(p,a)
#define IDirectDrawSurface_GetSurfaceDesc(p,a)          (p)->lpVtbl->GetSurfaceDesc(p,a)
#define IDirectDrawSurface_Initialize(p,a,b)            (p)->lpVtbl->Initialize(p,a,b)
#define IDirectDrawSurface_IsLost(p)                    (p)->lpVtbl->IsLost(p)
#define IDirectDrawSurface_Lock(p,a,b,c,d)              (p)->lpVtbl->Lock(p,a,b,c,d)
#define IDirectDrawSurface_ReleaseDC(p,a)               (p)->lpVtbl->ReleaseDC(p,a)
#define IDirectDrawSurface_Restore(p)                   (p)->lpVtbl->Restore(p)
#define IDirectDrawSurface_SetClipper(p,a)              (p)->lpVtbl->SetClipper(p,a)
#define IDirectDrawSurface_SetColorKey(p,a,b)           (p)->lpVtbl->SetColorKey(p,a,b)
#define IDirectDrawSurface_SetOverlayPosition(p,a,b)    (p)->lpVtbl->SetOverlayPosition(p,a,b)
#define IDirectDrawSurface_SetPalette(p,a)              (p)->lpVtbl->SetPalette(p,a)
#define IDirectDrawSurface_Unlock(p,b)                  (p)->lpVtbl->Unlock(p,b)
#define IDirectDrawSurface_UpdateOverlay(p,a,b,c,d,e)   (p)->lpVtbl->UpdateOverlay(p,a,b,c,d,e)
#define IDirectDrawSurface_UpdateOverlayDisplay(p,a)    (p)->lpVtbl->UpdateOverlayDisplay(p,a)
#define IDirectDrawSurface_UpdateOverlayZOrder(p,a,b)   (p)->lpVtbl->UpdateOverlayZOrder(p,a,b)
#endif

#undef INTERFACE
#define INTERFACE IDirectDrawSurface2
DECLARE_INTERFACE_( IDirectDrawSurface2, IUnknown )
{
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID FAR * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;
    /*** IDirectDrawSurface methods ***/
    STDMETHOD(AddAttachedSurface)(THIS_ LPDIRECTDRAWSURFACE2) PURE;
    STDMETHOD(AddOverlayDirtyRect)(THIS_ LPRECT) PURE;
    STDMETHOD(Blt)(THIS_ LPRECT,LPDIRECTDRAWSURFACE2, LPRECT,DWORD, LPDDBLTFX) PURE;
    STDMETHOD(BltBatch)(THIS_ LPDDBLTBATCH, DWORD, DWORD ) PURE;
    STDMETHOD(BltFast)(THIS_ DWORD,DWORD,LPDIRECTDRAWSURFACE2, LPRECT,DWORD) PURE;
    STDMETHOD(DeleteAttachedSurface)(THIS_ DWORD,LPDIRECTDRAWSURFACE2) PURE;
    STDMETHOD(EnumAttachedSurfaces)(THIS_ LPVOID,LPDDENUMSURFACESCALLBACK) PURE;
    STDMETHOD(EnumOverlayZOrders)(THIS_ DWORD,LPVOID,LPDDENUMSURFACESCALLBACK) PURE;
    STDMETHOD(Flip)(THIS_ LPDIRECTDRAWSURFACE2, DWORD) PURE;
    STDMETHOD(GetAttachedSurface)(THIS_ LPDDSCAPS, LPDIRECTDRAWSURFACE2 FAR *) PURE;
    STDMETHOD(GetBltStatus)(THIS_ DWORD) PURE;
    STDMETHOD(GetCaps)(THIS_ LPDDSCAPS) PURE;
    STDMETHOD(GetClipper)(THIS_ LPDIRECTDRAWCLIPPER FAR*) PURE;
    STDMETHOD(GetColorKey)(THIS_ DWORD, LPDDCOLORKEY) PURE;
    STDMETHOD(GetDC)(THIS_ HDC FAR *) PURE;
    STDMETHOD(GetFlipStatus)(THIS_ DWORD) PURE;
    STDMETHOD(GetOverlayPosition)(THIS_ LPLONG, LPLONG ) PURE;
    STDMETHOD(GetPalette)(THIS_ LPDIRECTDRAWPALETTE FAR*) PURE;
    STDMETHOD(GetPixelFormat)(THIS_ LPDDPIXELFORMAT) PURE;
    STDMETHOD(GetSurfaceDesc)(THIS_ LPDDSURFACEDESC) PURE;
    STDMETHOD(Initialize)(THIS_ LPDIRECTDRAW, LPDDSURFACEDESC) PURE;
    STDMETHOD(IsLost)(THIS) PURE;
    STDMETHOD(Lock)(THIS_ LPRECT,LPDDSURFACEDESC,DWORD,HANDLE) PURE;
    STDMETHOD(ReleaseDC)(THIS_ HDC) PURE;
    STDMETHOD(Restore)(THIS) PURE;
    STDMETHOD(SetClipper)(THIS_ LPDIRECTDRAWCLIPPER) PURE;
    STDMETHOD(SetColorKey)(THIS_ DWORD, LPDDCOLORKEY) PURE;
    STDMETHOD(SetOverlayPosition)(THIS_ LONG, LONG ) PURE;
    STDMETHOD(SetPalette)(THIS_ LPDIRECTDRAWPALETTE) PURE;
    STDMETHOD(Unlock)(THIS_ LPVOID) PURE;
    STDMETHOD(UpdateOverlay)(THIS_ LPRECT, LPDIRECTDRAWSURFACE2,LPRECT,DWORD, LPDDOVERLAYFX) PURE;
    STDMETHOD(UpdateOverlayDisplay)(THIS_ DWORD) PURE;
    STDMETHOD(UpdateOverlayZOrder)(THIS_ DWORD, LPDIRECTDRAWSURFACE2) PURE;
    /*** Added in the v2 interface ***/
    STDMETHOD(GetDDInterface)(THIS_ LPVOID FAR *) PURE;
    STDMETHOD(PageLock)(THIS_ DWORD) PURE;
    STDMETHOD(PageUnlock)(THIS_ DWORD) PURE;
};

#if !defined(__cplusplus) || defined(CINTERFACE)
#define IDirectDrawSurface2_QueryInterface(p,a,b)        (p)->lpVtbl->QueryInterface(p,a,b)
#define IDirectDrawSurface2_AddRef(p)                    (p)->lpVtbl->AddRef(p)
#define IDirectDrawSurface2_Release(p)                   (p)->lpVtbl->Release(p)
#define IDirectDrawSurface2_AddAttachedSurface(p,a)      (p)->lpVtbl->AddAttachedSurface(p,a)
#define IDirectDrawSurface2_AddOverlayDirtyRect(p,a)     (p)->lpVtbl->AddOverlayDirtyRect(p,a)
#define IDirectDrawSurface2_Blt(p,a,b,c,d,e)             (p)->lpVtbl->Blt(p,a,b,c,d,e)
#define IDirectDrawSurface2_BltBatch(p,a,b,c)            (p)->lpVtbl->BltBatch(p,a,b,c)
#define IDirectDrawSurface2_BltFast(p,a,b,c,d,e)         (p)->lpVtbl->BltFast(p,a,b,c,d,e)
#define IDirectDrawSurface2_DeleteAttachedSurface(p,a,b) (p)->lpVtbl->DeleteAttachedSurface(p,a,b)
#define IDirectDrawSurface2_EnumAttachedSurfaces(p,a,b)  (p)->lpVtbl->EnumAttachedSurfaces(p,a,b)
#define IDirectDrawSurface2_EnumOverlayZOrders(p,a,b,c)  (p)->lpVtbl->EnumOverlayZOrders(p,a,b,c)
#define IDirectDrawSurface2_Flip(p,a,b)                  (p)->lpVtbl->Flip(p,a,b)
#define IDirectDrawSurface2_GetAttachedSurface(p,a,b)    (p)->lpVtbl->GetAttachedSurface(p,a,b)
#define IDirectDrawSurface2_GetBltStatus(p,a)            (p)->lpVtbl->GetBltStatus(p,a)
#define IDirectDrawSurface2_GetCaps(p,b)                 (p)->lpVtbl->GetCaps(p,b)
#define IDirectDrawSurface2_GetClipper(p,a)              (p)->lpVtbl->GetClipper(p,a)
#define IDirectDrawSurface2_GetColorKey(p,a,b)           (p)->lpVtbl->GetColorKey(p,a,b)
#define IDirectDrawSurface2_GetDC(p,a)                   (p)->lpVtbl->GetDC(p,a)
#define IDirectDrawSurface2_GetFlipStatus(p,a)           (p)->lpVtbl->GetFlipStatus(p,a)
#define IDirectDrawSurface2_GetOverlayPosition(p,a,b)    (p)->lpVtbl->GetOverlayPosition(p,a,b)
#define IDirectDrawSurface2_GetPalette(p,a)              (p)->lpVtbl->GetPalette(p,a)
#define IDirectDrawSurface2_GetPixelFormat(p,a)          (p)->lpVtbl->GetPixelFormat(p,a)
#define IDirectDrawSurface2_GetSurfaceDesc(p,a)          (p)->lpVtbl->GetSurfaceDesc(p,a)
#define IDirectDrawSurface2_Initialize(p,a,b)            (p)->lpVtbl->Initialize(p,a,b)
#define IDirectDrawSurface2_IsLost(p)                    (p)->lpVtbl->IsLost(p)
#define IDirectDrawSurface2_Lock(p,a,b,c,d)              (p)->lpVtbl->Lock(p,a,b,c,d)
#define IDirectDrawSurface2_ReleaseDC(p,a)               (p)->lpVtbl->ReleaseDC(p,a)
#define IDirectDrawSurface2_Restore(p)                   (p)->lpVtbl->Restore(p)
#define IDirectDrawSurface2_SetClipper(p,a)              (p)->lpVtbl->SetClipper(p,a)
#define IDirectDrawSurface2_SetColorKey(p,a,b)           (p)->lpVtbl->SetColorKey(p,a,b)
#define IDirectDrawSurface2_SetOverlayPosition(p,a,b)    (p)->lpVtbl->SetOverlayPosition(p,a,b)
#define IDirectDrawSurface2_SetPalette(p,a)              (p)->lpVtbl->SetPalette(p,a)
#define IDirectDrawSurface2_Unlock(p,b)                  (p)->lpVtbl->Unlock(p,b)
#define IDirectDrawSurface2_UpdateOverlay(p,a,b,c,d,e)   (p)->lpVtbl->UpdateOverlay(p,a,b,c,d,e)
#define IDirectDrawSurface2_UpdateOverlayDisplay(p,a)    (p)->lpVtbl->UpdateOverlayDisplay(p,a)
#define IDirectDrawSurface2_UpdateOverlayZOrder(p,a,b)   (p)->lpVtbl->UpdateOverlayZOrder(p,a,b)
#define IDirectDrawSurface2_GetDDInterface(p,a)          (p)->lpVtbl->GetDDInterface(p,a)
#define IDirectDrawSurface2_PageLock(p,a)                (p)->lpVtbl->PageLock(p,a)
#define IDirectDrawSurface2_PageUnlock(p,a)              (p)->lpVtbl->PageUnlock(p,a)
#endif


#endif


typedef struct _DDSURFACEDESC
{
    DWORD               dwSize;                 // size of the DDSURFACEDESC structure
    DWORD               dwFlags;                // determines what fields are valid
    DWORD               dwHeight;               // height of surface to be created
    DWORD               dwWidth;                // width of input surface
    LONG                lPitch;                 // distance to start of next line (return value only)
    DWORD               dwBackBufferCount;      // number of back buffers requested
    union
    {
        DWORD           dwMipMapCount;          // number of mip-map levels requested
        DWORD           dwZBufferBitDepth;      // depth of Z buffer requested
        DWORD           dwRefreshRate;          // refresh rate (used when display mode is described)
    };
    DWORD               dwAlphaBitDepth;        // depth of alpha buffer requested
    DWORD               dwReserved;             // reserved
    LPVOID              lpSurface;              // pointer to the associated surface memory
    DDCOLORKEY          ddckCKDestOverlay;      // color key for destination overlay use
    DDCOLORKEY          ddckCKDestBlt;          // color key for destination blt use
    DDCOLORKEY          ddckCKSrcOverlay;       // color key for source overlay use
    DDCOLORKEY          ddckCKSrcBlt;           // color key for source blt use
    DDPIXELFORMAT       ddpfPixelFormat;        // pixel format description of the surface
    DDSCAPS             ddsCaps;                // direct draw surface capabilities
} DDSURFACEDESC;

#define DDSD_CAPS               0x00000001l     // default

#define DDSD_HEIGHT             0x00000002l

#define DDSD_WIDTH              0x00000004l

#define DDSD_PITCH              0x00000008l

#define DDSD_BACKBUFFERCOUNT    0x00000020l

#define DDSD_ZBUFFERBITDEPTH    0x00000040l

#define DDSD_ALPHABITDEPTH      0x00000080l



#define DDSD_PIXELFORMAT        0x00001000l

#define DDSD_CKDESTOVERLAY      0x00002000l

#define DDSD_CKDESTBLT          0x00004000l

#define DDSD_CKSRCOVERLAY       0x00008000l

#define DDSD_CKSRCBLT           0x00010000l

#define DDSD_MIPMAPCOUNT        0x00020000l

 /*
  * dwRefreshRate is valid
  */
#define DDSD_REFRESHRATE        0x00040000l


#define DDSD_ALL                0x0007f9eel



#define DDSCAPS_3D                              0x00000001l

#define DDSCAPS_ALPHA                           0x00000002l

#define DDSCAPS_BACKBUFFER                      0x00000004l

#define DDSCAPS_COMPLEX                         0x00000008l

#define DDSCAPS_FLIP                            0x00000010l

#define DDSCAPS_FRONTBUFFER                     0x00000020l

#define DDSCAPS_OFFSCREENPLAIN                  0x00000040l

#define DDSCAPS_OVERLAY                         0x00000080l

#define DDSCAPS_PALETTE                         0x00000100l

#define DDSCAPS_PRIMARYSURFACE                  0x00000200l

#define DDSCAPS_PRIMARYSURFACELEFT              0x00000400l

#define DDSCAPS_SYSTEMMEMORY                    0x00000800l

#define DDSCAPS_TEXTURE                         0x00001000l

#define DDSCAPS_3DDEVICE                        0x00002000l

#define DDSCAPS_VIDEOMEMORY                     0x00004000l

#define DDSCAPS_VISIBLE                         0x00008000l

#define DDSCAPS_WRITEONLY                       0x00010000l

#define DDSCAPS_ZBUFFER                         0x00020000l

#define DDSCAPS_OWNDC                           0x00040000l

#define DDSCAPS_LIVEVIDEO                       0x00080000l

#define DDSCAPS_HWCODEC                         0x00100000l

#define DDSCAPS_MODEX                           0x00200000l

#define DDSCAPS_MIPMAP                          0x00400000l



#define DDSCAPS_ALLOCONLOAD                     0x04000000l



 /****************************************************************************
 *
 * DIRECTDRAW DRIVER CAPABILITY FLAGS
 *
 ****************************************************************************/

#define DDCAPS_3D                       0x00000001l

#define DDCAPS_ALIGNBOUNDARYDEST        0x00000002l

#define DDCAPS_ALIGNSIZEDEST            0x00000004l
#define DDCAPS_ALIGNBOUNDARYSRC         0x00000008l

#define DDCAPS_ALIGNSIZESRC             0x00000010l

#define DDCAPS_ALIGNSTRIDE              0x00000020l

#define DDCAPS_BLT                      0x00000040l

#define DDCAPS_BLTQUEUE                 0x00000080l

#define DDCAPS_BLTFOURCC                0x00000100l

#define DDCAPS_BLTSTRETCH               0x00000200l

#define DDCAPS_GDI                      0x00000400l

#define DDCAPS_OVERLAY                  0x00000800l

#define DDCAPS_OVERLAYCANTCLIP          0x00001000l

#define DDCAPS_OVERLAYFOURCC            0x00002000l

#define DDCAPS_OVERLAYSTRETCH           0x00004000l

#define DDCAPS_PALETTE                  0x00008000l

#define DDCAPS_PALETTEVSYNC             0x00010000l

#define DDCAPS_READSCANLINE             0x00020000l

#define DDCAPS_STEREOVIEW               0x00040000l

#define DDCAPS_VBI                      0x00080000l

#define DDCAPS_ZBLTS                    0x00100000l

#define DDCAPS_ZOVERLAYS                0x00200000l

#define DDCAPS_COLORKEY                 0x00400000l

#define DDCAPS_ALPHA                    0x00800000l

#define DDCAPS_COLORKEYHWASSIST         0x01000000l

#define DDCAPS_NOHARDWARE               0x02000000l

#define DDCAPS_BLTCOLORFILL             0x04000000l

#define DDCAPS_BANKSWITCHED             0x08000000l

#define DDCAPS_BLTDEPTHFILL             0x10000000l

#define DDCAPS_CANCLIP                  0x20000000l

#define DDCAPS_CANCLIPSTRETCHED         0x40000000l

#define DDCAPS_CANBLTSYSMEM             0x80000000l


 /****************************************************************************
 *
 * MORE DIRECTDRAW DRIVER CAPABILITY FLAGS (dwCaps2)
 *
 ****************************************************************************/

#define DDCAPS2_CERTIFIED               0x00000001l

#define DDCAPS2_NO2DDURING3DSCENE       0x00000002l


#define DDFXALPHACAPS_BLTALPHAEDGEBLEND         0x00000001l

#define DDFXALPHACAPS_BLTALPHAPIXELS            0x00000002l

#define DDFXALPHACAPS_BLTALPHAPIXELSNEG         0x00000004l

#define DDFXALPHACAPS_BLTALPHASURFACES          0x00000008l

#define DDFXALPHACAPS_BLTALPHASURFACESNEG       0x00000010l

#define DDFXALPHACAPS_OVERLAYALPHAEDGEBLEND     0x00000020l

#define DDFXALPHACAPS_OVERLAYALPHAPIXELS        0x00000040l

#define DDFXALPHACAPS_OVERLAYALPHAPIXELSNEG     0x00000080l

#define DDFXALPHACAPS_OVERLAYALPHASURFACES      0x00000100l

#define DDFXALPHACAPS_OVERLAYALPHASURFACESNEG   0x00000200l


#define DDFXCAPS_BLTARITHSTRETCHY       0x00000020l

#define DDFXCAPS_BLTARITHSTRETCHYN      0x00000010l

#define DDFXCAPS_BLTMIRRORLEFTRIGHT     0x00000040l

#define DDFXCAPS_BLTMIRRORUPDOWN        0x00000080l

#define DDFXCAPS_BLTROTATION            0x00000100l

#define DDFXCAPS_BLTROTATION90          0x00000200l

#define DDFXCAPS_BLTSHRINKX             0x00000400l

#define DDFXCAPS_BLTSHRINKXN            0x00000800l

#define DDFXCAPS_BLTSHRINKY             0x00001000l

#define DDFXCAPS_BLTSHRINKYN            0x00002000l

#define DDFXCAPS_BLTSTRETCHX            0x00004000l

#define DDFXCAPS_BLTSTRETCHXN           0x00008000l

#define DDFXCAPS_BLTSTRETCHY            0x00010000l

#define DDFXCAPS_BLTSTRETCHYN           0x00020000l

#define DDFXCAPS_OVERLAYARITHSTRETCHY   0x00040000l

#define DDFXCAPS_OVERLAYARITHSTRETCHYN  0x00000008l

#define DDFXCAPS_OVERLAYSHRINKX         0x00080000l

#define DDFXCAPS_OVERLAYSHRINKXN        0x00100000l

#define DDFXCAPS_OVERLAYSHRINKY         0x00200000l

#define DDFXCAPS_OVERLAYSHRINKYN        0x00400000l

#define DDFXCAPS_OVERLAYSTRETCHX        0x00800000l

#define DDFXCAPS_OVERLAYSTRETCHXN       0x01000000l

#define DDFXCAPS_OVERLAYSTRETCHY        0x02000000l

#define DDFXCAPS_OVERLAYSTRETCHYN       0x04000000l

#define DDFXCAPS_OVERLAYMIRRORLEFTRIGHT 0x08000000l

#define DDFXCAPS_OVERLAYMIRRORUPDOWN    0x10000000l


#define DDSVCAPS_ENIGMA                 0x00000001l

#define DDSVCAPS_FLICKER                0x00000002l

#define DDSVCAPS_REDBLUE                0x00000004l

#define DDSVCAPS_SPLIT                  0x00000008l


#define DDPCAPS_4BIT                    0x00000001l

#define DDPCAPS_8BITENTRIES             0x00000002l

#define DDPCAPS_8BIT                    0x00000004l

#define DDPCAPS_INITIALIZE              0x00000008l

#define DDPCAPS_PRIMARYSURFACE          0x00000010l

#define DDPCAPS_PRIMARYSURFACELEFT      0x00000020l

#define DDPCAPS_ALLOW256                0x00000040l

#define DDPCAPS_VSYNC                   0x00000080l

#define DDPCAPS_1BIT                    0x00000100l

#define DDPCAPS_2BIT                    0x00000200l





/* 0 is the only legal value */




#define DDBD_1                  0x00004000l

#define DDBD_2                  0x00002000l

#define DDBD_4                  0x00001000l

#define DDBD_8                  0x00000800l

#define DDBD_16                 0x00000400l

#define DDBD_24                 0X00000200l

#define DDBD_32                 0x00000100l


#define DDCKEY_COLORSPACE       0x00000001l

#define DDCKEY_DESTBLT          0x00000002l

#define DDCKEY_DESTOVERLAY      0x00000004l

#define DDCKEY_SRCBLT           0x00000008l

#define DDCKEY_SRCOVERLAY       0x00000010l



#define DDCKEYCAPS_DESTBLT                      0x00000001l

#define DDCKEYCAPS_DESTBLTCLRSPACE              0x00000002l

#define DDCKEYCAPS_DESTBLTCLRSPACEYUV           0x00000004l

#define DDCKEYCAPS_DESTBLTYUV                   0x00000008l

#define DDCKEYCAPS_DESTOVERLAY                  0x00000010l

#define DDCKEYCAPS_DESTOVERLAYCLRSPACE          0x00000020l

#define DDCKEYCAPS_DESTOVERLAYCLRSPACEYUV       0x00000040l

#define DDCKEYCAPS_DESTOVERLAYONEACTIVE         0x00000080l

#define DDCKEYCAPS_DESTOVERLAYYUV               0x00000100l

#define DDCKEYCAPS_SRCBLT                       0x00000200l

#define DDCKEYCAPS_SRCBLTCLRSPACE               0x00000400l

#define DDCKEYCAPS_SRCBLTCLRSPACEYUV            0x00000800l

#define DDCKEYCAPS_SRCBLTYUV                    0x00001000l

#define DDCKEYCAPS_SRCOVERLAY                   0x00002000l

#define DDCKEYCAPS_SRCOVERLAYCLRSPACE           0x00004000l

#define DDCKEYCAPS_SRCOVERLAYCLRSPACEYUV        0x00008000l

#define DDCKEYCAPS_SRCOVERLAYONEACTIVE          0x00010000l

#define DDCKEYCAPS_SRCOVERLAYYUV                0x00020000l

#define DDCKEYCAPS_NOCOSTOVERLAY                0x00040000l



#define DDPF_ALPHAPIXELS                        0x00000001l

#define DDPF_ALPHA                              0x00000002l

#define DDPF_FOURCC                             0x00000004l

#define DDPF_PALETTEINDEXED4                    0x00000008l

#define DDPF_PALETTEINDEXEDTO8                  0x00000010l

#define DDPF_PALETTEINDEXED8                    0x00000020l

#define DDPF_RGB                                0x00000040l

#define DDPF_COMPRESSED                         0x00000080l

#define DDPF_RGBTOYUV                           0x00000100l

#define DDPF_YUV                                0x00000200l

#define DDPF_ZBUFFER                            0x00000400l

#define DDPF_PALETTEINDEXED1                    0x00000800l

#define DDPF_PALETTEINDEXED2                    0x00001000l



#define DDENUMSURFACES_ALL                      0x00000001l

#define DDENUMSURFACES_MATCH                    0x00000002l

#define DDENUMSURFACES_NOMATCH                  0x00000004l

#define DDENUMSURFACES_CANBECREATED             0x00000008l

#define DDENUMSURFACES_DOESEXIST                0x00000010l



#define DDEDM_REFRESHRATES                      0x00000001l



#define DDSCL_FULLSCREEN                        0x00000001l

#define DDSCL_ALLOWREBOOT                       0x00000002l

#define DDSCL_NOWINDOWCHANGES                   0x00000004l

#define DDSCL_NORMAL                            0x00000008l

#define DDSCL_EXCLUSIVE                         0x00000010l


#define DDSCL_ALLOWMODEX                        0x00000040l



#define DDBLT_ALPHADEST                         0x00000001l

#define DDBLT_ALPHADESTCONSTOVERRIDE            0x00000002l

#define DDBLT_ALPHADESTNEG                      0x00000004l

#define DDBLT_ALPHADESTSURFACEOVERRIDE          0x00000008l

#define DDBLT_ALPHAEDGEBLEND                    0x00000010l

#define DDBLT_ALPHASRC                          0x00000020l

#define DDBLT_ALPHASRCCONSTOVERRIDE             0x00000040l

#define DDBLT_ALPHASRCNEG                       0x00000080l

#define DDBLT_ALPHASRCSURFACEOVERRIDE           0x00000100l

#define DDBLT_ASYNC                             0x00000200l

#define DDBLT_COLORFILL                         0x00000400l

#define DDBLT_DDFX                              0x00000800l

#define DDBLT_DDROPS                            0x00001000l

#define DDBLT_KEYDEST                           0x00002000l

#define DDBLT_KEYDESTOVERRIDE                   0x00004000l

#define DDBLT_KEYSRC                            0x00008000l

#define DDBLT_KEYSRCOVERRIDE                    0x00010000l

#define DDBLT_ROP                               0x00020000l

#define DDBLT_ROTATIONANGLE                     0x00040000l

#define DDBLT_ZBUFFER                           0x00080000l

#define DDBLT_ZBUFFERDESTCONSTOVERRIDE          0x00100000l

#define DDBLT_ZBUFFERDESTOVERRIDE               0x00200000l

#define DDBLT_ZBUFFERSRCCONSTOVERRIDE           0x00400000l

#define DDBLT_ZBUFFERSRCOVERRIDE                0x00800000l

#define DDBLT_WAIT                              0x01000000l

#define DDBLT_DEPTHFILL                         0x02000000l



#define DDBLTFAST_NOCOLORKEY                    0x00000000
#define DDBLTFAST_SRCCOLORKEY                   0x00000001
#define DDBLTFAST_DESTCOLORKEY                  0x00000002
#define DDBLTFAST_WAIT                          0x00000010


#define DDFLIP_WAIT                          0x00000001l



#define DDOVER_ALPHADEST                        0x00000001l

#define DDOVER_ALPHADESTCONSTOVERRIDE           0x00000002l

#define DDOVER_ALPHADESTNEG                     0x00000004l

#define DDOVER_ALPHADESTSURFACEOVERRIDE         0x00000008l

#define DDOVER_ALPHAEDGEBLEND                   0x00000010l

#define DDOVER_ALPHASRC                         0x00000020l

#define DDOVER_ALPHASRCCONSTOVERRIDE            0x00000040l

#define DDOVER_ALPHASRCNEG                      0x00000080l

#define DDOVER_ALPHASRCSURFACEOVERRIDE          0x00000100l

#define DDOVER_HIDE                             0x00000200l

#define DDOVER_KEYDEST                          0x00000400l

#define DDOVER_KEYDESTOVERRIDE                  0x00000800l

#define DDOVER_KEYSRC                           0x00001000l

#define DDOVER_KEYSRCOVERRIDE                   0x00002000l

#define DDOVER_SHOW                             0x00004000l

#define DDOVER_ADDDIRTYRECT                     0x00008000l

#define DDOVER_REFRESHDIRTYRECTS                0x00010000l

#define DDOVER_REFRESHALL                      0x00020000l


#define DDOVER_DDFX                             0x00080000l



#define DDLOCK_SURFACEMEMORYPTR                 0x00000000L     // default

#define DDLOCK_WAIT                             0x00000001L

#define DDLOCK_EVENT                            0x00000002L

#define DDLOCK_READONLY                         0x00000010L

#define DDLOCK_WRITEONLY                        0x00000020L









#define DDBLTFX_ARITHSTRETCHY                   0x00000001l

#define DDBLTFX_MIRRORLEFTRIGHT                 0x00000002l

#define DDBLTFX_MIRRORUPDOWN                    0x00000004l

#define DDBLTFX_NOTEARING                       0x00000008l

#define DDBLTFX_ROTATE180                       0x00000010l

#define DDBLTFX_ROTATE270                       0x00000020l

#define DDBLTFX_ROTATE90                        0x00000040l

#define DDBLTFX_ZBUFFERRANGE                    0x00000080l

#define DDBLTFX_ZBUFFERBASEDEST                 0x00000100l


#define DDOVERFX_ARITHSTRETCHY                  0x00000001l

#define DDOVERFX_MIRRORLEFTRIGHT                0x00000002l

#define DDOVERFX_MIRRORUPDOWN                   0x00000004l


#define DDWAITVB_BLOCKBEGIN                     0x00000001l

#define DDWAITVB_BLOCKBEGINEVENT                0x00000002l

#define DDWAITVB_BLOCKEND                       0x00000004l


#define DDGFS_CANFLIP                   0x00000001l

#define DDGFS_ISFLIPDONE                0x00000002l


#define DDGBS_CANBLT                    0x00000001l

#define DDGBS_ISBLTDONE                 0x00000002l



#define DDENUMOVERLAYZ_BACKTOFRONT      0x00000000l

#define DDENUMOVERLAYZ_FRONTTOBACK      0x00000001l


#define DDOVERZ_SENDTOFRONT             0x00000000l

#define DDOVERZ_SENDTOBACK              0x00000001l

#define DDOVERZ_MOVEFORWARD             0x00000002l

#define DDOVERZ_MOVEBACKWARD            0x00000003l

#define DDOVERZ_INSERTINFRONTOF         0x00000004l

#define DDOVERZ_INSERTINBACKOF          0x00000005l


#define DD_OK                                   0


#define DDENUMRET_CANCEL                        0

#define DDENUMRET_OK                            1


#define DDERR_ALREADYINITIALIZED                MAKE_DDHRESULT( 5 )

#define DDERR_CANNOTATTACHSURFACE               MAKE_DDHRESULT( 10 )

#define DDERR_CANNOTDETACHSURFACE               MAKE_DDHRESULT( 20 )

#define DDERR_CURRENTLYNOTAVAIL                 MAKE_DDHRESULT( 40 )

#define DDERR_EXCEPTION                         MAKE_DDHRESULT( 55 )

#define DDERR_GENERIC                           E_FAIL

#define DDERR_HEIGHTALIGN                       MAKE_DDHRESULT( 90 )

#define DDERR_INCOMPATIBLEPRIMARY               MAKE_DDHRESULT( 95 )

#define DDERR_INVALIDCAPS                       MAKE_DDHRESULT( 100 )

#define DDERR_INVALIDCLIPLIST                   MAKE_DDHRESULT( 110 )

#define DDERR_INVALIDMODE                       MAKE_DDHRESULT( 120 )

#define DDERR_INVALIDOBJECT                     MAKE_DDHRESULT( 130 )

#define DDERR_INVALIDPARAMS                     E_INVALIDARG

#define DDERR_INVALIDPIXELFORMAT                MAKE_DDHRESULT( 145 )

#define DDERR_INVALIDRECT                       MAKE_DDHRESULT( 150 )

#define DDERR_LOCKEDSURFACES                    MAKE_DDHRESULT( 160 )

#define DDERR_NO3D                              MAKE_DDHRESULT( 170 )

#define DDERR_NOALPHAHW                         MAKE_DDHRESULT( 180 )


#define DDERR_NOCLIPLIST                        MAKE_DDHRESULT( 205 )

#define DDERR_NOCOLORCONVHW                     MAKE_DDHRESULT( 210 )

#define DDERR_NOCOOPERATIVELEVELSET             MAKE_DDHRESULT( 212 )

#define DDERR_NOCOLORKEY                        MAKE_DDHRESULT( 215 )

#define DDERR_NOCOLORKEYHW                      MAKE_DDHRESULT( 220 )

#define DDERR_NODIRECTDRAWSUPPORT               MAKE_DDHRESULT( 222 )

#define DDERR_NOEXCLUSIVEMODE                   MAKE_DDHRESULT( 225 )

#define DDERR_NOFLIPHW                          MAKE_DDHRESULT( 230 )

#define DDERR_NOGDI                             MAKE_DDHRESULT( 240 )

#define DDERR_NOMIRRORHW                        MAKE_DDHRESULT( 250 )

#define DDERR_NOTFOUND                          MAKE_DDHRESULT( 255 )

#define DDERR_NOOVERLAYHW                       MAKE_DDHRESULT( 260 )

#define DDERR_NORASTEROPHW                      MAKE_DDHRESULT( 280 )

#define DDERR_NOROTATIONHW                      MAKE_DDHRESULT( 290 )

#define DDERR_NOSTRETCHHW                       MAKE_DDHRESULT( 310 )

#define DDERR_NOT4BITCOLOR                      MAKE_DDHRESULT( 316 )

#define DDERR_NOT4BITCOLORINDEX                 MAKE_DDHRESULT( 317 )

#define DDERR_NOT8BITCOLOR                      MAKE_DDHRESULT( 320 )

#define DDERR_NOTEXTUREHW                       MAKE_DDHRESULT( 330 )

#define DDERR_NOVSYNCHW                         MAKE_DDHRESULT( 335 )

#define DDERR_NOZBUFFERHW                       MAKE_DDHRESULT( 340 )

#define DDERR_NOZOVERLAYHW                      MAKE_DDHRESULT( 350 )

#define DDERR_OUTOFCAPS                         MAKE_DDHRESULT( 360 )

#define DDERR_OUTOFMEMORY                       E_OUTOFMEMORY

#define DDERR_OUTOFVIDEOMEMORY                  MAKE_DDHRESULT( 380 )

#define DDERR_OVERLAYCANTCLIP                   MAKE_DDHRESULT( 382 )

#define DDERR_OVERLAYCOLORKEYONLYONEACTIVE      MAKE_DDHRESULT( 384 )

#define DDERR_PALETTEBUSY                       MAKE_DDHRESULT( 387 )

#define DDERR_COLORKEYNOTSET                    MAKE_DDHRESULT( 400 )

#define DDERR_SURFACEALREADYATTACHED            MAKE_DDHRESULT( 410 )

#define DDERR_SURFACEALREADYDEPENDENT           MAKE_DDHRESULT( 420 )

#define DDERR_SURFACEBUSY                       MAKE_DDHRESULT( 430 )

#define DDERR_CANTLOCKSURFACE                   MAKE_DDHRESULT( 435 )

#define DDERR_SURFACEISOBSCURED                 MAKE_DDHRESULT( 440 )

#define DDERR_SURFACELOST                       MAKE_DDHRESULT( 450 )

#define DDERR_SURFACENOTATTACHED                MAKE_DDHRESULT( 460 )

#define DDERR_TOOBIGHEIGHT                      MAKE_DDHRESULT( 470 )

#define DDERR_TOOBIGSIZE                        MAKE_DDHRESULT( 480 )

#define DDERR_TOOBIGWIDTH                       MAKE_DDHRESULT( 490 )

#define DDERR_UNSUPPORTED                       E_NOTIMPL

#define DDERR_UNSUPPORTEDFORMAT                 MAKE_DDHRESULT( 510 )

#define DDERR_UNSUPPORTEDMASK                   MAKE_DDHRESULT( 520 )

#define DDERR_VERTICALBLANKINPROGRESS           MAKE_DDHRESULT( 537 )

#define DDERR_WASSTILLDRAWING                   MAKE_DDHRESULT( 540 )

#define DDERR_XALIGN                            MAKE_DDHRESULT( 560 )

#define DDERR_INVALIDDIRECTDRAWGUID             MAKE_DDHRESULT( 561 )

#define DDERR_DIRECTDRAWALREADYCREATED          MAKE_DDHRESULT( 562 )

#define DDERR_NODIRECTDRAWHW                    MAKE_DDHRESULT( 563 )

#define DDERR_PRIMARYSURFACEALREADYEXISTS       MAKE_DDHRESULT( 564 )

#define DDERR_NOEMULATION                       MAKE_DDHRESULT( 565 )

#define DDERR_REGIONTOOSMALL                    MAKE_DDHRESULT( 566 )

#define DDERR_CLIPPERISUSINGHWND                MAKE_DDHRESULT( 567 )

#define DDERR_NOCLIPPERATTACHED                 MAKE_DDHRESULT( 568 )

#define DDERR_NOHWND                            MAKE_DDHRESULT( 569 )

#define DDERR_HWNDSUBCLASSED                    MAKE_DDHRESULT( 570 )

#define DDERR_HWNDALREADYSET                    MAKE_DDHRESULT( 571 )

#define DDERR_NOPALETTEATTACHED                 MAKE_DDHRESULT( 572 )

#define DDERR_NOPALETTEHW                       MAKE_DDHRESULT( 573 )

#define DDERR_BLTFASTCANTCLIP                   MAKE_DDHRESULT( 574 )

#define DDERR_NOBLTHW                           MAKE_DDHRESULT( 575 )

#define DDERR_NODDROPSHW                        MAKE_DDHRESULT( 576 )

#define DDERR_OVERLAYNOTVISIBLE                 MAKE_DDHRESULT( 577 )

#define DDERR_NOOVERLAYDEST                     MAKE_DDHRESULT( 578 )

#define DDERR_INVALIDPOSITION                   MAKE_DDHRESULT( 579 )

#define DDERR_NOTAOVERLAYSURFACE                MAKE_DDHRESULT( 580 )
 
#define DDERR_EXCLUSIVEMODEALREADYSET           MAKE_DDHRESULT( 581 )

#define DDERR_NOTFLIPPABLE                      MAKE_DDHRESULT( 582 )

#define DDERR_CANTDUPLICATE                     MAKE_DDHRESULT( 583 )

#define DDERR_NOTLOCKED                         MAKE_DDHRESULT( 584 )

#define DDERR_CANTCREATEDC                      MAKE_DDHRESULT( 585 )

#define DDERR_NODC                              MAKE_DDHRESULT( 586 )

#define DDERR_WRONGMODE                         MAKE_DDHRESULT( 587 )

#define DDERR_IMPLICITLYCREATED                 MAKE_DDHRESULT( 588 )

#define DDERR_NOTPALETTIZED                     MAKE_DDHRESULT( 589 )


#define DDERR_UNSUPPORTEDMODE                   MAKE_DDHRESULT( 590 )

#define DDERR_NOMIPMAPHW                        MAKE_DDHRESULT( 591 )

#define DDERR_INVALIDSURFACETYPE                MAKE_DDHRESULT( 592 )



#define DDERR_DCALREADYCREATED                  MAKE_DDHRESULT( 620 )

#define DDERR_CANTPAGELOCK                      MAKE_DDHRESULT( 640 )

#define DDERR_CANTPAGEUNLOCK                    MAKE_DDHRESULT( 660 )

#define DDERR_NOTPAGELOCKED                     MAKE_DDHRESULT( 680 )

#define DDERR_NOTINITIALIZED                    CO_E_NOTINITIALIZED

/* Alpha bit depth constants */


#ifdef __cplusplus
};
#endif

#endif
