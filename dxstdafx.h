#pragma once
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
//#define STRICT
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <assert.h>
#include <mmsystem.h>
#include <new.h>      // for placement new
#include <math.h>      
#include <memory>
// Enable extra D3D debugging in debug builds if using the debug DirectX runtime.  
// This makes D3D objects work well in the debugger watch window, but slows down 
// performance slightly.
//#if defined(DEBUG) | defined(_DEBUG)
//#ifndef D3D_DEBUG_INFO
//#define D3D_DEBUG_INFO
//#endif
//#endif

// Direct3D includes
#include <d3d9.h>
#include <d3dx9.h>
#include <dxerr.h>

// DirectSound includes
#include <mmsystem.h>
#include <mmreg.h>
#include <dsound.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include "base\CrashReporter.h"

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if ( (p) && (p) != NULL) { (p)->Release(); (p)=NULL; } }
#endif
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }

const float PI =  3.1415926535897932384626433832795f;
const float TWO_PI = 2.0f * PI;
const float HALF_PI = 0.5f * PI;
const float TWO_PI_INV = 1.0f / TWO_PI;

#define DEGTORAD( degree ) ((degree) * (PI / 180.0f))
#define RADTODEG( radian ) ((radian) * (180.0f / PI))

#define _SCL_SECURE_NO_WARNINGS

// ---------------------------------------
// type definitions
// ---------------------------------------
typedef signed char int8;
typedef signed short int16;
typedef signed int int32;
typedef signed __int64 int64;
typedef unsigned char uint8;
typedef unsigned char uchar;
typedef unsigned char uchar32;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned __int64 uint64;
typedef float f32;
typedef double f64;
typedef unsigned int IdString;
typedef unsigned long ulong;
typedef unsigned short ushort;

//const uint32 UINT32_MAX = 0xffffffffu;

#if defined(RELEASE) 
	#ifndef HR
#define HR(x) { HRESULT hr = x;	if ( FAILED(hr) ) { DXTraceA(__FILE__ ,__LINE__ ,hr,#x,true); 	}} 
	#endif
#else
	#ifndef HR
		//#define HR(x) x;
#define HR(x) { HRESULT hr = x ; if ( FAILED(hr) ) { char desc[1024]; sprintf_s(desc,1024, "(DX) %s - %s",DXGetErrorString(hr),DXGetErrorDescription(hr));	char buf[2048];	sprintf_s(buf,"%s(%d) : Error: %s\n", __FILE__, __LINE__, desc);LOGE << buf; }}
	#endif
#endif

#ifndef XASSERT
#define XASSERT(cond, s, ...) do { if (!(cond)) {\
	char buf[2048]; \
	sprintf_s(buf,2048,">> ERROR: " s "\n", ##__VA_ARGS__); \
	LOGE << buf; \
	ds::gCrashDump->dump(); \
	assert(cond); \
} } while (false)
#endif

#ifndef BIT
#define BIT( num ) ( 1 << ( num ) )
#endif

#ifndef DBG_V2
#define DBG_V2(v) "x: " << v.x << " y: " << v.y
#endif

#ifndef DBG_V3
#define DBG_V3(v) "x: " << v.x << " y: " << v.y << " z: " << v.z
#endif

#ifndef DBG_V4
#define DBG_V4(v) "x: " << v.x << " y: " << v.y << " z: " << v.z << " w: " << v.w
#endif

#ifndef DBG_RECT
#define DBG_RECT(v) "top: " << v.top << " left: " << v.left << " width: " << v.width() << " height: " << v.height()
#endif

#ifndef DBG_CLR
#define DBG_CLR(v) "r: " << v.r << " g: " << v.g << " b: " << v.b << " a: " << v.a
#endif

#ifndef DBG_TEX
#define DBG_TEX(v) "dim.x: " << v.dim.x << " dim.y: " << v.dim.y << " u1: " << v.uv.x << " v1: " << v.uv.y << " u2: " << v.uv.z << " v2: " << v.uv.w
#endif

#ifndef DBG_PNT
#define DBG_PNT(v) "x: " << v.x << " y: " << v.y
#endif

#ifndef BM_NEW
#define BM_NEW(x) new(gBlockMemory->alloc(sizeof(x)))x();
#endif

#ifndef CALL_MEMBER_FN
#define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember))
#endif

#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
	TypeName(const TypeName&);               \
	void operator=(const TypeName&)
#endif