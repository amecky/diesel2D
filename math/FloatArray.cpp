#include "FloatArray.h"
#include <math.h>
#include "..\utils\Log.h"

namespace ds {

FloatArray::FloatArray() : m_Count(0) , m_LoopMode(PLM_LAST) , m_Interpolation(PI_LINEAR) {
}

FloatArray::~FloatArray() {
}

void FloatArray::add(float timeStep,float value) {
	if ( m_Count < 20 ) {
		PathItem* item = &m_Array[m_Count];
		item->time = timeStep;
		item->value = value;
		++m_Count;
	}
}

float FloatArray::get(float time) {
	if ( m_Count == 0 ) {
		return 0.0f;
	}
	if ( m_Count == 1 ) {
		return m_Array[0].value;
	}
	float normTime = time;
	if ( m_LoopMode == PLM_ZERO ) {
		float maxTime = m_Array[m_Count-1].time;
		if ( normTime > maxTime ) {
			normTime = 0.0f;
		}
	}
	else if ( m_LoopMode == PLM_LAST ) {
		float maxTime = m_Array[m_Count-1].time;
		if ( normTime > maxTime ) {
			normTime = maxTime;
		}
	}
	else if ( m_LoopMode == PLM_LOOP ) {
		float minTime = m_Array[0].time;
		float maxTime = m_Array[m_Count-1].time;       
		normTime = fmod(time,(maxTime-minTime));
	}
	for ( int i = 0; i < m_Count - 1 ; ++i ) {
		PathItem* current = &m_Array[i];
		PathItem* next = &m_Array[i+1];
		if ( normTime >= current->time && normTime <= next->time ) {
			float t = ( normTime - current->time ) / ( next->time - current->time);
			if ( m_Interpolation == PI_LINEAR) {
				return current->value + ( next->value - current->value) * t;
			}
			else if ( m_Interpolation == PI_STEP) {
				return current->value;
			}
			else if ( m_Interpolation == PI_CUBIC ) {
				float prev = current->value;
				if ( i > 0 ) {
					prev = m_Array[i-1].value;
				}
				float nextNext = next->value;
				if ( i < m_Count -2 ) {
					nextNext = m_Array[i+2].value;
				}
				return catmullRom(normTime,prev,current->value,next->value,nextNext);
			}
		}
	}
	return 0.0f;
}

float FloatArray::catmullRom(float t, float v0, float v1, float v2, float v3) {
	const float tt = t*t;
	return 0.5f*((2.0f*v1)+(-v0+v2)*t+(2.0f*v0-5.0f*v1+4.0f*v2-v3)*tt+(-v0+3.0f*(v1-v2)+v3)*tt*t);
}

}
