#pragma once
#include "..\utils\PlainTextReader.h"
#include "..\io\BinaryLoader.h"
#include "Particle.h"
#include "..\io\BinaryWriter.h"
#include "..\data\DataTranslator.h"

namespace ds {

enum ParticleGeneratorType {
	PGT_DEFAULT,
	PGT_RING,
	PGT_POINT,
	PGT_SIZE,
	PGT_CIRCLE,
	PGT_UNKNOWN
};
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
	virtual const ParticleGeneratorType getType() const = 0;
	//virtual const int getChunkID() const = 0;
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

// -------------------------------------------------------
// default generator
// -------------------------------------------------------
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
			array->type[i] = 1;
		}
	}
	const ParticleGeneratorType getType() const {
		return PGT_DEFAULT;
	}
};

// -------------------------------------------------------
// Ring generator
// -------------------------------------------------------
struct RingGeneratorData {

	float radius;
	float variance;
	float angleVariance;
	float step;
	
	RingGeneratorData() : radius(10.0f), variance(0.0f), angleVariance(0.0f), step(0.0f) {}

};

class RingGenerator : public AbstractParticleGenerator<RingGeneratorData> {

public:
	RingGenerator() : AbstractParticleGenerator<RingGeneratorData>() {
		m_Translator.add("radius",&RingGeneratorData::radius);
		m_Translator.add("variance",&RingGeneratorData::variance);
		m_Translator.add("angle_variance", &RingGeneratorData::angleVariance);
		m_Translator.add("step", &RingGeneratorData::step);
		m_Angle = 0.0f;
	}
	virtual ~RingGenerator() {}
	void init(float radius,float variance) {
		m_Data.radius = radius;
		m_Data.variance = variance;
	}
	void generate(ParticleArray* array,const Vector2f& position,float dt,uint32 start,uint32 end) {
		uint32 count = end - start;
		//float angle = 0.0f;
		float angleVariance = DEGTORAD(m_Data.angleVariance);
		float step = TWO_PI / static_cast<float>(count);
		if (m_Data.step != 0.0f) {
			step = DEGTORAD(m_Data.step);
		}
		for ( uint32 i = 0; i < count; ++i ) {
			float myAngle = m_Angle + ds::math::random(-angleVariance,angleVariance);
			float rad = ds::math::random(m_Data.radius-m_Data.variance,m_Data.radius+m_Data.variance);
			array->position[start + i].x = position.x + rad * cos(myAngle);
			array->position[start + i].y = position.y + rad * sin(myAngle);
			array->rotation[start + i] = myAngle;
			array->type[start + i] = 1;
			m_Angle += step;
		}
	}
	const ParticleGeneratorType getType() const {
		return PGT_RING;
	}
private:
	float m_Angle;
};

// -------------------------------------------------------
// Ring generator
// -------------------------------------------------------
struct CircleGeneratorData {

	float minRadius;
	float maxRadius;

};

class CircleGenerator : public AbstractParticleGenerator<CircleGeneratorData> {

public:
	CircleGenerator() : AbstractParticleGenerator<CircleGeneratorData>() {
		m_Translator.add("min_radius", &CircleGeneratorData::minRadius);
		m_Translator.add("max_radius", &CircleGeneratorData::maxRadius);
	}
	virtual ~CircleGenerator() {}
	void init(float minRadius, float maxRadius) {
		m_Data.minRadius = minRadius;
		m_Data.maxRadius = maxRadius;
	}
	void generate(ParticleArray* array, const Vector2f& position, float dt, uint32 start, uint32 end) {
		uint32 count = end - start;
		for (uint32 i = 0; i < count; ++i) {
			float myAngle = ds::math::random(0.0f,TWO_PI);
			float rad = ds::math::random(m_Data.minRadius, m_Data.maxRadius);
			array->position[start + i].x = position.x + rad * cos(myAngle);
			array->position[start + i].y = position.y + rad * sin(myAngle);
			array->rotation[start + i] = myAngle;
			array->type[start + i] = 1;
		}
	}
	const ParticleGeneratorType getType() const {
		return PGT_CIRCLE;
	}
};

// -------------------------------------------------------
// Line generator
// -------------------------------------------------------
struct LineGeneratorData {

	Vector2f startPos;
	Vector2f endPos;

	LineGeneratorData() : startPos(0,0) , endPos(0,0) {}
};

class LineGenerator : public AbstractParticleGenerator<LineGeneratorData> {

public:
	LineGenerator() : AbstractParticleGenerator<LineGeneratorData>() {
		m_Translator.add("start_position", &LineGeneratorData::startPos);
		m_Translator.add("end_position", &LineGeneratorData::endPos);
	}
	virtual ~LineGenerator() {}
	void init(const Vector2f& startPos,const Vector2f& endPos) {
		m_Data.startPos = startPos;
		m_Data.endPos = endPos;
	}	
	void generate(ParticleArray* array, const Vector2f& position, float dt, uint32 start, uint32 end) {
		float dx = math::random(m_Data.startPos.x, m_Data.endPos.x);
		float dy = math::random(m_Data.startPos.y, m_Data.endPos.y);
		array->position[start].x = dx;
		array->position[start].y = dy;
		array->rotation[start] = 0.0f;
		array->type[start] = 1;
	}
	const ParticleGeneratorType getType() const {
		return PGT_POINT;
	}
};
// -------------------------------------------------------
// Point generator
// -------------------------------------------------------
struct PointGeneratorData {

