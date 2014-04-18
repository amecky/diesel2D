#pragma once
#include "..\utils\PlainTextReader.h"
#include "..\data\Gizmo.h"
#include "..\io\BinaryLoader.h"

namespace ds {

// -------------------------------------------------------
// Particle emitter data
// -------------------------------------------------------
struct ParticleEmitterData {

	uint32 ejectionPeriod;
	uint32 ejectionVariance;
	uint32 ejectionCounter;
	float velocity;
	float velocityVariance;
	float ttl;
	float ttlVariance;
	float sizeMinVariance;
	float sizeMaxVariance;
	uint32 count;

	ParticleEmitterData() : ejectionPeriod(0) , ejectionVariance(0) 
		, ejectionCounter(0) , velocity(0.0f) , velocityVariance(0.0f) 
		, ttl(1.0f) , ttlVariance(0.0f) , sizeMinVariance(0.0f) , sizeMaxVariance(0.0f) , count(1) {}

	void load(BinaryLoader* loader) {
		loader->read(&count);
		loader->read(&velocity);
		loader->read(&velocityVariance);
		loader->read(&ejectionPeriod);
		loader->read(&ejectionVariance);
		loader->read(&ejectionCounter);
		loader->read(&ttl);
		loader->read(&ttlVariance);
		loader->read(&sizeMinVariance);
		loader->read(&sizeMaxVariance);		
	}
};

// -------------------------------------------------------
// EmitterMesh
// -------------------------------------------------------
class EmitterMesh {

public:
	EmitterMesh() {}
	virtual ~EmitterMesh() {}
	virtual void getPoint(uint32 index,uint32 total,Vector2f& position,Vector2f& normal,float* angle,bool random = false) = 0;
};

// -------------------------------------------------------
// Ring emitter
// -------------------------------------------------------
struct RingEmitterSettings {

	int radius;
	int radiusVariance;
	float startAngle;
	float angleVariance;
	float startVariance;
	float normalVariance;

	RingEmitterSettings() : radius(10) ,  radiusVariance(2) , startAngle(0.0f) , angleVariance(0.0f) , startVariance(0.0f) , normalVariance(0.0f) {}

	void load(BinaryLoader* loader) {
		loader->read(&radius);
		loader->read(&angleVariance);
		loader->read(&startAngle);
		loader->read(&startVariance);
		loader->read(&radiusVariance);
		loader->read(&normalVariance);
	}

};

// -------------------------------------------------------
// RingEmitterMesh
// -------------------------------------------------------
class RingEmitterMesh : public EmitterMesh {

public:
	RingEmitterMesh(RingEmitterSettings* settings) : EmitterMesh() , m_Settings(settings) {}
	virtual ~RingEmitterMesh() {
		delete m_Settings;
	}
	void getPoint(uint32 index,uint32 total,Vector2f& position,Vector2f& normal,float* angle,bool random = false);
	RingEmitterSettings* getSettings() {
		return m_Settings;
	}
private:
	RingEmitterSettings* m_Settings;
};

// -------------------------------------------------------
// PointEmitterMesh
// -------------------------------------------------------
class PointEmitterMesh : public EmitterMesh {

public:
	PointEmitterMesh() : EmitterMesh() {}
	virtual ~PointEmitterMesh() {}
	void getPoint(uint32 index,uint32 total,Vector2f& position,Vector2f& normal,float* angle,bool random = false);
private:
	
};

// -------------------------------------------------------
// BoxEmitterSettings
// -------------------------------------------------------
struct BoxEmitterSettings : public Gizmo {

	int width;
	int height;
	int size;	
	float angleVariance;

	BoxEmitterSettings() : Gizmo("box_emitter") , width(10) , height(10) , size(2) , angleVariance(0.0f) {
		add("width",&width);
		add("height",&height);
		add("size",&size);
		add("angle_variance",&angleVariance);	
	}

};
// -------------------------------------------------------
// BoxEmitterMesh
// -------------------------------------------------------
class BoxEmitterMesh : public EmitterMesh {

struct BoxEntry {
	Vector2f pos;
	float angle;
	Vector2f normal;
};

public:
	BoxEmitterMesh(BoxEmitterSettings* settings);
	virtual ~BoxEmitterMesh() {
		delete m_Settings;
		delete[] m_Entries;
	}
	void getPoint(uint32 index,uint32 total,Vector2f& position,Vector2f& normal,float* angle,bool random = false);
	BoxEmitterSettings* getSettings() {
		return m_Settings;
	}
private:
	BoxEmitterSettings* m_Settings;
	BoxEntry* m_Entries;
	int m_Max;
};

// -------------------------------------------------------
// Cone emitter settings
// -------------------------------------------------------
struct ConeEmitterSettings : public Gizmo {

	int radius;
	int count;
	int radiusVariance;
	float startAngle;
	float angleVariance;
	float endAngle;

	ConeEmitterSettings() : Gizmo("cone_emitter") , radius(10) , radiusVariance(2) , startAngle(0.0f) , endAngle(180.0f) , angleVariance(0.0f) {
		add("radius",&radius);
		add("count",&count);
		add("radius_variance",&radiusVariance);
		add("start_angle",&startAngle);
		add("angle_variance",&angleVariance);
		add("end_angle",&endAngle);
	}

};
// -------------------------------------------------------
// ConeEmitterMesh
// -------------------------------------------------------
class ConeEmitterMesh : public EmitterMesh {

public:
	ConeEmitterMesh(ConeEmitterSettings* settings) : m_Settings(settings) {}
	virtual ~ConeEmitterMesh() {
		delete m_Settings;
	}
	void getPoint(uint32 index,uint32 total,Vector2f& position,Vector2f& normal,float* angle,bool random = false);
	ConeEmitterSettings* getSettings() {
		return m_Settings;
	}
	void setAngle(float startAngle,float endAngle) {
		m_Settings->startAngle = startAngle;
		m_Settings->endAngle = endAngle;
	}
private:
	ConeEmitterSettings* m_Settings;
	int m_Max;
};

}
