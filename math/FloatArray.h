#pragma once
#include "math_types.h"
#include "..\utils\Color.h"
/*!
 * \class FloatArray
 * 
 * FloatArray is an array of floats
 *
 * \author amecky@gmail.com
 * \date 14.01.2014
 */
namespace ds {

enum PathInterpolation {
	PI_LINEAR,
	PI_STEP,
	PI_CUBIC
};

enum PathLoopMode {
	PLM_ZERO,
	PLM_LAST,
	PLM_LOOP
};

struct PathItem {

	float time;
	float value;
};

const int MAX_FLOAT_ARRAY_ITEMS = 20;

class FloatArray {

public:
	//! Constructor
	/*!
	 * The new created instance will use PLM_LAST as path loop mode and
	 * PI_LINEAR as interpolation mode.
	*/
	FloatArray();
	~FloatArray();
	//! Add value
	/*!
	 * This methods adds a pair of value and timestep to the internal list
	 * \param timeStep the actual timestep
	 * \param value the value
	*/
	void add(float timeStep,float value);
	//! Gets the value at the given time
	float get(float time);
	//! Reset the internal list
	void reset() {
		m_Count = 0;
	}
	//! Returns the number of elements
	/*
	 * \return number of elements
	*/
	const int size() const {
		return m_Count;
	}
	//! Returns the timestep value at the given index
	/*
	 * \return timestep at index
	*/
	const float key(int index) const {
		return m_Array[index].time;
	}
	//! Returns value at the given index
	/*
	 * \return value at index
	*/
	const float value(int index) const {
		return m_Array[index].value;
	}
	//! Sets interpolation mode
	void setInterpolationMode(const PathInterpolation& interpolation) {
		m_Interpolation = interpolation;
	}
	//! Sets the loop mode
	/*!
	 * The loop mode defines the behavior if the timestep used in
	 * the get call exceeds the last timestep in the last. It will
	 * either be set to max or zero or subtract the max from current
	 *
	 * \param loopMode the actual LoopMode that should be used
	*/
	void setLoopMode(const PathLoopMode& loopMode) {
		m_LoopMode = loopMode;
	}
private:
	float catmullRom(float t, float v0, float v1, float v2, float v3);
	PathItem m_Array[MAX_FLOAT_ARRAY_ITEMS];
	PathLoopMode m_LoopMode;
	PathInterpolation m_Interpolation;
	int m_Count;
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
	const float getTimeStep(int idx) const {
		return red.key(idx);
	}
	const Color getColor(int idx) const {
		return Color(red.value(idx),green.value(idx),blue.value(idx),alpha.value(idx));
	}
	const int size() const {
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

	const int size() const {
		return x.size();
	}

	const float getTimeStep(int idx) const {
		return x.key(idx);
	}

	const Vector2f getVec2(int idx) const {
		return Vector2f(x.value(idx),y.value(idx));
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

}
