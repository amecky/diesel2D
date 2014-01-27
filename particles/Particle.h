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

	Vec2 initialSize;
	float minRotationSpeed;
	float maxRotationSpeed;
	Rect textureRect;
	Color startColor;
	Color endColor;
	float random;
	bool reset;

	ParticleData() 
		: Gizmo("particle_data" ) , random(0.0f) , initialSize(Vec2(1,1)) , minRotationSpeed(0.0f) , maxRotationSpeed(0.0f) , startColor(Color(255,255,255,255)) , endColor(255,255,255,255) , textureRect(Rect(0,0,256,256)) {
		add("min_rotation_speed",&minRotationSpeed);
		add("max_rotation_speed",&maxRotationSpeed);
		add("random",&random);
		add("texture_rect",&textureRect);
		add("initial_size",&initialSize);
		add("start_color",&startColor);		
		add("end_color",&endColor);		
	}

	ParticleData(const Rect& r,float initialSizeX = 1.0f,float initialSizeY = 1.0f,float rnd = 0.0f) 
		: Gizmo("particle_data" ) , textureRect(r) , initialSize(Vec2(initialSizeX,initialSizeY)) , random(rnd) , minRotationSpeed(0.0f) , maxRotationSpeed(0.0f) , startColor(Color(255,255,255,255)) , endColor(255,255,255,255) {
		add("min_rotation_speed",&minRotationSpeed);
		add("max_rotation_speed",&maxRotationSpeed);
		add("random",&random);
		add("texture_rect",&textureRect);
		add("initial_size",&initialSize);
		add("start_color",&startColor);		
		add("end_color",&endColor);			
	}
	/*
	void load(NewSettingsReader& reader) {
		reader.get<Rect>("texture_rect",&textureRect);
		reader.get<Vec2>("initial_size",&initialSize);
		reader.get<float>("min_rotation_speed",&minRotationSpeed);
		reader.get<float>("max_rotation_speed",&maxRotationSpeed);
		reader.get<Color>("start_color",&startColor);		
		reader.get<Color>("end_color",&endColor);	
		reader.get<float>("random",&random);
	}
	*/
	/*
	void load(Category* category) {
		textureRect = category->getRect("texture_rect");
		initialSize = category->getVec2("initial_size");
		category->getColor("start_color",&startColor);
		category->getColor("end_color",&endColor);
		category->getFloat("min_rotation_speed",&minRotationSpeed);
		category->getFloat("max_rotation_speed",&maxRotationSpeed);
		category->getFloat("random",&random);
	}
	*/
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
