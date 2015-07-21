#pragma once
#include "..\math\math_types.h"
#include "Vector.h"
#include "..\utils\Color.h"
#include "..\renderer\render_types.h"

namespace ds {

	typedef unsigned int SID;
	const unsigned int INVALID_SID = UINT_MAX;

	struct Sprite {

		SID id;
		Vector2f position;
		Vector2f scale;
		float rotation;
		Texture texture;
		Color color;
		int type;
		AABBox box;

		Sprite() : id(0) , position(0,0) , scale(1,1) , rotation(0.0f) , color(Color::WHITE) , type(0) {}

	};
	
}