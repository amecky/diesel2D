#include "GraphicsDevice.h"
#include "..\utils\Log.h"

namespace ds {

namespace graphics {

	// ---------------------------------------------------
	// translate the format into string
	// ---------------------------------------------------
	std::string D3DFormatToString( D3DFORMAT format, bool bWithPrefix ) {
		std::string pstr = "UNKNOWN";
		switch( format ) {
		case D3DFMT_UNKNOWN:         pstr = "D3DFMT_UNKNOWN"; break;
		case D3DFMT_R8G8B8:          pstr = "D3DFMT_R8G8B8"; break;
		case D3DFMT_A8R8G8B8:        pstr = "D3DFMT_A8R8G8B8"; break;
		case D3DFMT_X8R8G8B8:        pstr = "D3DFMT_X8R8G8B8"; break;
		case D3DFMT_R5G6B5:          pstr = "D3DFMT_R5G6B5"; break;
		case D3DFMT_X1R5G5B5:        pstr = "D3DFMT_X1R5G5B5"; break;
		case D3DFMT_A1R5G5B5:        pstr = "D3DFMT_A1R5G5B5"; break;
		case D3DFMT_A4R4G4B4:        pstr = "D3DFMT_A4R4G4B4"; break;
		case D3DFMT_R3G3B2:          pstr = "D3DFMT_R3G3B2"; break;
		case D3DFMT_A8:              pstr = "D3DFMT_A8"; break;
		case D3DFMT_A8R3G3B2:        pstr = "D3DFMT_A8R3G3B2"; break;
		case D3DFMT_X4R4G4B4:        pstr = "D3DFMT_X4R4G4B4"; break;
		case D3DFMT_A2B10G10R10:     pstr = "D3DFMT_A2B10G10R10"; break;
		case D3DFMT_A8B8G8R8:        pstr = "D3DFMT_A8B8G8R8"; break;
		case D3DFMT_X8B8G8R8:        pstr = "D3DFMT_X8B8G8R8"; break;
		case D3DFMT_G16R16:          pstr = "D3DFMT_G16R16"; break;
		case D3DFMT_A2R10G10B10:     pstr = "D3DFMT_A2R10G10B10"; break;
		case D3DFMT_A16B16G16R16:    pstr = "D3DFMT_A16B16G16R16"; break;
		case D3DFMT_A8P8:            pstr = "D3DFMT_A8P8"; break;
		case D3DFMT_P8:              pstr = "D3DFMT_P8"; break;
		case D3DFMT_L8:              pstr = "D3DFMT_L8"; break;
		case D3DFMT_A8L8:            pstr = "D3DFMT_A8L8"; break;
		case D3DFMT_A4L4:            pstr = "D3DFMT_A4L4"; break;
		case D3DFMT_V8U8:            pstr = "D3DFMT_V8U8"; break;
		case D3DFMT_L6V5U5:          pstr = "D3DFMT_L6V5U5"; break;
		case D3DFMT_X8L8V8U8:        pstr = "D3DFMT_X8L8V8U8"; break;
		case D3DFMT_Q8W8V8U8:        pstr = "D3DFMT_Q8W8V8U8"; break;
		case D3DFMT_V16U16:          pstr = "D3DFMT_V16U16"; break;
		case D3DFMT_A2W10V10U10:     pstr = "D3DFMT_A2W10V10U10"; break;
		case D3DFMT_UYVY:            pstr = "D3DFMT_UYVY"; break;
		case D3DFMT_YUY2:            pstr = "D3DFMT_YUY2"; break;
		case D3DFMT_DXT1:            pstr = "D3DFMT_DXT1"; break;
		case D3DFMT_DXT2:            pstr = "D3DFMT_DXT2"; break;
		case D3DFMT_DXT3:            pstr = "D3DFMT_DXT3"; break;
		case D3DFMT_DXT4:            pstr = "D3DFMT_DXT4"; break;
		case D3DFMT_DXT5:            pstr = "D3DFMT_DXT5"; break;
		case D3DFMT_D16_LOCKABLE:    pstr = "D3DFMT_D16_LOCKABLE"; break;
		case D3DFMT_D32:             pstr = "D3DFMT_D32"; break;
		case D3DFMT_D15S1:           pstr = "D3DFMT_D15S1"; break;
		case D3DFMT_D24S8:           pstr = "D3DFMT_D24S8"; break;
		case D3DFMT_D24X8:           pstr = "D3DFMT_D24X8"; break;
		case D3DFMT_D24X4S4:         pstr = "D3DFMT_D24X4S4"; break;
		case D3DFMT_D16:             pstr = "D3DFMT_D16"; break;
		case D3DFMT_L16:             pstr = "D3DFMT_L16"; break;
		case D3DFMT_VERTEXDATA:      pstr = "D3DFMT_VERTEXDATA"; break;
		case D3DFMT_INDEX16:         pstr = "D3DFMT_INDEX16"; break;
		case D3DFMT_INDEX32:         pstr = "D3DFMT_INDEX32"; break;
		case D3DFMT_Q16W16V16U16:    pstr = "D3DFMT_Q16W16V16U16"; break;
		case D3DFMT_MULTI2_ARGB8:    pstr = "D3DFMT_MULTI2_ARGB8"; break;
		case D3DFMT_R16F:            pstr = "D3DFMT_R16F"; break;
		case D3DFMT_G16R16F:         pstr = "D3DFMT_G16R16F"; break;
		case D3DFMT_A16B16G16R16F:   pstr = "D3DFMT_A16B16G16R16F"; break;
		case D3DFMT_R32F:            pstr = "D3DFMT_R32F"; break;
		case D3DFMT_G32R32F:         pstr = "D3DFMT_G32R32F"; break;
		case D3DFMT_A32B32G32R32F:   pstr = "D3DFMT_A32B32G32R32F"; break;
		case D3DFMT_CxV8U8:          pstr = "D3DFMT_CxV8U8"; break;
		default:                     pstr = "Unknown format"; break;
		}
		return pstr;

	}
}

};
