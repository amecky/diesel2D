#pragma once
#include "..\io\json.h"
#include "Particle.h"

namespace ds {

	struct ParticleGeneratorData {

		virtual void save(JSONWriter& writer) = 0;
		virtual void read(const JSONReader& reader, int category) = 0;

	};

enum ParticleGeneratorType {
	PGT_DEFAULT,
	PGT_RING,
	PGT_POINT,
	PGT_SIZE,
	PGT_CIRCLE,
	PGT_LIFETIME,
	PGT_RADIAL_VELOCITY,
	PGT_COLOR,
	PGT_HSV_COLOR,
	PGT_ROTATION_VELOCITY,
	PGT_BOX,
	PGT_UNKNOWN
};
// -------------------------------------------------------
// Particle generator
// -------------------------------------------------------
class ParticleGenerator {

public:
	ParticleGenerator() {}
	virtual ~ParticleGenerator() {}
	virtual void generate(ParticleArray* array, const ParticleGeneratorData* data, float dt, uint32 start, uint32 end) = 0;
	virtual const ParticleGeneratorType getType() const = 0;
	virtual const char* getName() const = 0;
	int getChunkID() {
		return getType() + 200;
	}
};

// -------------------------------------------------------
// Ring generator
// -------------------------------------------------------
struct RingGeneratorData : ParticleGeneratorData {

	float radius;
	float variance;
	float angleVariance;
	float step;
	
	RingGeneratorData() : radius(10.0f), variance(0.0f), angleVariance(0.0f), step(0.0f) {}

	void save(JSONWriter& writer) {
		writer.write("radius", radius);
		writer.write("variance", variance);
		writer.write("angle_variance", angleVariance);
		writer.write("step", step);
	}

	void read(const JSONReader& reader, int category) {
		reader.get_float(category, "radius", &radius);
		reader.get_float(category, "variance", &variance);
		reader.get_float(category, "angle_variance", &angleVariance);
		reader.get_float(category, "step", &step);
	}
};

class RingGenerator : public ParticleGenerator {

public:
	RingGenerator() : ParticleGenerator() {
		m_Angle = 0.0f;
	}
	virtual ~RingGenerator() {}
	void generate(ParticleArray* array, const ParticleGeneratorData* data, float dt, uint32 start, uint32 end) {
		ZoneTracker z("RingGenerator:generate");
		uint32 count = end - start;
		//float angle = 0.0f;
		const RingGeneratorData* my_data = static_cast<const RingGeneratorData*>(data);
		float angleVariance = DEGTORAD(my_data->angleVariance);
		float step = TWO_PI / static_cast<float>(count);
		if (my_data->step != 0.0f) {
			step = DEGTORAD(my_data->step);
		}
		for ( uint32 i = 0; i < count; ++i ) {
			float myAngle = m_Angle + ds::math::random(-angleVariance,angleVariance);
			float rad = ds::math::random(my_data->radius - my_data->variance, my_data->radius + my_data->variance);
			array->position[start + i].x = array->position[start + i].x + rad * math::fastCos(myAngle);
			array->position[start + i].y = array->position[start + i].y + rad * math::fastSin(myAngle);
			array->position[start + i].z = array->position[start + i].z;
			//array->position[start + i].y = position.y;
			//array->position[start + i].z = position.z + rad * math::fastSin(myAngle);
			array->rotation[start + i] = myAngle;
			m_Angle += step;
		}
	}
	const char* getName() const {
		return "ring_position";
	}
	const ParticleGeneratorType getType() const {
		return PGT_RING;
	}
private:
	float m_Angle;
};

// -------------------------------------------------------
// Box generator
// -------------------------------------------------------
struct BoxGeneratorData : ParticleGeneratorData {

	int numX;
	int numY;
	float stepX;
	float stepY;

	BoxGeneratorData() : numX(1), numY(1), stepX(10.0f), stepY(10.0f) {}

