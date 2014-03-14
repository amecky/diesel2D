#pragma once
#include "..\utils\Color.h"
#include "..\math\math_types.h"
#include "..\settings\DynamicSettings.h"
#include "..\utils\PlainTextReader.h"
#include "..\data\Gizmo.h"

namespace ds {
// -------------------------------------------------------
// Particle data
// -------------------------------------------------------
struct ParticleData : public Gizmo {

	Vector2f initialSize;
	float minRotationSpeed;
	float maxRotationSpeed;
	Rect textureRect;
	Color startColor;
	Color endColor;
	float random;
	bool reset;

	ParticleData() 
		: Gizmo("particle_data" ) , random(0.0f) , initialSize(Vector2f(1,1)) , minRotationSpeed(0.0f) 
		, maxRotationSpeed(0.0f) , startColor(Color(255,255,255,255)) , endColor(255,255,255,255) 
		, textureRect(Rect(0,0,256,256)) {
		add("min_rotation_speed",&minRotationSpeed);
		add("max_rotation_speed",&maxRotationSpeed);
		add("random",&random);
		add("texture_rect",&textureRect);
		add("initial_size",&initialSize);
		add("start_color",&startColor);		
		add("end_color",&endColor);		
	}	
};

// -------------------------------------------------------
// Particle
// -------------------------------------------------------
struct Particle {
	Vector2f position;
	Vector2f normal;
	Vector2f initialSize;
	Vector2f prevPos;
	float ttl;
	float rotation;
	float rotationSpeed;
	float timer;	
	float radialVelocity;
	float random;
	bool head;
	Particle* next;
	Particle* prev;
	ParticleData* data;
};

}
