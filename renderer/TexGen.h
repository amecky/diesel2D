#pragma once
#include "Renderer.h"
#include "..\pipeline\PAKReader.h"

namespace ds {

	namespace texture {

		int loadRawTexture(Renderer* renderer,const char* fileName);

		int readI(PAKReader& reader);

		void saveRawTexture(Renderer* renderer,int id,const char* fileName);
	}

}
