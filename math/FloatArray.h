#pragma once
#include "math_types.h"
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
	int size() {
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

}
