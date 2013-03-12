#pragma once
#include "..\utils\Color.h"
#include "..\utils\GameMath.h"

namespace ds {
// -------------------------------------------------------
// Particle data
// -------------------------------------------------------
class ParticleData {

public:
	ParticleData() {}
	~ParticleData() {}
	float initialSize;
	float minSize;
	float maxSize;	
	Rect textureRect;
	float ttl;
	Color startColor;
	Color endColor;
};

// -------------------------------------------------------
// Particle
// -------------------------------------------------------
struct Particle {
	uint32 index;
	Vec2 startPos;
	Vec2 velocity;
	Vec2 acceleration;
	float sizeX;
	float sizeY;
	float rotation;
	float timer;
	Particle* next;
	ParticleData* data;
	Color color;
};

}
