/* 
 * File:   System.cpp
 * Author: meckya
 * 
 * Created on 21. Juli 2014, 09:32
 */

#include "System.h"
#include "FlowManager.h"
#include "..\utils\Log.h"
#include "..\utils\Profiler.h"

namespace ds {

	System::System() : m_SystemIndex(0) {
	}

	System::System(const System& orig) {
	}

	System::~System() {
	}

	void MoveSystem::connect(DataContainer& data, uint32 input, uint32 channel) {
		// both must be int
		if (data.getDataType(channel) != DAT_INT) {
			LOGE << "------ ERROR -------";
		}
		switch (input) {
		case 0: m_Pos = data.getDataChannel<int>(0); break;
		case 1: m_Vel = data.getDataChannel<int>(1); break;
		}
	}

	void MoveSystem::tick(DataContainer& data, float dt, FlowEvents& events) {
		PRS("MS::tick")
		SystemBits* connections = data.getConnections();		
		PRS("MS::tick::move")
		int* pos = m_Pos;
		int* vel = m_Vel;
		for (int i = 0; i < data.size(); ++i) {
			if (isConnected(connections[i])) {
				*pos += *vel * dt;
			}
			++pos;
			++vel;
		}
		PRE("MS::tick::move")
			PRE("MS::tick")
	}

}