	void save(JSONWriter& writer) {
		writer.write("num_x", numX);
		writer.write("num_y", numY);
		writer.write("step_x", stepX);
		writer.write("step_y", stepY);
	}

	void read(const JSONReader& reader, int category) {
		reader.get_int(category, "num_x", &numX);
		reader.get_int(category, "num_y", &numY);
		reader.get_float(category, "step_x", &stepX);
		reader.get_float(category, "step_y", &stepY);
	}
};

class BoxGenerator : public ParticleGenerator {

public:
	BoxGenerator() : ParticleGenerator() {
		m_Angle = 0.0f;
	}
	virtual ~BoxGenerator() {}
	void generate(ParticleArray* array, const ParticleGeneratorData* data, float dt, uint32 start, uint32 end) {
		ZoneTracker z("BoxGenerator:generate");
		uint32 count = end - start;
		const BoxGeneratorData* my_data = static_cast<const BoxGeneratorData*>(data);
		int cnt = 0;
		for (int y = 0; y < my_data->numY; ++y) {
			for (int x = 0; x < my_data->numX; ++x) {
				v2 center = array->position[cnt].xy();
				float dx = center.x + x * my_data->stepX - my_data->numX * my_data->stepX * 0.5f;
				float dy = center.y + y * my_data->stepY - my_data->numY * my_data->stepY * 0.5f;
				if (cnt < count) {
					array->position[cnt] = v3(dx, dy, 0.0f);
					array->rotation[cnt] = math::getTargetAngle(array->position[cnt].xy(), center);
				}
				++cnt;
			}
		}
	}
	const char* getName() const {
		return "box";
	}
	const ParticleGeneratorType getType() const {
		return PGT_BOX;
	}
private:
	float m_Angle;
};
/*
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
	void generate(ParticleArray* array, const ParticleGeneratorData& data, float dt, uint32 start, uint32 end) {
		uint32 count = end - start;
		for (uint32 i = 0; i < count; ++i) {
			float myAngle = ds::math::random(0.0f,TWO_PI);
			float rad = ds::math::random(m_Data.minRadius, m_Data.maxRadius);
			array->position[start + i].x = data.position.x + rad * math::fastCos(myAngle);
			array->position[start + i].y = data.position.y + rad * math::fastSin(myAngle);
			array->rotation[start + i] = myAngle;
			array->type[start + i] = 1;
		}
	}
	const char* getName() const {
		return "Circle";
	}
	const ParticleGeneratorType getType() const {
		return PGT_CIRCLE;
	}
};

// -------------------------------------------------------
// Line generator
// -------------------------------------------------------
struct LineGeneratorData {

	Vector3f startPos;
	Vector3f endPos;

	LineGeneratorData() : startPos(0,0,0) , endPos(0,0,0) {}
};

class LineGenerator : public AbstractParticleGenerator<LineGeneratorData> {

public:
	LineGenerator() : AbstractParticleGenerator<LineGeneratorData>() {
		m_Translator.add("start_position", &LineGeneratorData::startPos);
		m_Translator.add("end_position", &LineGeneratorData::endPos);
	}
	virtual ~LineGenerator() {}
	void init(const Vector3f& startPos,const Vector3f& endPos) {
		m_Data.startPos = startPos;
		m_Data.endPos = endPos;
	}	
	void generate(ParticleArray* array, const ParticleGeneratorData& data, float dt, uint32 start, uint32 end) {
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
	const char* getName() const {
		return "Line";
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
	void generate(ParticleArray* array, const ParticleGeneratorData& data, float dt, uint32 start, uint32 end) {
		float angle = m_Data.angle;
		float angleVariance = DEGTORAD(m_Data.angleVariance);
		float myAngle = angle + ds::math::random(-angleVariance,angleVariance);
		array->position[start].x = data.position.x;
		array->position[start].y = data.position.y;
		array->rotation[start] = myAngle;
		if (data.rotation != 0.0f) {
			array->rotation[start] = data.rotation;
		}
		array->type[start] = 1;
	}
	const ParticleGeneratorType getType() const {
		return PGT_POINT;
	}
	const char* getName() const {
		return "Point";
	}
};

// -------------------------------------------------------
// Sphere generator
// -------------------------------------------------------
struct SphereGeneratorData {

	Vector2f beta;
	Vector2f phi;
	float radius;
	float radiusVariance;

	SphereGeneratorData() : radius(1.0f), radiusVariance(0.0f), beta(-180, 180), phi(0, 360) {}
};

class SphereGenerator : public AbstractParticleGenerator<SphereGeneratorData> {

public:
	SphereGenerator() : AbstractParticleGenerator<SphereGeneratorData>() {
		m_Translator.add("radius", &SphereGeneratorData::radius);
		m_Translator.add("radius_variance", &SphereGeneratorData::radiusVariance);
		m_Translator.add("beta", &SphereGeneratorData::beta);
		m_Translator.add("phi", &SphereGeneratorData::phi);
	}
	virtual ~SphereGenerator() {}
	void generate(ParticleArray* array, const ParticleGeneratorData& data, float dt, uint32 start, uint32 end) {
		uint32 count = end - start;
		int slices = 16;
		int ring = count / slices;
		float deltaBeta = TWO_PI / static_cast<float> (slices);
		float beta = -PI;// 0.0f;// m_Data.angle;
		int cnt = 0;
		for (int j = 0; j < slices; ++j) {
			for (int i = 0; i < ring; ++i) {				
				float myAngle = static_cast<float>(i) / static_cast<float>(ring)* TWO_PI;
				float rad = ds::math::random(m_Data.radius - m_Data.radiusVariance, m_Data.radius + m_Data.radiusVariance);
				array->position[start + cnt].x = data.position.x + rad * math::fastCos(myAngle) * math::fastSin(beta);
				array->position[start + cnt].y = data.position.y + rad * math::fastSin(myAngle) * math::fastSin(beta);
				array->position[start + cnt].z = data.position.z + rad * math::fastCos(beta);
				array->rotation[start + cnt] = beta;
				array->type[start + cnt] = 1;
				++cnt;
			}
			beta += deltaBeta;
		}
	}
	const char* getName() const {
		return "Sphere";
	}
	const ParticleGeneratorType getType() const {
		return PGT_POINT;
	}
};

// -------------------------------------------------------
// Sphere generator
// -------------------------------------------------------
struct RandomSphereGeneratorData {

	float radius;
	float radiusVariance;
	Vector2f beta;
	Vector2f phi;

	RandomSphereGeneratorData() : radius(1.0f), radiusVariance(0.0f) , beta(-180,180) , phi(0,360) {}
};

class RandomSphereGenerator : public AbstractParticleGenerator<RandomSphereGeneratorData> {

public:
	RandomSphereGenerator() : AbstractParticleGenerator<RandomSphereGeneratorData>() {
		m_Translator.add("radius", &RandomSphereGeneratorData::radius);
		m_Translator.add("radius_variance", &RandomSphereGeneratorData::radiusVariance);
		m_Translator.add("beta", &RandomSphereGeneratorData::beta);
		m_Translator.add("phi", &RandomSphereGeneratorData::phi);
	}
	virtual ~RandomSphereGenerator() {}
	void generate(ParticleArray* array, const ParticleGeneratorData& data, float dt, uint32 start, uint32 end) {
		uint32 count = end - start;
		for (uint32 i = 0; i < count; ++i) {
			float myAngle = ds::math::random(DEGTORAD(m_Data.phi.x), DEGTORAD(m_Data.phi.y));
			float beta = ds::math::random(DEGTORAD(m_Data.beta.x), DEGTORAD(m_Data.beta.y));
			float rad = ds::math::random(m_Data.radius - m_Data.radiusVariance, m_Data.radius + m_Data.radiusVariance);
			array->position[start + i].x = data.position.x + rad * math::fastCos(myAngle) * math::fastSin(beta);
			array->position[start + i].y = data.position.y + rad * math::fastSin(myAngle) * math::fastSin(beta);
			array->position[start + i].z = data.position.z + rad * math::fastCos(beta);
			array->rotation[start + i] = beta;
			array->type[start + i] = 1;
		}
		
	}
	const char* getName() const {
		return "RandomSphere";
	}
	const ParticleGeneratorType getType() const {
		return PGT_POINT;
	}
};
*/
// -------------------------------------------------------
// RadialVelocityGenerator
// -------------------------------------------------------
struct RadialVelocityGeneratorData : ParticleGeneratorData {

