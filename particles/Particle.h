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

struct ParticleSettings {

	Vector2f scale;
	Rect textureRect;
	Color color;
	float ttl;

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

	uint32 count;
	uint32 countAlive;

	ParticleArray() : count(0) , countAlive(0) {}

	~ParticleArray() {
		delete color;
		delete random;
		delete rotation;
		delete timer;
		delete scale;
		delete normal;
		delete velocity;
		delete position;
	}

	void initialize(unsigned int maxParticles) {
		position = new Vector2f[maxParticles];
		velocity = new Vector2f[maxParticles];
		acceleration = new Vector2f[maxParticles];
		normal = new Vector2f[maxParticles];
		scale = new Vector2f[maxParticles];
		timer = new Vector3f[maxParticles];
		rotation = new float[maxParticles];
		random = new float[maxParticles];
		color = new Color[maxParticles];
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

// -------------------------------------------------------
// Particle array
// -------------------------------------------------------
const int SIMPLE_PARTICLE_SIZE = 16;

struct SimpleParticleArray {

	float* data;

	uint32 count;
	uint32 countAlive;

	SimpleParticleArray() : count(0), countAlive(0) {}

	~SimpleParticleArray() {
		delete[] data;
	}

	void initialize(unsigned int maxParticles) {
		data = new float[maxParticles * SIMPLE_PARTICLE_SIZE];
		count = maxParticles;
		countAlive = 0;
	}

	void swapData(uint32 a, uint32 b) {
		if (a != b) {
			int oa = a * SIMPLE_PARTICLE_SIZE;
			int ob = b * SIMPLE_PARTICLE_SIZE;
			for (int i = 0; i < SIMPLE_PARTICLE_SIZE; ++i) {
				data[oa + i] = data[ob + i];
			}
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

const int PARTICLE_MAX_CHANNELS = 8;
/*
0 = position
1 = velocity
2 = scale
3 = rotation
4 = timer / ttl / norm time
5 = color
6 = uv
7 = dimension
*/
class NParticleArray {

public:
	NParticleArray(int size) : m_Size(size), m_Count(0) {
		m_Data = new Vector4f[size * PARTICLE_MAX_CHANNELS];
	}
	~NParticleArray() {
		delete[] m_Data;
	}
	Vector4f* begin(int channel) const {
		return m_Data + channel * m_Size;
	}
	Vector4f* end(int channel) const {
		return m_Data + channel * m_Size + m_Count;
	}

	void kill(int id) {
		if (m_Count > 0) {
			swapData(id, m_Count - 1);
			--m_Count;
		}
	}
	void wake(int id) {
		if (m_Count < m_Size) {
			swapData(id, m_Count);
			++m_Count;
		}
	}
	const int size() const {
		return m_Count;
	}
	const int maxSize() const {
		return m_Size;
	}
private:
	void swapData(int a, int b) {
		if (a != b) {
			Vector4f* pa = m_Data + a;
			Vector4f* pb = m_Data + b;
			for (int i = 0; i < PARTICLE_MAX_CHANNELS; ++i) {
				*pa = *pb;
				pb += m_Size;
				pa += m_Size;
			}
		}
	}

	int m_Count;
	int m_Size;
	Vector4f* m_Data;

};
