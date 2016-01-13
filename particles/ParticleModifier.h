#pragma once
#include "Particle.h"
#include "..\utils\Profiler.h"
#include "ParticleSystemData.h"
#include "..\io\json.h"

namespace ds {

enum ParticleModifierType {
	PMT_POSITION,
	PMT_LINEAR_SIZE,
	PMT_LINEAR_ALPHA,
	PMT_LINEAR_COLOR,
	PMT_DAMPING_VELOCITY,
	PMT_LIFECYCLE,
	PMT_VELOCITY_ROTATION,
	PMT_PERPENDICULAR_MOVE,
	PMT_COLOR_PATH,
	PMT_ALPHA_PATH,
	PMT_SIZE_PATH,
	PMT_ROTATION,
	PMT_EOL
};

struct ParticleModifierData {

	virtual void read(const JSONReader& reader, int category) = 0;
	virtual void save(JSONWriter& writer) = 0;

};

// -------------------------------------------------------
// Particle modifier
// -------------------------------------------------------
class ParticleModifier {

public:
	ParticleModifier() {}
	virtual ~ParticleModifier() {}
	virtual void update(ParticleArray* array, const ParticleModifierData* data, float dt) = 0;
	virtual void init(ParticleArray* array, const ParticleModifierData* data, uint32 start, uint32 end) = 0;
	virtual const ParticleModifierType getType() const = 0;
	virtual const char* getName() const = 0;
	int getChunkID() {
		return getType() + 100;
	}
};

// -------------------------------------------------------
// ParticlePositionModifier
// -------------------------------------------------------
class ParticlePositionModifier : public ParticleModifier {

public:
	ParticlePositionModifier() : ParticleModifier() {}
	virtual ~ParticlePositionModifier() {}

	void  update(ParticleArray* array, const ParticleModifierData* data, float dt) {
		for ( uint32 i = 0; i < array->countAlive; ++i ) {
			array->velocity[i] += array->acceleration[i] * dt;
			array->position[i] += array->velocity[i] * dt;
		}
	}
	void init(ParticleArray* array, const ParticleModifierData* data, uint32 start, uint32 end) {}
	const ParticleModifierType getType() const {
		return PMT_POSITION;
	}
	const char* getName() const {
		return "position";
	}
};

// -------------------------------------------------------
// ParticleTimeModifier
// -------------------------------------------------------
class ParticleTimeModifier : public ParticleModifier {

public:
	ParticleTimeModifier() : ParticleModifier() {}
	virtual ~ParticleTimeModifier() {}

	void  update(ParticleArray* array, const ParticleModifierData* data, float dt) {
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
	void init(ParticleArray* array, const ParticleModifierData* data, uint32 start, uint32 end) {}
	const ParticleModifierType getType() const {
		return PMT_LIFECYCLE;
	}
	const char* getName() const {
		return "lifecycle";
	}
};

// -------------------------------------------------------
// Linear color modifier
// -------------------------------------------------------
struct LinearColorModifierData : ParticleModifierData {

	Color startColor;
	Color endColor;

	LinearColorModifierData() : startColor(255,255,255,255) , endColor(0,0,0,0) {}

	void save(JSONWriter& writer) {
		writer.write("start_color", startColor);
		writer.write("end_color", endColor);
	}

	void read(const JSONReader& reader, int category) {
		reader.get_color(category, "start_color", &startColor);
		reader.get_color(category, "end_color", &endColor);
	}
};

class LinearColorModifier : public ParticleModifier {

public:
	LinearColorModifier() : ParticleModifier() {
	}
	virtual ~LinearColorModifier() {}
	
	void update(ParticleArray* array, const ParticleModifierData* data, float dt) {
		assert(data != 0);
		const LinearColorModifierData* my_data = static_cast<const LinearColorModifierData*>(data);
		for ( uint32 i = 0; i < array->countAlive; ++i ) {
			array->color[i] = color::lerp(my_data->startColor, my_data->endColor, array->timer[i].y);
		}
	}
	void init(ParticleArray* array, const ParticleModifierData* data, uint32 start, uint32 end) {
		assert(data != 0);
		const LinearColorModifierData* my_data = static_cast<const LinearColorModifierData*>(data);
		for (uint32 i = start; i < end; ++i) {
			array->color[i] = my_data->startColor;
		}
	}
	const ParticleModifierType getType() const {
		return PMT_LINEAR_COLOR;
	}
	const char* getName() const {
		return "linear_color";
	}
};

// -------------------------------------------------------
// Linear color modifier
// -------------------------------------------------------
struct LinearAlphaModifierData : ParticleModifierData {

