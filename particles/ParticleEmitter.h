#pragma once
#include "..\utils\PlainTextReader.h"
#include "..\io\BinaryLoader.h"
#include "Particle.h"
#include "..\io\BinaryWriter.h"
#include "..\data\DataTranslator.h"
#include "EmitterMesh.h"

namespace ds {

// -------------------------------------------------------
// Particle generator
// -------------------------------------------------------
class ParticleGenerator {

public:
	ParticleGenerator() {}
	virtual ~ParticleGenerator() {}
	virtual void generate(ParticleArray* array,const Vector2f& position,float dt,uint32 start,uint32 end) = 0;
	virtual void convert(Category* category,BinaryWriter& writer) {} 
	virtual void load(BinaryLoader* loader) {}
};

// -------------------------------------------------------
// Abstract particle generator
// -------------------------------------------------------
template<class DATA>
class AbstractParticleGenerator : public ParticleGenerator {

public:
	AbstractParticleGenerator() : ParticleGenerator() {}
	virtual ~AbstractParticleGenerator() {}

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

class DefaultParticleGenerator : public ParticleGenerator{

public:
	DefaultParticleGenerator() : ParticleGenerator() {}
	virtual ~DefaultParticleGenerator() {}
	void generate(ParticleArray* array,const Vector2f& position,float dt,uint32 start,uint32 end) {
		for ( uint32 i = start; i < end; ++i ) {
			array->color[i] = Color::WHITE;
			array->scale[i] = Vector2f(1,1);
			array->rotation[i] = 0.0f;
			array->timer[i] = Vector3f(0,1,1);
			array->random[i] = 1.0f;
			array->acceleration[i] = Vector2f(0,0);
		}
	}

};

struct RingGeneratorData {

	float radius;
	float variance;
};

class RingGenerator : public AbstractParticleGenerator<RingGeneratorData> {

public:
	RingGenerator() : AbstractParticleGenerator<RingGeneratorData>() {
		m_Translator.add("radius",&RingGeneratorData::radius);
		m_Translator.add("variance",&RingGeneratorData::variance);
	}
	virtual ~RingGenerator() {}
	void init(float radius,float variance) {
		m_Data.radius = radius;
		m_Data.variance = variance;
	}
	void generate(ParticleArray* array,const Vector2f& position,float dt,uint32 start,uint32 end) {
		uint32 count = end - start;
		Vector2f v(512,384);
		float angle = 0.0f;
		float step = TWO_PI / static_cast<float>(count);
		for ( uint32 i = 0; i < count; ++i ) {
			float rad = ds::math::random(m_Data.radius-m_Data.variance,m_Data.radius+m_Data.variance);
			array->position[start+i].x = v.x + rad * cos(angle);
			array->position[start+i].y = v.y + rad * sin(angle);
			array->rotation[start+i] = angle;
			angle += step;
		}
	}
};

// -------------------------------------------------------
// RadialVelocityGenerator
// -------------------------------------------------------
struct RadialVelocityGeneratorData {

	float velocity;
	float variance;

};
class RadialVelocityGenerator : public AbstractParticleGenerator<RadialVelocityGeneratorData> {

public:
	RadialVelocityGenerator() : AbstractParticleGenerator<RadialVelocityGeneratorData>() {
		m_Translator.add("velocity",&RadialVelocityGeneratorData::velocity);
		m_Translator.add("variance",&RadialVelocityGeneratorData::variance);
	}

	virtual ~RadialVelocityGenerator() {}
	void init(float velocity,float variance) {
		m_Data.velocity = velocity;
		m_Data.variance = variance;
	}
	void generate(ParticleArray* array,const Vector2f& position,float dt,uint32 start,uint32 end) {
		uint32 count = end - start;
		for ( uint32 i = 0; i < count; ++i ) {
			float v = ds::math::random(m_Data.velocity-m_Data.variance,m_Data.velocity+m_Data.variance);
			array->velocity[start+i] = vector::getRadialVelocity(array->rotation[start+i],v);
		}
	}
};

// -------------------------------------------------------
// LifetimeGenerator
// -------------------------------------------------------
struct LifetimeGeneratorData {