	float velocity;
	float variance;

	RadialVelocityGeneratorData() : velocity(0.0f), variance(0.0f) {}

	void save(JSONWriter& writer) {
		writer.write("velocity", velocity);
		writer.write("variance", variance);
	}

	void read(const JSONReader& reader, int category) {
		reader.get_float(category, "velocity", &velocity);
		reader.get_float(category, "variance", &variance);
	}
};

class RadialVelocityGenerator : public ParticleGenerator {

public:
	RadialVelocityGenerator() : ParticleGenerator() {}
	virtual ~RadialVelocityGenerator() {}
	void generate(ParticleArray* array, const ParticleGeneratorData* data, float dt, uint32 start, uint32 end) {
		ZoneTracker z("RadialVelocityGenerator:generate");
		assert(data != 0);
		uint32 count = end - start;
		const RadialVelocityGeneratorData* my_data = static_cast<const RadialVelocityGeneratorData*>(data);
		for ( uint32 i = 0; i < count; ++i ) {
			float v = ds::math::random(my_data->velocity - my_data->variance, my_data->velocity + my_data->variance);
			//Vector3f d = array->position[start + i] - data.position;
			//Vector3f dn = normalize(d);
			//array->velocity[start + i] = dn * v;
			array->velocity[start+i] = vector::getRadialVelocity(array->rotation[start+i],v);
		}
	}
	const char* getName() const {
		return "radial_velocity";
	}
	const ParticleGeneratorType getType() const {
		return PGT_RADIAL_VELOCITY;
	}
};
/*
// -------------------------------------------------------
// VelocityGenerator
// -------------------------------------------------------
struct VelocityGeneratorData {

	Vector3f velocity;
	Vector3f variance;

};
class VelocityGenerator : public AbstractParticleGenerator<VelocityGeneratorData> {

public:
	VelocityGenerator() : AbstractParticleGenerator<VelocityGeneratorData>() {
		m_Translator.add("velocity", &VelocityGeneratorData::velocity);
		m_Translator.add("variance", &VelocityGeneratorData::variance);
	}

	virtual ~VelocityGenerator() {}
	void init(const Vector3f& velocity, const Vector3f& variance) {
		m_Data.velocity = velocity;
		m_Data.variance = variance;
	}
	void generate(ParticleArray* array, const ParticleGeneratorData& data, float dt, uint32 start, uint32 end) {
		uint32 count = end - start;
		for (uint32 i = 0; i < count; ++i) {
			array->velocity[start + i].x = ds::math::random(m_Data.velocity.x - m_Data.variance.x, m_Data.velocity.x + m_Data.variance.x);
			array->velocity[start + i].y = ds::math::random(m_Data.velocity.y - m_Data.variance.y, m_Data.velocity.y + m_Data.variance.y);
		}
	}
	const ParticleGeneratorType getType() const {
		return PGT_DEFAULT;
	}
	const char* getName() const {
		return "Velocity";
	}
};
*/
// -------------------------------------------------------
// LifetimeGenerator
// -------------------------------------------------------
struct LifetimeGeneratorData : ParticleGeneratorData {