	float startAlpha;
	float endAlpha;

	LinearAlphaModifierData() : startAlpha(1.0f), endAlpha(0.0f) {}

	void save(JSONWriter& writer) {
		writer.write("start_alpha", startAlpha);
		writer.write("end_alpha", endAlpha);
	}

	void read(const JSONReader& reader, int category) {
		reader.get_float(category, "start_alpha", &startAlpha);
		reader.get_float(category, "end_alpha", &endAlpha);
	}
};

class LinearAlphaModifier : public ParticleModifier {

public:
	LinearAlphaModifier() : ParticleModifier() {
	}
	virtual ~LinearAlphaModifier() {}
	void  update(ParticleArray* array, const ParticleModifierData* data, float dt) {
		assert(data != 0);
		const LinearAlphaModifierData* my_data = static_cast<const LinearAlphaModifierData*>(data);
		for (uint32 i = 0; i < array->countAlive; ++i) {
			array->color[i].a = tweening::interpolate(tweening::easeInOutQuad, my_data->startAlpha, my_data->endAlpha, array->timer[i].y);
		}
	}
	void init(ParticleArray* array, const ParticleModifierData* data, uint32 start, uint32 end) {
		assert(data != 0);
		const LinearAlphaModifierData* my_data = static_cast<const LinearAlphaModifierData*>(data);
		for (uint32 i = start; i < end; ++i) {
			array->color[i].a = my_data->startAlpha;
		}
	}
	const ParticleModifierType getType() const {
		return PMT_LINEAR_ALPHA;
	}
	const char* getName() const {
		return "linear_alpha";
	}
};

// -------------------------------------------------------
// Linear size modifier
// -------------------------------------------------------
struct LinearSizeModifierData : ParticleModifierData {
	
	Vector2f minScale;
	Vector2f maxScale;

	LinearSizeModifierData() : minScale(0,0) , maxScale(1,1) {}

	void save(JSONWriter& writer) {
		writer.write("min_scale", minScale);
		writer.write("max_scale", maxScale);
	}

	void read(const JSONReader& reader, int category) {
		reader.get_vec2(category, "min_scale", &minScale);
		reader.get_vec2(category, "max_scale", &maxScale);
	}
};

class LinearSizeModifier : public ParticleModifier {

public:
	LinearSizeModifier() : ParticleModifier() {
	}
	virtual ~LinearSizeModifier() {}

	void update(ParticleArray* array, const ParticleModifierData* data, float dt) {
		assert(data != 0);
		const LinearSizeModifierData* my_data = static_cast<const LinearSizeModifierData*>(data);
		for ( uint32 i = 0; i < array->countAlive; ++i ) {			
			array->scale[i] = lerp(my_data->minScale, my_data->maxScale, array->timer[i].y);
			array->scale[i].x *= array->baseScale[i].x;
			array->scale[i].y *= array->baseScale[i].y;
		}
	}
	void init(ParticleArray* array, const ParticleModifierData* data, uint32 start, uint32 end) {
		assert(data != 0);
		const LinearSizeModifierData* my_data = static_cast<const LinearSizeModifierData*>(data);
		for (uint32 i = start; i < end; ++i) {
			array->scale[i].x = array->baseScale[i].x * my_data->minScale.x;
			array->scale[i].y = array->baseScale[i].y * my_data->minScale.y;
		}
	}
	const ParticleModifierType getType() const {
		return PMT_LINEAR_SIZE;
	}
	const char* getName() const {
		return "linear_size";
	}
};

// -------------------------------------------------------
// PerpendicularMoveModifier
// -------------------------------------------------------
struct PerpendicularMoveModifierData : ParticleModifierData {
	
