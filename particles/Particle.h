#pragma once
#include "..\utils\Color.h"
#include "..\math\math_types.h"
#include "..\settings\DynamicSettings.h"

namespace ds {
// -------------------------------------------------------
// Particle data
// -------------------------------------------------------
struct ParticleData : public DynamicSettings {

	Vec2 initialSize;
	float minRotationSpeed;
	float maxRotationSpeed;
	Rect textureRect;
	Color startColor;
	Color endColor;
	float random;
	bool reset;

	ParticleData() 
		: random(0.0f) , initialSize(Vec2(1,1)) , minRotationSpeed(0.0f) , maxRotationSpeed(0.0f) , startColor(Color(255,255,255,255)) , endColor(255,255,255,255) , textureRect(Rect(0,0,256,256)) {}

	ParticleData(const Rect& r,float initialSizeX = 1.0f,float initialSizeY = 1.0f,float rnd = 0.0f) 
		: textureRect(r) , initialSize(Vec2(initialSizeX,initialSizeY)) , random(rnd) , minRotationSpeed(0.0f) , maxRotationSpeed(0.0f) , startColor(Color(255,255,255,255)) , endColor(255,255,255,255) {
	}

	void load(NewSettingsReader& reader) {
		reader.get<Rect>("texture_rect",&textureRect);
		reader.get<Vec2>("initial_size",&initialSize);
		reader.get<float>("min_rotation_speed",&minRotationSpeed);
		reader.get<float>("max_rotation_speed",&maxRotationSpeed);
		reader.get<Color>("start_color",&startColor);		
		reader.get<Color>("end_color",&endColor);	
		reader.get<float>("random",&random);
	}
};

// -------------------------------------------------------
// Particle
// -------------------------------------------------------
struct Particle {
	Vec2 position;
	Vec2 normal;
	Vec2 initialSize;
	float ttl;
	float rotation;
	float rotationSpeed;
	float timer;	
	float radialVelocity;
	float random;
	Particle* next;
	Particle* prev;
	ParticleData* data;
};

}
