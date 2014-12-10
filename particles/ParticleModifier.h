#pragma once
#include "Particle.h"
#include "..\utils\PlainTextReader.h"
#include "..\io\BinaryWriter.h"
#include "..\data\DataTranslator.h"

namespace ds {

// -------------------------------------------------------
// Particle modifier
// -------------------------------------------------------
class ParticleModifier {

public:
	ParticleModifier() {}
	virtual ~ParticleModifier() {}
	virtual void update(ParticleArray* array,float dt) = 0;
	virtual void convert(Category* category,BinaryWriter& writer) {} 
	virtual void load(BinaryLoader* loader) {}
};

// -------------------------------------------------------
// Abstract particle modifier
// -------------------------------------------------------
template<class DATA>
class AbstractParticleModifier : public ParticleModifier {

public:
	AbstractParticleModifier() : ParticleModifier() {}
	virtual ~AbstractParticleModifier() {}

	DataTranslator<DATA>& getTranslator() {
		return m_Translator;
	}

	void convert(Category* category,BinaryWriter& writer) {
		DATA data;
		DataTranslator<DATA>& translator = getTranslator();
		translator.read(category,&data);
		translator.saveChunk(writer,1,&data,true);
	}

	virtual void load(BinaryLoader* loader) {
		DataTranslator<DATA>& translator = getTranslator();
		translator.readChunk(*loader,&m_Data);
	}

protected:
	DataTranslator<DATA> m_Translator;
	DATA m_Data;
};

// -------------------------------------------------------
// ParticlePositionModifier
// -------------------------------------------------------
class ParticlePositionModifier : public ParticleModifier {

public:
	ParticlePositionModifier() : ParticleModifier() {}
	virtual ~ParticlePositionModifier() {}

	void update(ParticleArray* array,float dt) {
		for ( uint32 i = 0; i < array->countAlive; ++i ) {
			array->velocity[i] += array->acceleration[i] * dt;
			array->position[i] += array->velocity[i] * dt;
		}
	}

};

// -------------------------------------------------------
// ParticleTimeModifier
// -------------------------------------------------------
class ParticleTimeModifier : public ParticleModifier {

public:
	ParticleTimeModifier() : ParticleModifier() {}
	virtual ~ParticleTimeModifier() {}

	void update(ParticleArray* array,float dt) {
		for ( uint32 i = 0; i < array->countAlive; ++i ) {
			array->timer[i].x += dt;
			array->timer[i].y = array->timer[i].x / array->timer[i].z;
		}
		uint32 cnt = 0;
		while ( cnt < array->countAlive) {
			if ( array->timer[cnt].x > array->timer[cnt].z ) {
				array->kill(cnt);
			}
			++cnt;
		}
	}
};

// -------------------------------------------------------
// Linear color modifier
// -------------------------------------------------------
struct LinearColorModifierData {

	Color startColor;
	Color endColor;

	LinearColorModifierData() : startColor(255,255,255,255) , endColor(0,0,0,0) {}
};

class LinearColorModifier : public AbstractParticleModifier<LinearColorModifierData> {

public:
	LinearColorModifier() : AbstractParticleModifier<LinearColorModifierData>() {
		m_Translator.add("start_color",&LinearColorModifierData::startColor);
		m_Translator.add("end_color",&LinearColorModifierData::endColor);
	}
	virtual ~LinearColorModifier() {}
	void init(const Color& start,const Color& end) {
		m_Data.startColor = start;
		m_Data.endColor = end;
	}
	void update(ParticleArray* array,float dt) {
		for ( uint32 i = 0; i < array->countAlive; ++i ) {
			array->color[i] = color::lerp(m_Data.startColor,m_Data.endColor,array->timer[i].y);			
		}
	}
};

// -------------------------------------------------------
// Linear size modifier
// -------------------------------------------------------
struct LinearSizeModifierData {
	
	Vector2f minScale;
	Vector2f maxScale;

	LinearSizeModifierData() : minScale(0,0) , maxScale(1,1) {}
};

class LinearSizeModifier : public AbstractParticleModifier<LinearSizeModifierData> {

public:
	LinearSizeModifier() : AbstractParticleModifier<LinearSizeModifierData>() {
		m_Translator.add("min_scale",&LinearSizeModifierData::minScale);
		m_Translator.add("max_scale",&LinearSizeModifierData::maxScale);
	}
	virtual ~LinearSizeModifier() {}
	void init(const Vector2f& minScale,const Vector2f& maxScale) {
		m_Data.minScale = minScale;
		m_Data.maxScale = maxScale;
	}
	void update(ParticleArray* array,float dt) {
		for ( uint32 i = 0; i < array->countAlive; ++i ) {			
			array->scale[i] = lerp(m_Data.minScale,m_Data.maxScale,array->timer[i].y);
		}
	}
};

// -------------------------------------------------------
// PerpendicularMoveModifier
// -------------------------------------------------------
struct PerpendicularMoveModifierData {
	