	float angle;
	float angleVariance;

	PointGeneratorData() : angle(0.0f) , angleVariance(0.0f) {}
};

class PointGenerator : public AbstractParticleGenerator<PointGeneratorData> {

public:
	PointGenerator() : AbstractParticleGenerator<PointGeneratorData>() {
		m_Translator.add("angle",&PointGeneratorData::angle);
		m_Translator.add("angle_variance", &PointGeneratorData::angleVariance);
	}
	virtual ~PointGenerator() {}
	void init(float angle,float variance) {
		m_Data.angle = angle;
		m_Data.angleVariance = variance;
	}
	void setAngle(float angle) {
		m_Data.angle = angle;
	}
	void generate(ParticleArray* array,const Vector2f& position,float dt,uint32 start,uint32 end) {
		float angle = m_Data.angle;
		float angleVariance = DEGTORAD(m_Data.angleVariance);
		float myAngle = angle + ds::math::random(-angleVariance,angleVariance);
		array->position[start].x = position.x;
		array->position[start].y = position.y;
		array->rotation[start] = myAngle;
		array->type[start] = 1;
	}
	const ParticleGeneratorType getType() const {
		return PGT_POINT;
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
	const ParticleGeneratorType getType() const {
		return PGT_DEFAULT;
	}
};

// -------------------------------------------------------
// VelocityGenerator
// -------------------------------------------------------
struct VelocityGeneratorData {

	Vector2f velocity;
	Vector2f variance;

};
class VelocityGenerator : public AbstractParticleGenerator<VelocityGeneratorData> {

public:
	VelocityGenerator() : AbstractParticleGenerator<VelocityGeneratorData>() {
		m_Translator.add("velocity", &VelocityGeneratorData::velocity);
		m_Translator.add("variance", &VelocityGeneratorData::variance);
	}

	virtual ~VelocityGenerator() {}
	void init(const Vector2f& velocity, const Vector2f& variance) {
		m_Data.velocity = velocity;
		m_Data.variance = variance;
	}
	void generate(ParticleArray* array, const Vector2f& position, float dt, uint32 start, uint32 end) {
		uint32 count = end - start;
		for (uint32 i = 0; i < count; ++i) {
			array->velocity[start + i].x = ds::math::random(m_Data.velocity.x - m_Data.variance.x, m_Data.velocity.x + m_Data.variance.x);
			array->velocity[start + i].y = ds::math::random(m_Data.velocity.y - m_Data.variance.y, m_Data.velocity.y + m_Data.variance.y);
		}
	}
	const ParticleGeneratorType getType() const {
		return PGT_DEFAULT;
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
	const ParticleGeneratorType getType() const {
		return PGT_DEFAULT;
	}
};

// -------------------------------------------------------
// ColorGenerator
// -------------------------------------------------------
struct ColorGeneratorData {

	Color color;

};

class  ColorGenerator : public AbstractParticleGenerator< ColorGeneratorData> {

public:
	ColorGenerator() : AbstractParticleGenerator< ColorGeneratorData>() {
		m_Translator.add("color", &ColorGeneratorData::color);
	}
	virtual ~ColorGenerator() {}
	void init(const Color& color) {
		m_Data.color = color;
	}
	void generate(ParticleArray* array, const Vector2f& position, float dt, uint32 start, uint32 end) {
		uint32 count = end - start;
		for (uint32 i = 0; i < count; ++i) {
			array->color[start + i] = m_Data.color;
		}
	}
	const ParticleGeneratorType getType() const {
		return PGT_DEFAULT;
	}
};

// -------------------------------------------------------
// ColorGenerator
// -------------------------------------------------------
struct HSVColorGeneratorData {

	Vector3f hsv;
	float hueVariance;
	float saturationVariance;
	float valueVariance;
	float alpha;
};

class  HSVColorGenerator : public AbstractParticleGenerator<HSVColorGeneratorData> {

public:
	HSVColorGenerator() : AbstractParticleGenerator<HSVColorGeneratorData>() {
		m_Translator.add("hsv", &HSVColorGeneratorData::hsv);
		m_Translator.add("alpha", &HSVColorGeneratorData::alpha);
		m_Translator.add("hue_variance", &HSVColorGeneratorData::hueVariance);
		m_Translator.add("saturation_variance", &HSVColorGeneratorData::saturationVariance);
		m_Translator.add("value_variance", &HSVColorGeneratorData::valueVariance);
	}
	
	virtual ~HSVColorGenerator() {}
	
	void init(const Vector3f& hsv) {
		m_Data.hsv = hsv;
	}

