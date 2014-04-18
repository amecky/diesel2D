#pragma once
#include "..\utils\Color.h"
#include "..\math\math_types.h"
#include "..\utils\PlainTextReader.h"
#include "..\io\BinaryLoader.h"

namespace ds {
// -------------------------------------------------------
// Particle data
// -------------------------------------------------------
struct ParticleData {

	Vector2f initialSize;
	float minRotationSpeed;
	float maxRotationSpeed;
	Rect textureRect;
	Color startColor;
	Color endColor;
	float random;
	bool reset;

	ParticleData() : random(0.0f) , initialSize(Vector2f(1,1)) , minRotationSpeed(0.0f) 
		, maxRotationSpeed(0.0f) , startColor(Color(255,255,255,255)) , endColor(255,255,255,255) 
		, textureRect(Rect(0,0,256,256)) {}	

	void load(BinaryLoader* loader) {
		loader->read(&textureRect);
		loader->read(&initialSize);
		loader->read(&startColor);
		loader->read(&endColor);
		loader->read(&minRotationSpeed);
		loader->read(&maxRotationSpeed);
		loader->read(&random);		
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
