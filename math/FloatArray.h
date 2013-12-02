#pragma once

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
	FloatArray();
	~FloatArray();
	void add(float timeStep,float value);
	float get(float time);
	void reset() {
		m_Count = 0;
	}
	int size() {
		return m_Count;
	}
	const float key(int index) const {
		return m_Array[index].time;
	}
	const float value(int index) const {
		return m_Array[index].value;
	}
	void setInterpolationMode(const PathInterpolation& interpolation) {
		m_Interpolation = interpolation;
	}
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