	void generate(ParticleArray* array, const Vector2f& position, float dt, uint32 start, uint32 end) {
		uint32 count = end - start;
		for (uint32 i = 0; i < count; ++i) {
			float hv = ds::math::random(-m_Data.hueVariance, m_Data.hueVariance);
			float h = math::clamp(m_Data.hsv.x + hv, 0.0f, 360.0f);
			float sv = ds::math::random(-m_Data.saturationVariance, m_Data.saturationVariance);
			float s = math::clamp(m_Data.hsv.y + sv, 0.0f, 100.0f);
			float vv = ds::math::random(-m_Data.valueVariance, m_Data.valueVariance);
			float v = math::clamp(m_Data.hsv.z + vv, 0.0f, 100.0f);
			Color c = color::hsvToColor(h, s, v);			
			c.a = m_Data.alpha;
			array->color[start + i] = c;
		}
	}
	const ParticleGeneratorType getType() const {
		return PGT_DEFAULT;
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
	const ParticleGeneratorType getType() const {
		return PGT_SIZE;
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
	const ParticleGeneratorType getType() const {
		return PGT_DEFAULT;
	}
};

// -------------------------------------------------------
// ParticleSpawner
// -------------------------------------------------------
struct ParticleSpawnerData {

	int rate; // particles per second
	float frequence;
	int mode; // 0 = burst 1 = rate
};

class ParticleSpawner {

public:
	ParticleSpawner() {}
	~ParticleSpawner() {}
	void tick(float dt);
	int spawn(ParticleArray* array);
	DataTranslator<ParticleSpawnerData>& getTranslator() {
		return m_Translator;
	}

	void convert(Category* category, BinaryWriter& writer) {
		ParticleSpawnerData data;
		DataTranslator<ParticleSpawnerData>& translator = getTranslator();
		translator.read(category, &data);
		translator.saveChunk(writer, 1, &data, true);
	}

	virtual void load(BinaryLoader* loader) {
		DataTranslator<ParticleSpawnerData>& translator = getTranslator();
		translator.readChunk(*loader, &m_Data);
	}

private:
	DataTranslator<ParticleSpawnerData> m_Translator;
	ParticleSpawnerData m_Data;
};
// -------------------------------------------------------
// ParticleEmitter
// -------------------------------------------------------
// -------------------------------------------------------
// Particle emitter data
// -------------------------------------------------------
struct ParticleEmitterData {

	uint32 ejectionPeriod;
	uint32 ejectionVariance;
	uint32 ejectionCounter;
	uint32 count;
	float duration; // seconds
	int loops; // how many ticks to run

	ParticleEmitterData() : ejectionPeriod(0), ejectionVariance(0), ejectionCounter(0), count(1) {}

	void load(BinaryLoader* loader) {
		loader->read(&count);
		loader->read(&ejectionPeriod);
		loader->read(&ejectionVariance);
		loader->read(&ejectionCounter);
		loader->read(&duration);
	}
};

class ParticleEmitterInstance;

class ParticleEmitter {

typedef std::vector<ParticleGenerator*> Generators;

public:
	ParticleEmitter();
	~ParticleEmitter();
	void clear(bool addDefault = true);
	void start();
	void generate(ParticleArray* array, const Vector2f& pos, float dt, bool started = true);
	ParticleGenerator* getGenerator(ParticleGeneratorType type);
	void add(ParticleGenerator* generator);
	ParticleEmitterData& getEmitterData();
	ParticleSpawner& getParticleSpawner();
	void tick(float dt);
	void init();
	void stop();
	void createInstance(ParticleEmitterInstance* instance);
private:
	void generate(ParticleArray* array, const Vector2f& pos,int count, float dt);
	Generators m_Generators;
	ParticleEmitterData m_EmitterData;
	ParticleSpawner m_Spawner;
	float m_Duration;
	float m_Timer;
	bool m_Active;
	bool m_Burst;
	bool m_Endless;
	float m_Accu;
	float m_Frequency;
};

class ParticleEmitterInstance {

typedef std::vector<ParticleGenerator*> Generators;

public:
	ParticleEmitterInstance() {}
	~ParticleEmitterInstance() {}
	void clear(bool addDefault = true);
	void start();
	void generate(ParticleArray* array, const Vector2f& pos, float dt, bool started = true);
	void add(ParticleGenerator* generator);
	void setData(ParticleEmitterData* data) {
		m_EmitterData = data;
	}
	ParticleEmitterData& getEmitterData();
	ParticleSpawner& getParticleSpawner();
	void tick(float dt);
	void init();
	void stop();
private:
	void generate(ParticleArray* array, const Vector2f& pos, int count, float dt);
	Generators m_Generators;
	ParticleEmitterData* m_EmitterData;
	ParticleSpawner m_Spawner;
	float m_Duration;
	float m_Timer;
	bool m_Active;
	bool m_Burst;
	bool m_Endless;
	float m_Accu;
	float m_Frequency;
};
}


