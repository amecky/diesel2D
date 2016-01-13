#pragma once
#include "..\utils\Color.h"
#include "..\math\math_types.h"
#include "..\renderer\vertex_types.h"

namespace ds {

	// -------------------------------------------------------
	// Particle array
	// -------------------------------------------------------
	struct ParticleArray {

		v3* position;
		v3* velocity;
		v3* acceleration;
		v3* normal;
		v2* scale;
		v2* baseScale;
		float* rotation;
		float* rotationVelocity;
		v3* timer;	
		Color* color;
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
			int size = maxParticles * ( sizeof(v3) * 4 + sizeof(v2) * 2 + sizeof(float) * 2 + sizeof(v3) + sizeof(Color) + 4 * sizeof(PTCVertex));
			buffer = new char[size];
			position = (Vector3f*)(buffer);
			velocity = (Vector3f*)(position + maxParticles);
			acceleration = (Vector3f*)(velocity + maxParticles);
			normal = (Vector3f*)(acceleration + maxParticles);
			scale = (Vector2f*)(normal + maxParticles);
			baseScale = (v2*)(scale + maxParticles);
			rotation = (float*)(baseScale + maxParticles);
			rotationVelocity = (float*)(rotation + maxParticles);
			timer = (Vector3f*)(rotationVelocity + maxParticles);
			color = (Color*)(timer + maxParticles);
			vertices = (PTCVertex*)(color + maxParticles);
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
				baseScale[a] = baseScale[b];
				rotation[a] = rotation[b];
				rotationVelocity[a] = rotationVelocity[b];
				timer[a] = timer[b];
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