	float ttl;
	float variance;

	LifetimeGeneratorData() : ttl(1.0f), variance(0.0f) {}

	void save(JSONWriter& writer) {
		writer.write("ttl", ttl);
		writer.write("variance", variance);
	}

	void read(const JSONReader& reader, int category) {
		reader.get_float(category, "ttl", &ttl);
		reader.get_float(category, "variance", &variance);
	}
};

class LifetimeGenerator : public ParticleGenerator {

public:
	LifetimeGenerator() : ParticleGenerator() {
	}
	virtual ~LifetimeGenerator() {}
	void generate(ParticleArray* array, const ParticleGeneratorData* data, float dt, uint32 start, uint32 end) {
		ZoneTracker z("LifetimeGenerator:generate");
		assert(data != 0);
		uint32 count = end - start;
		const LifetimeGeneratorData* my_data = static_cast<const LifetimeGeneratorData*>(data);
		for ( uint32 i = 0; i < count; ++i ) {
			float ttl = ds::math::random(my_data->ttl - my_data->variance, my_data->ttl + my_data->variance);
			array->timer[start+i] = Vector3f(0.0f,0.0f,ttl);
		}
	}
	const ParticleGeneratorType getType() const {
		return PGT_LIFETIME;
	}
	const char* getName() const {
		return "lifecycle";
	}
};

// -------------------------------------------------------
// ColorGenerator
// -------------------------------------------------------
struct ColorGeneratorData : ParticleGeneratorData {

