#include "GraphicsDevice.h"
#include "..\utils\Log.h"

namespace ds {

GraphicsDevice::GraphicsDevice(HWND hWnd,const Settings& settings) {
	// Create the D3D object.
    pD3D = Direct3DCreate9(D3D_SDK_VERSION);

	D3DDISPLAYMODE d3ddm;
	HR(pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm));

	pD3D->GetDeviceCaps( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &m_DeviceCaps );
	
	
    // Set up the structure used to create the D3DDevice
    D3DPRESENT_PARAMETERS m_pp;
    ZeroMemory( &m_pp, sizeof( m_pp ) );	
	D3DFORMAT adapterFormat = D3DFMT_X8R8G8B8;
    if ( SUCCEEDED( pD3D->CheckDeviceFormat( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, adapterFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D24S8 ) ) ) {
        m_pp.AutoDepthStencilFormat = D3DFMT_D24S8;
    }
    else if ( SUCCEEDED( pD3D->CheckDeviceFormat( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, adapterFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D24X8 ) ) ) {
        m_pp.AutoDepthStencilFormat = D3DFMT_D24X8;
    }
    else if ( SUCCEEDED( pD3D->CheckDeviceFormat( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, adapterFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D16 ) ) ) {
        m_pp.AutoDepthStencilFormat = D3DFMT_D16;
    }
	m_pp.BackBufferWidth        = settings.width;
	m_pp.BackBufferHeight       = settings.height;
    m_pp.BackBufferFormat       = adapterFormat;
    m_pp.BackBufferCount        = 1;
	DWORD total;

	if(SUCCEEDED(pD3D->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,d3ddm.Format,true,D3DMULTISAMPLE_NONMASKABLE,&total))) {
		m_pp.MultiSampleType = D3DMULTISAMPLE_NONMASKABLE;
		m_pp.MultiSampleQuality = total - 1;
		LOG(logINFO) << "multi sample is supported - quality level " << total -1;
	}
	else {
		LOG(logINFO) << "NO multi sample is supported";
		m_pp.MultiSampleType        = D3DMULTISAMPLE_NONE;
		m_pp.MultiSampleQuality     = 0;
	}
    m_pp.SwapEffect             = D3DSWAPEFFECT_DISCARD; 
    m_pp.hDeviceWindow          = hWnd;
	if ( settings.fullscreen ) {
		m_pp.Windowed = false;
	}
	else {
		m_pp.Windowed = true;
	}
    m_pp.EnableAutoDepthStencil = TRUE; 
	m_pp.AutoDepthStencilFormat = D3DFMT_D16;	
	if ( settings.synched ) {
		m_pp.PresentationInterval   = D3DPRESENT_INTERVAL_DEFAULT;
	}
	else {
		m_pp.PresentationInterval   = D3DPRESENT_INTERVAL_IMMEDIATE;
	}
	//m_pp.FullScreen_RefreshRateInHz = (Windowed) ? 0 : m_displayMode.RefreshRate;

	DWORD vertexProcessing = 0;
    if ( m_DeviceCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )
    {
        vertexProcessing = D3DCREATE_HARDWARE_VERTEXPROCESSING;
		// Check for pure device
		if ( m_DeviceCaps.DevCaps & D3DDEVCAPS_PUREDEVICE )
		{
			vertexProcessing |= D3DCREATE_PUREDEVICE;
		}
    }
    else
    {
        vertexProcessing = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    }

    // Create the D3DDevice
    HR(pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,vertexProcessing,&m_pp, &pd3dDevice ));

	format = d3ddm.Format;
	std::string info = D3DFormatToString(format,true);    
	HR(pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR));
	HR(pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR));
	width = settings.width;
	height = settings.height;
	writeInfo(settings,info);
	
}


GraphicsDevice::~GraphicsDevice() {
	LOG(logINFO) << "Releasing GraphicsDevice";
	if( pd3dDevice != NULL ) {
        pd3dDevice->Release();
	}
	if( pD3D != NULL ) {
        pD3D->Release();
	}
}

void GraphicsDevice::writeInfo(const Settings& settings,const std::string& info) {
	LOG(logINFO) << "Hardware informations";
	LOG(logINFO) << "Maximum primitives: " << m_DeviceCaps.MaxPrimitiveCount;
	LOG(logINFO) << "Vertex shader version: " <<((m_DeviceCaps.VertexShaderVersion>>8)&0xFF) << "." << ((m_DeviceCaps.VertexShaderVersion>>0)&0xFF);
	LOG(logINFO) << "Pixel shader version: " << ((m_DeviceCaps.PixelShaderVersion>>8)&0xFF) << "." << ((m_DeviceCaps.PixelShaderVersion>>0)&0xFF);
	LOG(logINFO) << "Settings";
	LOG(logINFO) << "Format: " << info;
	LOG(logINFO) << "Width: " << settings.width;
	LOG(logINFO) << "Height " << settings.height;
	LOG(logINFO) << "Mode: " << settings.mode;
	LOG(logINFO) << "Synched: " << settings.synched;
}

// ---------------------------------------------------
// translate the format into string
// ---------------------------------------------------
std::string GraphicsDevice::D3DFormatToString( D3DFORMAT format, bool bWithPrefix ) {
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

};