	float radius;
	float amplitude;

	PerpendicularMoveModifierData() : radius(10.0f) , amplitude(1.0f) {}

	void read(JSONReader& reader, int category) {
		
	}
};

class PerpendicularMoveModifier : public ParticleModifier {

public:
	PerpendicularMoveModifier() : ParticleModifier() {

	}

	virtual ~PerpendicularMoveModifier() {}
	void update(ParticleArray* array, const ParticleModifierData* data, float dt) {
		assert(data != 0);
		const PerpendicularMoveModifierData* my_data = static_cast<const PerpendicularMoveModifierData*>(data);
		for ( uint32 i = 0; i < array->countAlive; ++i ) {	
			Vector2f perp = Vector2f(-array->velocity[i].y,array->velocity[i].x);
			perp = normalize(perp);
			perp = perp * (sin(array->timer[i].x * my_data->amplitude) * my_data->radius);// * p->random;	
			//array->position[i] += perp * dt;
		}
	}
	void init(ParticleArray* array, const ParticleModifierData* data, uint32 start, uint32 end) {}
	const ParticleModifierType getType() const {
		return PMT_PERPENDICULAR_MOVE;
	}
	const char* getName() const {
		return "perpendicular_move";
	}
};

// -------------------------------------------------------
// ColorPathModifier
// -------------------------------------------------------
struct ColorPathModifierData : ParticleModifierData {

	ColorPath path;

	void save(JSONWriter& writer) {
		writer.write(path);
	}

	void read(const JSONReader& reader, int category) {
		reader.get_color_path(category, "path", &path);
	}
};

class ColorPathModifier : public ParticleModifier {

public:
	ColorPathModifier() : ParticleModifier() {
	}
	virtual ~ColorPathModifier() {}
	void update(ParticleArray* array, const ParticleModifierData* data, float dt) {
		assert(data != 0);
		const ColorPathModifierData* my_data = static_cast<const ColorPathModifierData*>(data);
		for ( uint32 i = 0; i < array->countAlive; ++i ) {
			my_data->path.get(array->timer[i].y, &array->color[i]);
		}
	}
	void init(ParticleArray* array, const ParticleModifierData* data, uint32 start, uint32 end) {
		assert(data != 0);
		const ColorPathModifierData* my_data = static_cast<const ColorPathModifierData*>(data);
		for (uint32 i = start; i < end; ++i) {
			array->color[i] = my_data->path.value(0);
		}
	}
	const ParticleModifierType getType() const {
		return PMT_COLOR_PATH;
	}
	const char* getName() const {
		return "color_path";
	}
};

// -------------------------------------------------------
// AlphaPathModifier
// -------------------------------------------------------
struct AlphaPathModifierData : ParticleModifierData{

	FloatArray path;

	void save(JSONWriter& writer) {
		writer.write(path);
	}

};

class AlphaPathModifier : public ParticleModifier {

public:
	AlphaPathModifier() : ParticleModifier() {
	}
	virtual ~AlphaPathModifier() {}
	void update(ParticleArray* array, const ParticleModifierData* data, float dt) {
		assert(data != 0);
		const AlphaPathModifierData* my_data = static_cast<const AlphaPathModifierData*>(data);
		for (uint32 i = 0; i < array->countAlive; ++i) {
			//array->color[i].a = data.path.get(array->timer[i].y) / 255.0f;
		}
	}
	void init(ParticleArray* array, const ParticleModifierData* data, uint32 start, uint32 end) {
		assert(data != 0);
		const AlphaPathModifierData* my_data = static_cast<const AlphaPathModifierData*>(data);
		for (uint32 i = start; i < end; ++i) {
			//array->color[i].a = data.path.get(0.0f);
		}
	}
	const ParticleModifierType getType() const {
		return PMT_ALPHA_PATH;
	}
	const char* getName() const {
		return "alpha_path";
	}
};

// -------------------------------------------------------
// DampingVelocityModifier
// -------------------------------------------------------
struct DampingVelocityModifierData : ParticleModifierData {

	float damping;

	DampingVelocityModifierData() : damping(0.0f) {}