	Color color;

	ColorGeneratorData() : color(Color::WHITE) {}

	void save(JSONWriter& writer) {
		writer.write("color", color);
	}

	void read(const JSONReader& reader, int category) {
		reader.get_color(category, "color", &color);
	}

};

class  ColorGenerator : public ParticleGenerator {

public:
	ColorGenerator() : ParticleGenerator() {
	}
	virtual ~ColorGenerator() {}
	void generate(ParticleArray* array, const ParticleGeneratorData* data, float dt, uint32 start, uint32 end) {
		assert(data != 0);
		uint32 count = end - start;
		const ColorGeneratorData* my_data = static_cast<const ColorGeneratorData*>(data);
		for (uint32 i = 0; i < count; ++i) {
			array->color[start + i] = my_data->color;
		}
	}
	const ParticleGeneratorType getType() const {
		return PGT_COLOR;
	}
	const char* getName() const {
		return "color";
	}
};

// -------------------------------------------------------
// ColorGenerator
// -------------------------------------------------------
struct HSVColorGeneratorData : ParticleGeneratorData {

	v3 hsv;
	float hueVariance;
	float saturationVariance;
	float valueVariance;
	float alpha;

	HSVColorGeneratorData() : hsv(360, 100, 100), hueVariance(0.0f), saturationVariance(0.0f), valueVariance(0.0f), alpha(255.0f) {}

	void save(JSONWriter& writer) {
		writer.write("hsv", hsv);
		writer.write("alpha", alpha);
		writer.write("hue_variance", hueVariance);
		writer.write("saturation_variance", saturationVariance);
		writer.write("value_variance", valueVariance);
	}

	void read(const JSONReader& reader, int category) {
		reader.get_vec3(category, "hsv", &hsv);
		reader.get_float(category, "alpha", &alpha);
		reader.get_float(category, "hue_variance", &hueVariance);
		reader.get_float(category, "saturation_variance", &saturationVariance);
		reader.get_float(category, "value_variance", &valueVariance);
	}
};

class  HSVColorGenerator : public ParticleGenerator {

public:
	HSVColorGenerator() : ParticleGenerator() {
		
	}
	
	virtual ~HSVColorGenerator() {}
	
