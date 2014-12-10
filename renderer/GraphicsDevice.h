#pragma once
#include "..\dxstdafx.h"
#include "..\base\Settings.h"
#include <string>
#include "Renderer.h"

namespace ds {

namespace graphics {

	void initializeDevice(const Settings& settings);

	std::string D3DFormatToString( D3DFORMAT format, bool bWithPrefix );

	void writeInfo(const Settings& settings,const std::string& info);
}

};

