/* 
 * File:   System.h
 * Author: meckya
 *
 * Created on 21. Juli 2014, 09:32
 */

#ifndef SYSTEM_H
#define	SYSTEM_H

#include "DataContainer.h"

namespace ds {

	struct FlowEvents;

	class System {

	public:
		System();
		System(const System& orig);
		virtual ~System();

		virtual void connect(DataContainer& data, uint32 input, uint32 channel) {}

		virtual void tick(DataContainer& data, float dt,FlowEvents& events) = 0;

		bool isConnected(SystemBits connections) {
			return connections.test(m_SystemIndex);
		}

		void setSystemIndex(int idx) {
			m_SystemIndex = idx;
		}
		const int getSystemIndex() const {
			return m_SystemIndex;
		}
	protected:
		int m_SystemIndex;
	};

	class MoveSystem : public System {

	public:
		MoveSystem() : System() {}
		virtual ~MoveSystem() {}
		void connect(DataContainer& data, uint32 input, uint32 channel);
		void tick(DataContainer& data, float dt, FlowEvents& events);
	private:
		int* m_Pos;
		int* m_Vel;
	};	
}
#endif	/* SYSTEM_H */

