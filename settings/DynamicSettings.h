#pragma once
#include "..\utils\PlainTextReader.h"
#include "..\math\FloatArray.h"
#include "..\utils\Color.h"

namespace ds {

struct DynamicSettings {

	DynamicSettings() {}
	virtual ~DynamicSettings() {}
	virtual void load(NewSettingsReader& reader) = 0;

};

// -------------------------------------------------------
// FloatPath
// -------------------------------------------------------
struct FloatPath : public DynamicSettings {

	FloatArray array;

	void add(float timeStep,float v) {
		array.add(timeStep,v);
	} 
	void update(float time,float* v) {
		*v = array.get(time);
	}
	void reset() {
		array.reset();
	}
	int size() {
		return array.size();
	}
	void load(NewSettingsReader& reader) {
		reader.get("v_steps","t_steps",&array);
	}
	void setInterpolationMode(const PathInterpolation& interpolation) {
		array.setInterpolationMode(interpolation);
	}
	void setLoopMode(const PathLoopMode& loopMode) {
		array.setLoopMode(loopMode);
	}
};

// -------------------------------------------------------
// Vec2Path
// -------------------------------------------------------
struct Vec2Path : public DynamicSettings {

	FloatArray x;
	FloatArray y;

	void add(float timeStep,const Vec2& v) {
		x.add(timeStep,v.x);
		y.add(timeStep,v.y);
	} 
	void update(float time,Vec2* v) {
		v->x = x.get(time);
		v->y = y.get(time);		
	}
	void reset() {
		x.reset();
		y.reset();
	}
	int size() {
		return x.size();
	}
	void load(NewSettingsReader& reader) {
		reader.get("steps_x","steps_t",&x);
		reader.get("steps_y","steps_t",&y);
	}
	void setInterpolationMode(const PathInterpolation& interpolation) {
		x.setInterpolationMode(interpolation);
		y.setInterpolationMode(interpolation);
	}
	void setLoopMode(const PathLoopMode& loopMode) {
		x.setLoopMode(loopMode);
		y.setLoopMode(loopMode);
	}
};

// -------------------------------------------------------
// ColorPath
// -------------------------------------------------------
struct ColorPath : public DynamicSettings {

	FloatArray red;
	FloatArray green;
	FloatArray blue;
	FloatArray alpha;

	void add(float timeStep,const Color& color) {
		red.add(timeStep,color.r);
		green.add(timeStep,color.g);
		blue.add(timeStep,color.b);
		alpha.add(timeStep,color.a);
	} 
	void update(float time,Color* color) {
		float norm = alpha.get(time);
		color->r = red.get(time) * norm;
		color->g = green.get(time) * norm;
		color->b = blue.get(time) * norm;
		color->a = norm;		
	}
	int size() {
		return red.size();
	}
	void reset() {
		red.reset();
		green.reset();
		blue.reset();
		alpha.reset();
	}
	void load(NewSettingsReader& reader) {
		reader.get("r_array","t_steps",&red);
		reader.get("g_array","t_steps",&green);
		reader.get("b_array","t_steps",&blue);
		reader.get("a_array","t_steps",&alpha);		
	}

	void setInterpolationMode(const PathInterpolation& interpolation) {
		red.setInterpolationMode(interpolation);
		green.setInterpolationMode(interpolation);
		blue.setInterpolationMode(interpolation);
		alpha.setInterpolationMode(interpolation);
	}
	void setLoopMode(const PathLoopMode& loopMode) {
		red.setLoopMode(loopMode);
		green.setLoopMode(loopMode);
		blue.setLoopMode(loopMode);
		alpha.setLoopMode(loopMode);
	}

};


}
