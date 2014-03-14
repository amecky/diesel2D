#pragma once
#include "ParticleEmitter.h"
#include "BoxEmitter.h"

namespace ds {

// -------------------------------------------------------
// EmitterMesh
// -------------------------------------------------------
class EmitterMesh {

public:
	EmitterMesh() {}
	virtual ~EmitterMesh() {}
	virtual void getPoint(uint32 index,uint32 total,Vector2f& position,Vector2f& normal,bool random = false) = 0;
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
	void getPoint(uint32 index,uint32 total,Vector2f& position,Vector2f& normal,bool random = false);
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
	void getPoint(uint32 index,uint32 total,Vector2f& position,Vector2f& normal,bool random = false);
private:
	
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
	void getPoint(uint32 index,uint32 total,Vector2f& position,Vector2f& normal,bool random = false);
	BoxEmitterSettings* getSettings() {
		return m_Settings;
	}
private:
	BoxEmitterSettings* m_Settings;
	BoxEntry* m_Entries;
	int m_Max;
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
	void getPoint(uint32 index,uint32 total,Vector2f& position,Vector2f& normal,bool random = false);
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