	float ttl;
	float variance;
};

class LifetimeGenerator : public AbstractParticleGenerator<LifetimeGeneratorData> {

public:
	LifetimeGenerator() : AbstractParticleGenerator<LifetimeGeneratorData>() {
		m_Translator.add("ttl",&LifetimeGeneratorData::ttl);
		m_Translator.add("variance",&LifetimeGeneratorData::variance);
	}
	virtual ~LifetimeGenerator() {}
	void init(float ttl,float variance) {
		m_Data.ttl = ttl;
		m_Data.variance = variance;
	}
	void generate(ParticleArray* array,const Vector2f& position,float dt,uint32 start,uint32 end) {
		uint32 count = end - start;
		for ( uint32 i = 0; i < count; ++i ) {
			float ttl = ds::math::random(m_Data.ttl-m_Data.variance,m_Data.ttl+m_Data.variance);
			array->timer[start+i] = Vector3f(0.0f,0.0f,ttl);
		}
	}
};

// -------------------------------------------------------
// SizeGenerator
// -------------------------------------------------------
struct SizeGeneratorData {

	Vector2f scale;
	Vector2f variance;
};

class SizeGenerator : public AbstractParticleGenerator<SizeGeneratorData> {

public:
	SizeGenerator() : AbstractParticleGenerator<SizeGeneratorData>() {
		m_Translator.add("size",&SizeGeneratorData::scale);
		m_Translator.add("variance",&SizeGeneratorData::variance);
	}
	virtual ~SizeGenerator() {}
	void init(const Vector2f& scale,const Vector2f& variance) {
		m_Data.scale = scale;
		m_Data.variance = variance;
	}
	void generate(ParticleArray* array,const Vector2f& position,float dt,uint32 start,uint32 end) {
		uint32 count = end - start;
		for ( uint32 i = 0; i < count; ++i ) {	
			array->scale[start+i].x = ds::math::random(m_Data.scale.x - m_Data.variance.x,m_Data.scale.x + m_Data.variance.x);
			array->scale[start+i].y = ds::math::random(m_Data.scale.y - m_Data.variance.y,m_Data.scale.y + m_Data.variance.y);
		}
	}
};

// -------------------------------------------------------
// ParticleRandomGenerator
// -------------------------------------------------------
struct ParticleRandomGeneratorData {

	float minRandom;
	float maxRandom;

	ParticleRandomGeneratorData() : minRandom(0.0f) , maxRandom(1.0f) {}
};

class ParticleRandomGenerator : public AbstractParticleGenerator<ParticleRandomGeneratorData> {

public:
	ParticleRandomGenerator() : AbstractParticleGenerator<ParticleRandomGeneratorData>() {
		m_Translator.add("min_random",&ParticleRandomGeneratorData::minRandom);
		m_Translator.add("max_random",&ParticleRandomGeneratorData::maxRandom);
	}
	virtual ~ParticleRandomGenerator() {}
	void init(float random) {
		m_Data.minRandom = random;
		m_Data.maxRandom = 1.0f;
	}
	void generate(ParticleArray* array,const Vector2f& position,float dt,uint32 start,uint32 end) {
		uint32 count = end - start;
		for ( uint32 i = 0; i < count; ++i ) {	
			array->random[start+i] = ds::math::random(m_Data.minRandom,m_Data.maxRandom);
		}
	}
};

// -------------------------------------------------------
// ParticleEmitter
// -------------------------------------------------------
class ParticleEmitter {

	typedef std::vector<ParticleGenerator*> Generators;

public:
	ParticleEmitter() {
		add(new DefaultParticleGenerator());
	}
	~ParticleEmitter() {
		clear();
	}

	void clear() {
		Generators::iterator it = m_Generators.begin();
		while ( it != m_Generators.end()) {
			delete (*it);
			it = m_Generators.erase(it);
		}
		add(new DefaultParticleGenerator());
	}
	void generate(ParticleArray* array,const Vector2f& pos,float dt) {
		uint32 start = array->countAlive;
		uint32 end = start + m_EmitterData.count;
		if ( end > array->count ) {
			end = array->count;
		}
		for ( size_t i = 0; i < m_Generators.size(); ++i ) {
			m_Generators[i]->generate(array,pos,dt,start,end);
		}
		for ( uint32 i = start; i < end; ++i ) {
			array->wake(i);
		}
	}
	void add(ParticleGenerator* generator) {
		m_Generators.push_back(generator);
	}
	ParticleEmitterData& getEmitterData() {
		return m_EmitterData;
	}
private:
	Generators m_Generators;
	ParticleEmitterData m_EmitterData;
};

}


