

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0497 */
/* Compiler settings for dxgitype.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

/* verify that the <rpcsal.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCSAL_H_VERSION__
#define __REQUIRED_RPCSAL_H_VERSION__ 100
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__


#ifndef __dxgitype_h__
#define __dxgitype_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


/* interface __MIDL_itf_dxgitype_0000_0000 */
/* [local] */ 

#define _FACDXGI    0x87a
#define MAKE_DXGI_HRESULT( code )    MAKE_HRESULT( 1, _FACDXGI, code )
#define MAKE_DXGI_STATUS( code )    MAKE_HRESULT( 0, _FACDXGI, code )
#define DXGI_STATUS_OCCLUDED                MAKE_DXGI_STATUS(1 )
#define DXGI_STATUS_CLIPPED                 MAKE_DXGI_STATUS(2 )
#define DXGI_STATUS_NO_REDIRECTION          MAKE_DXGI_STATUS(4 )
#define DXGI_STATUS_NO_DESKTOP_ACCESS       MAKE_DXGI_STATUS(5 )
#define DXGI_STATUS_GRAPHICS_VIDPN_SOURCE_IN_USE       MAKE_DXGI_STATUS(6 )
#define DXGI_STATUS_MODE_CHANGED            MAKE_DXGI_STATUS(7 )
#define DXGI_ERROR_INVALID_CALL    MAKE_DXGI_HRESULT( 1 )
#define DXGI_ERROR_NOT_FOUND    MAKE_DXGI_HRESULT( 2 )
#define DXGI_ERROR_MORE_DATA    MAKE_DXGI_HRESULT( 3 )
#define DXGI_ERROR_UNSUPPORTED    MAKE_DXGI_HRESULT( 4 )
#define DXGI_ERROR_DEVICE_REMOVED    MAKE_DXGI_HRESULT( 5 )
#define DXGI_ERROR_DEVICE_HUNG    MAKE_DXGI_HRESULT( 6 )
#define DXGI_ERROR_DEVICE_RESET    MAKE_DXGI_HRESULT( 7 )
#define DXGI_ERROR_WAS_STILL_DRAWING    MAKE_DXGI_HRESULT( 10 )
#define DXGI_ERROR_FRAME_STATISTICS_DISJOINT MAKE_DXGI_HRESULT( 11 )
#define DXGI_ERROR_DRIVER_INTERNAL_ERROR    MAKE_DXGI_HRESULT( 32 )
#define DXGI_ERROR_NONEXCLUSIVE    MAKE_DXGI_HRESULT( 33 )
#define DXGI_FORMAT_DEFINED 1
typedef 
enum DXGI_FORMAT
    {	DXGI_FORMAT_UNKNOWN	= 0,
	DXGI_FORMAT_R32G32B32A32_TYPELESS	= 1,
	DXGI_FORMAT_R32G32B32A32_FLOAT	= 2,
	DXGI_FORMAT_R32G32B32A32_UINT	= 3,
	DXGI_FORMAT_R32G32B32A32_SINT	= 4,
	DXGI_FORMAT_R32G32B32_TYPELESS	= 5,
	DXGI_FORMAT_R32G32B32_FLOAT	= 6,
	DXGI_FORMAT_R32G32B32_UINT	= 7,
	DXGI_FORMAT_R32G32B32_SINT	= 8,
	DXGI_FORMAT_R16G16B16A16_TYPELESS	= 9,
	DXGI_FORMAT_R16G16B16A16_FLOAT	= 10,
	DXGI_FORMAT_R16G16B16A16_UNORM	= 11,
	DXGI_FORMAT_R16G16B16A16_UINT	= 12,
	DXGI_FORMAT_R16G16B16A16_SNORM	= 13,
	DXGI_FORMAT_R16G16B16A16_SINT	= 14,
	DXGI_FORMAT_R32G32_TYPELESS	= 15,
	DXGI_FORMAT_R32G32_FLOAT	= 16,
	DXGI_FORMAT_R32G32_UINT	= 17,
	DXGI_FORMAT_R32G32_SINT	= 18,
	DXGI_FORMAT_R32G8X24_TYPELESS	= 19,
	DXGI_FORMAT_D32_FLOAT_S8X24_UINT	= 20,
	DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS	= 21,
	DXGI_FORMAT_X32_TYPELESS_G8X24_UINT	= 22,
	DXGI_FORMAT_R10G10B10A2_TYPELESS	= 23,
	DXGI_FORMAT_R10G10B10A2_UNORM	= 24,
	DXGI_FORMAT_R10G10B10A2_UINT	= 25,
	DXGI_FORMAT_R11G11B10_FLOAT	= 26,
	DXGI_FORMAT_R8G8B8A8_TYPELESS	= 27,
	DXGI_FORMAT_R8G8B8A8_UNORM	= 28,
	DXGI_FORMAT_R8G8B8A8_UNORM_SRGB	= 29,
	DXGI_FORMAT_R8G8B8A8_UINT	= 30,
	DXGI_FORMAT_R8G8B8A8_SNORM	= 31,
	DXGI_FORMAT_R8G8B8A8_SINT	= 32,
	DXGI_FORMAT_R16G16_TYPELESS	= 33,
	DXGI_FORMAT_R16G16_FLOAT	= 34,
	DXGI_FORMAT_R16G16_UNORM	= 35,
	DXGI_FORMAT_R16G16_UINT	= 36,
	DXGI_FORMAT_R16G16_SNORM	= 37,
	DXGI_FORMAT_R16G16_SINT	= 38,
	DXGI_FORMAT_R32_TYPELESS	= 39,
	DXGI_FORMAT_D32_FLOAT	= 40,
	DXGI_FORMAT_R32_FLOAT	= 41,
	DXGI_FORMAT_R32_UINT	= 42,
	DXGI_FORMAT_R32_SINT	= 43,
	DXGI_FORMAT_R24G8_TYPELESS	= 44,
	DXGI_FORMAT_D24_UNORM_S8_UINT	= 45,
	DXGI_FORMAT_R24_UNORM_X8_TYPELESS	= 46,
	DXGI_FORMAT_X24_TYPELESS_G8_UINT	= 47,
	DXGI_FORMAT_R8G8_TYPELESS	= 48,
	DXGI_FORMAT_R8G8_UNORM	= 49,
	DXGI_FORMAT_R8G8_UINT	= 50,
	DXGI_FORMAT_R8G8_SNORM	= 51,
	DXGI_FORMAT_R8G8_SINT	= 52,
	DXGI_FORMAT_R16_TYPELESS	= 53,
	DXGI_FORMAT_R16_FLOAT	= 54,
	DXGI_FORMAT_D16_UNORM	= 55,
	DXGI_FORMAT_R16_UNORM	= 56,
	DXGI_FORMAT_R16_UINT	= 57,
	DXGI_FORMAT_R16_SNORM	= 58,
	DXGI_FORMAT_R16_SINT	= 59,
	DXGI_FORMAT_R8_TYPELESS	= 60,
	DXGI_FORMAT_R8_UNORM	= 61,
	DXGI_FORMAT_R8_UINT	= 62,
	DXGI_FORMAT_R8_SNORM	= 63,
	DXGI_FORMAT_R8_SINT	= 64,
	DXGI_FORMAT_A8_UNORM	= 65,
	DXGI_FORMAT_R1_UNORM	= 66,
	DXGI_FORMAT_R9G9B9E5_SHAREDEXP	= 67,
	DXGI_FORMAT_R8G8_B8G8_UNORM	= 68,
	DXGI_FORMAT_G8R8_G8B8_UNORM	= 69,
	DXGI_FORMAT_BC1_TYPELESS	= 70,
	DXGI_FORMAT_BC1_UNORM	= 71,
	DXGI_FORMAT_BC1_UNORM_SRGB	= 72,
	DXGI_FORMAT_BC2_TYPELESS	= 73,
	DXGI_FORMAT_BC2_UNORM	= 74,
	DXGI_FORMAT_BC2_UNORM_SRGB	= 75,
	DXGI_FORMAT_BC3_TYPELESS	= 76,
	DXGI_FORMAT_BC3_UNORM	= 77,
	DXGI_FORMAT_BC3_UNORM_SRGB	= 78,
	DXGI_FORMAT_BC4_TYPELESS	= 79,
	DXGI_FORMAT_BC4_UNORM	= 80,
	DXGI_FORMAT_BC4_SNORM	= 81,
	DXGI_FORMAT_BC5_TYPELESS	= 82,
	DXGI_FORMAT_BC5_UNORM	= 83,
	DXGI_FORMAT_BC5_SNORM	= 84,
	DXGI_FORMAT_B5G6R5_UNORM	= 85,
	DXGI_FORMAT_B5G5R5A1_UNORM	= 86,
	DXGI_FORMAT_B8G8R8A8_UNORM	= 87,
	DXGI_FORMAT_B8G8R8X8_UNORM	= 88,
	DXGI_FORMAT_FORCE_UINT	= 0xffffffffUL
    } 	DXGI_FORMAT;

