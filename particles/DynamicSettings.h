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

}
