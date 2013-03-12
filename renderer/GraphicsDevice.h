#pragma once
#include "..\dxstdafx.h"
#include "..\base\Settings.h"
#include <string>

namespace ds {

class GraphicsDevice {

public:
	GraphicsDevice(HWND hWnd,const Settings& settings);
	~GraphicsDevice();
	int getWidth() { return width;}
	int getHeight() { return height;}
	LPDIRECT3DDEVICE9 get() { return pd3dDevice;}
	D3DFORMAT getFormat() { return format;}
private:
	void writeInfo(const Settings& settings,const std::string& info);
	std::string D3DFormatToString( D3DFORMAT format, bool bWithPrefix );

	LPDIRECT3DDEVICE9 pd3dDevice; // Our rendering device
	LPDIRECT3D9 pD3D;	
	D3DCAPS9 m_DeviceCaps;
	int width;
	int height;
	D3DFORMAT format;
};

};

