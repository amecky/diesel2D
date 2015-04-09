#pragma once
#include "..\utils\Color.h"
#include "..\math\math_types.h"
#include "..\utils\PlainTextReader.h"
#include "..\io\BinaryLoader.h"
#include "..\renderer\vertex_types.h"

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

struct ParticleSettings {

	Vector2f scale;
	Rect textureRect;
	Color color;
	float ttl;

};

// -------------------------------------------------------
// Particle array
// -------------------------------------------------------
struct ParticleArray {

	Vector2f* position;
	Vector2f* velocity;
	Vector2f* acceleration;
	Vector2f* normal;
	Vector2f* scale;
	float* rotation;
	Vector3f* timer;	
	float* random;
	Color* color;
	int* type;
	PTCVertex* vertices;
	char* buffer;

	uint32 count;
	uint32 countAlive;

	ParticleArray() : count(0) , countAlive(0) , buffer(0) {}

	~ParticleArray() {		
		if ( buffer != 0 ) {
			delete[] buffer;
		}
	}

	void initialize(unsigned int maxParticles) {
		int size = maxParticles * ( sizeof(Vector2f) * 5 + sizeof(float) + sizeof(Vector3f) + sizeof(float) + sizeof(Color) + sizeof(int) + 4 * sizeof(PTCVertex));
		buffer = new char[size];
		position = (Vector2f*)(buffer);
		velocity = (Vector2f*)(position + maxParticles);
		acceleration = (Vector2f*)(velocity + maxParticles);
		normal = (Vector2f*)(acceleration + maxParticles);
		scale = (Vector2f*)(normal + maxParticles);
		timer = (Vector3f*)(scale + maxParticles);
		rotation = (float*)(timer + maxParticles);
		random = (float*)(rotation + maxParticles);
		color = (Color*)(random + maxParticles);
		type = (int*)(color + maxParticles);
		vertices = (PTCVertex*)(type + maxParticles);
		count = maxParticles;
		countAlive = 0;
	}

	void swapData(uint32 a, uint32 b) {
		if ( a != b ) {
			position[a] = position[b];
			velocity[a] = velocity[b];
			acceleration[a] = acceleration[b];
			normal[a] = normal[b];
			scale[a] = scale[b];
			rotation[a] = rotation[b];
			timer[a] = timer[b];
			random[a] = random[b];
			color[a] = color[b];
		}
	}

	void kill(uint32 id) {
		if (countAlive > 0) {
			swapData(id, countAlive - 1);
			--countAlive;
		}
	}

	void wake(uint32 id) {
		if (countAlive < count)	{
			swapData(id, countAlive);
			++countAlive;
		}
	}   

};

}
