#pragma once
#include "..\utils\PlainTextReader.h"
#include "..\math\FloatArray.h"
#include "..\utils\Color.h"

namespace ds {

// -------------------------------------------------------
// FloatPath
// -------------------------------------------------------
struct FloatPath {

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
	
	void setInterpolationMode(const PathInterpolation& interpolation) {
		array.setInterpolationMode(interpolation);
	}
	void setLoopMode(const PathLoopMode& loopMode) {
		array.setLoopMode(loopMode);
	}
};

// -------------------------------------------------------
// Vector2fPath
// -------------------------------------------------------
struct Vector2fPath {

	FloatArray x;
	FloatArray y;

	void add(float timeStep,const Vector2f& v) {
		x.add(timeStep,v.x);
		y.add(timeStep,v.y);
	} 
	void update(float time,Vector2f* v) {
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
struct ColorPath {

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