	void save(JSONWriter& writer) {
		writer.write("damping", damping);
	}

	void read(const JSONReader& reader, int category) {
		reader.get_float(category, "damping", &damping);
	}

};

class DampingVelocityModifier : public ParticleModifier {

public:
	DampingVelocityModifier() : ParticleModifier() {
	}
	virtual ~DampingVelocityModifier() {}
	void update(ParticleArray* array, const ParticleModifierData* data, float dt) {
		assert(data != 0);
		const DampingVelocityModifierData* my_data = static_cast<const DampingVelocityModifierData*>(data);
		float d = my_data->damping * 0.01f;
		for ( uint32 i = 0; i < array->countAlive; ++i ) {
			Vector3f v = array->velocity[i] * d;
			array->velocity[i] -= v * dt;
		}
	}
	void init(ParticleArray* array, const ParticleModifierData* data, uint32 start, uint32 end) {}
	const ParticleModifierType getType() const {
		return PMT_DAMPING_VELOCITY;
	}
	const char* getName() const {
		return "damping_velocity";
	}
};

// -------------------------------------------------------
// SizePathModifier
// -------------------------------------------------------
struct SizePathModifierData : ParticleModifierData{

	Vector2fPath path;

	void save(JSONWriter& writer) {
		writer.write(path);
	}

	void read(const JSONReader& reader, int category) {
		reader.get_vec2_path(category, "path", &path);
	}

};

class SizePathModifier : public ParticleModifier {

public:
	SizePathModifier() : ParticleModifier() {
	}
	virtual ~SizePathModifier() {}
	void update(ParticleArray* array, const ParticleModifierData* data, float dt) {
		assert(data != 0);
		const SizePathModifierData* my_data = static_cast<const SizePathModifierData*>(data);
		for ( uint32 i = 0; i < array->countAlive; ++i ) {
			my_data->path.get(array->timer[i].y,&array->scale[i]);		
			array->scale[i].x *= array->baseScale[i].x;
			array->scale[i].y *= array->baseScale[i].y;
		}
	}
	void init(ParticleArray* array, const ParticleModifierData* data, uint32 start, uint32 end) {
		assert(data != 0);
		const SizePathModifierData* my_data = static_cast<const SizePathModifierData*>(data);
		for (uint32 i = start; i < end; ++i) {
			array->scale[i] = array->baseScale[i] + my_data->path.value(0);
		}
	}
	const ParticleModifierType getType() const {
		return PMT_SIZE_PATH;
	}
	const char* getName() const {
		return "size_path";
	}
};

// -------------------------------------------------------
// VelocityRotationModifier
// -------------------------------------------------------
class VelocityRotationModifier : public ParticleModifier {

public:
	VelocityRotationModifier() : ParticleModifier() {}
	virtual ~VelocityRotationModifier() {}

	void update(ParticleArray* array, const ParticleModifierData* data, float dt) {
		for ( uint32 i = 0; i < array->countAlive; ++i ) {
			//array->rotation[i] = math::getTargetAngle(V2_RIGHT,array->velocity[i]);			
		}
	}
	void init(ParticleArray* array, const ParticleModifierData* data, uint32 start, uint32 end) {}
	const ParticleModifierType getType() const {
		return PMT_VELOCITY_ROTATION;
	}
	const char* getName() const {
		return "velocity_rotation";
	}
};

// -------------------------------------------------------
// RotationModifier
// -------------------------------------------------------
class RotationModifier : public ParticleModifier {

public:
	RotationModifier() : ParticleModifier() {}
	virtual ~RotationModifier() {}

	void update(ParticleArray* array, const ParticleModifierData* data, float dt) {
		for (uint32 i = 0; i < array->countAlive; ++i) {
			array->rotation[i] += array->rotationVelocity[i] * dt;
		}
	}
	void init(ParticleArray* array, const ParticleModifierData* data, uint32 start, uint32 end) {}
	const ParticleModifierType getType() const {
		return PMT_ROTATION;
	}
	const char* getName() const {
		return "rotation";
	}
};

namespace modifier {

	ParticleModifier* create_by_name(const char* name);
}

}