typedef struct DXGI_RGB
    {
    float Red;
    float Green;
    float Blue;
    } 	DXGI_RGB;

typedef struct DXGI_GAMMA_CONTROL
    {
    DXGI_RGB Scale;
    DXGI_RGB Offset;
    DXGI_RGB GammaCurve[ 1025 ];
    } 	DXGI_GAMMA_CONTROL;

typedef struct DXGI_GAMMA_CONTROL_CAPABILITIES
    {
    BOOL ScaleAndOffsetSupported;
    float MaxConvertedValue;
    float MinConvertedValue;
    UINT NumGammaControlPoints;
    float ControlPointPositions[ 1025 ];
    } 	DXGI_GAMMA_CONTROL_CAPABILITIES;

typedef struct DXGI_RATIONAL
    {
    UINT Numerator;
    UINT Denominator;
    } 	DXGI_RATIONAL;

typedef 
enum DXGI_MODE_SCANLINE_ORDER
    {	DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED	= 0,
	DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE	= 1,
	DXGI_MODE_SCANLINE_ORDER_UPPER_FIELD_FIRST	= 2,
	DXGI_MODE_SCANLINE_ORDER_LOWER_FIELD_FIRST	= 3
    } 	DXGI_MODE_SCANLINE_ORDER;

typedef 
enum DXGI_MODE_SCALING
    {	DXGI_MODE_SCALING_UNSPECIFIED	= 0,
	DXGI_MODE_SCALING_CENTERED	= 1,
	DXGI_MODE_SCALING_STRETCHED	= 2
    } 	DXGI_MODE_SCALING;

typedef 
enum DXGI_MODE_ROTATION
    {	DXGI_MODE_ROTATION_UNSPECIFIED	= 0,
	DXGI_MODE_ROTATION_IDENTITY	= 1,
	DXGI_MODE_ROTATION_ROTATE90	= 2,
	DXGI_MODE_ROTATION_ROTATE180	= 3,
	DXGI_MODE_ROTATION_ROTATE270	= 4
    } 	DXGI_MODE_ROTATION;

typedef struct DXGI_MODE_DESC
    {
    UINT Width;
    UINT Height;
    DXGI_RATIONAL RefreshRate;
    DXGI_FORMAT Format;
    DXGI_MODE_SCANLINE_ORDER ScanlineOrdering;
    DXGI_MODE_SCALING Scaling;
    } 	DXGI_MODE_DESC;

typedef struct DXGI_SAMPLE_DESC
    {
    UINT Count;
    UINT Quality;
    } 	DXGI_SAMPLE_DESC;



extern RPC_IF_HANDLE __MIDL_itf_dxgitype_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_dxgitype_0000_0000_v0_0_s_ifspec;

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