	float radius;
	float amplitude;

	PerpendicularMoveModifierData() : radius(10.0f) , amplitude(1.0f) {}
};

class PerpendicularMoveModifier : public AbstractParticleModifier<PerpendicularMoveModifierData> {

public:
	PerpendicularMoveModifier() : AbstractParticleModifier<PerpendicularMoveModifierData>() {
		m_Translator.add("radius",&PerpendicularMoveModifierData::radius);
		m_Translator.add("amplitude",&PerpendicularMoveModifierData::amplitude);
	}

	virtual ~PerpendicularMoveModifier() {}
	void init(float radius,float amplitude) {
		m_Data.radius = radius;
		m_Data.amplitude = amplitude;
	}
	void update(ParticleArray* array,float dt) {
		for ( uint32 i = 0; i < array->countAlive; ++i ) {	
			Vector2f perp = Vector2f(-array->velocity[i].y,array->velocity[i].x);
			perp = normalize(perp);
			perp = perp * (sin(array->timer[i].x * m_Data.amplitude * array->random[i]) * m_Data.radius * array->random[i]);// * p->random;	
			array->position[i] += perp * dt;
		}
	}
};

// -------------------------------------------------------
// ColorPathModifier
// -------------------------------------------------------
struct ColorPathModifierData {

	ColorPath path;

};

class ColorPathModifier : public AbstractParticleModifier<ColorPathModifierData> {

public:
	ColorPathModifier() : AbstractParticleModifier<ColorPathModifierData>() {
		m_Translator.add("path",&ColorPathModifierData::path);
	}
	virtual ~ColorPathModifier() {}
	void add(float time,const Color& clr) {
		m_Data.path.add(time,clr);
	}
	void update(ParticleArray* array,float dt) {
		for ( uint32 i = 0; i < array->countAlive; ++i ) {
			m_Data.path.update(array->timer[i].y,&array->color[i]);			
		}
	}
};

// -------------------------------------------------------
// AlphaPathModifier
// -------------------------------------------------------
struct AlphaPathModifierData {

	FloatArray path;

};

class AlphaPathModifier : public AbstractParticleModifier<AlphaPathModifierData> {

public:
	AlphaPathModifier() : AbstractParticleModifier<AlphaPathModifierData>() {
		m_Translator.add("path", &AlphaPathModifierData::path);
	}
	virtual ~AlphaPathModifier() {}
	void add(float time, float a) {
		m_Data.path.add(time, a);
	}
	void update(ParticleArray* array, float dt) {
		for (uint32 i = 0; i < array->countAlive; ++i) {
			array->color[i].a = m_Data.path.get(array->timer[i].y) / 255.0f;
		}
	}
};

// -------------------------------------------------------
// DampingVelocityModifier
// -------------------------------------------------------
struct DampingVelocityModifierData {

	float damping;

};

class DampingVelocityModifier : public AbstractParticleModifier<DampingVelocityModifierData> {

public:
	DampingVelocityModifier() : AbstractParticleModifier<DampingVelocityModifierData>() {
		m_Translator.add("damping",&DampingVelocityModifierData::damping);
	}
	virtual ~DampingVelocityModifier() {}
	void init(float damping) {
		m_Data.damping = damping;
	}
	void update(ParticleArray* array,float dt) {
		float d = m_Data.damping * 0.01f;
		for ( uint32 i = 0; i < array->countAlive; ++i ) {
			Vector2f v = array->velocity[i] * d;
			array->velocity[i] -= v * dt;
		}
	}
};

// -------------------------------------------------------
// SizePathModifier
// -------------------------------------------------------
struct SizePathModifierData {

	Vector2fPath path;

};

class SizePathModifier : public AbstractParticleModifier<SizePathModifierData> {

public:
	SizePathModifier() : AbstractParticleModifier<SizePathModifierData>() {
		m_Translator.add("path",&SizePathModifierData::path);
	}
	virtual ~SizePathModifier() {}
	void add(float time,const Vector2f& v) {
		m_Data.path.add(time,v);
	}
	void update(ParticleArray* array,float dt) {
		for ( uint32 i = 0; i < array->countAlive; ++i ) {
			m_Data.path.update(array->timer[i].y,&array->scale[i]);			
		}
	}
};

// -------------------------------------------------------
// VelocityRotationModifier
// -------------------------------------------------------
class VelocityRotationModifier : public ParticleModifier {

public:
	VelocityRotationModifier() : ParticleModifier() {}
	virtual ~VelocityRotationModifier() {}

	void update(ParticleArray* array,float dt) {
		for ( uint32 i = 0; i < array->countAlive; ++i ) {
			array->rotation[i] = math::getTargetAngle(V2_RIGHT,array->velocity[i]);			
		}
	}
};

}