	void generate(ParticleArray* array, const ParticleGeneratorData* data, float dt, uint32 start, uint32 end) {
		assert(data != 0);
		uint32 count = end - start;
		const HSVColorGeneratorData* my_data = static_cast<const HSVColorGeneratorData*>(data);
		for (uint32 i = 0; i < count; ++i) {
			float hv = ds::math::random(-my_data->hueVariance, my_data->hueVariance);
			float h = math::clamp(my_data->hsv.x + hv, 0.0f, 360.0f);
			float sv = ds::math::random(-my_data->saturationVariance, my_data->saturationVariance);
			float s = math::clamp(my_data->hsv.y + sv, 0.0f, 100.0f);
			float vv = ds::math::random(-my_data->valueVariance, my_data->valueVariance);
			float v = math::clamp(my_data->hsv.z + vv, 0.0f, 100.0f);
			Color c = color::hsvToColor(h, s, v);			
			if (my_data->alpha > 1.0f) {
				c.a = my_data->alpha / 255.0f;
			}
			else {
				c.a = my_data->alpha;
			}
			array->color[start + i] = c;
		}
	}
	const ParticleGeneratorType getType() const {
		return PGT_HSV_COLOR;
	}
	const char* getName() const {
		return "hsv_color";
	}
};

// -------------------------------------------------------
// SizeGenerator
// -------------------------------------------------------
struct SizeGeneratorData : ParticleGeneratorData {

	v2 scale;
	v2 variance;

	SizeGeneratorData() : scale(1, 1), variance(0, 0) {}

	void save(JSONWriter& writer) {
		writer.write("scale", scale);
		writer.write("variance", variance);
	}

	void read(const JSONReader& reader, int category) {
		reader.get_vec2(category, "scale", &scale);
		reader.get_vec2(category, "variance", &variance);
	}
};

class SizeGenerator : public ParticleGenerator {

public:
	SizeGenerator() : ParticleGenerator() {
		
	}
	virtual ~SizeGenerator() {}
	
	void generate(ParticleArray* array, const ParticleGeneratorData* data, float dt, uint32 start, uint32 end) {
		assert(data != 0);
		const SizeGeneratorData* my_data = static_cast<const SizeGeneratorData*>(data);
		uint32 count = end - start;
		for (uint32 i = 0; i < count; ++i) {
			array->scale[start + i].x = ds::math::random(my_data->scale.x - my_data->variance.x, my_data->scale.x + my_data->variance.x);
			array->scale[start + i].y = ds::math::random(my_data->scale.y - my_data->variance.y, my_data->scale.y + my_data->variance.y);
			array->baseScale[start + i] = array->scale[start + i];
		}
	}
	const ParticleGeneratorType getType() const {
		return PGT_SIZE;
	}
	const char* getName() const {
		return "size";
	}
};

// -------------------------------------------------------
// RotationVelocityGenerator
// -------------------------------------------------------
struct RotationVelocityGeneratorData : ParticleGeneratorData {

	float velocity;
	float variance;

	RotationVelocityGeneratorData() : velocity(0.0f), variance(0.0f) {}

	void save(JSONWriter& writer) {
		writer.write("velocity", RADTODEG(velocity));
		writer.write("variance", RADTODEG(variance));
	}

	void read(const JSONReader& reader, int category) {
		reader.get_float(category, "velocity", &velocity);
		reader.get_float(category, "variance", &variance);
		velocity = DEGTORAD(velocity);
		variance = DEGTORAD(variance);
	}
};

class RotationVelocityGenerator : public ParticleGenerator {

public:
	RotationVelocityGenerator() : ParticleGenerator() {}
	virtual ~RotationVelocityGenerator() {}
	void generate(ParticleArray* array, const ParticleGeneratorData* data, float dt, uint32 start, uint32 end) {
		assert(data != 0);
		uint32 count = end - start;
		const RotationVelocityGeneratorData* my_data = static_cast<const RotationVelocityGeneratorData*>(data);
		for (uint32 i = 0; i < count; ++i) {
			array->rotationVelocity[i] = ds::math::random(my_data->velocity - my_data->variance, my_data->velocity + my_data->variance);
		}
	}
	const char* getName() const {
		return "rotation_velocity";
	}
	const ParticleGeneratorType getType() const {
		return PGT_ROTATION_VELOCITY;
	}
};

}


