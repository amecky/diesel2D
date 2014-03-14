#pragma once
#include <Vector.h>
#include "..\utils\Color.h"
#include "..\math\math_types.h"

namespace ds {

struct SpriteData {

	Vector2f position;
	Vector2f velocity;
	int index;
	float radius;
	float angle;
	Vector2f size;
	Color color;
	bool active;
	float timer;
	float delay;
	//Rect textureRect;
	float u1;
	float u2;
	float v1;
	float v2;
	float dimX;
	float dimY;
	int id;

	SpriteData() : position(0.0f,0.0f) , velocity(0.0f,0.0f) 
		, index(0) , radius(0.0f) , angle(0.0f) , color(255,255,255,255) 
		, size(1.0f,1.0f) , active(true) , timer(0.0f) , delay(0.0f) , id(0) 
		, u1(0.0f) , v1(0.0f) , u2(1.0f) , v2(1.0f) , dimX(10.0f) , dimY(10.0f) {}
};

}