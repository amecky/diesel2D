/* 
 * File:   World.h
 * Author: meckya
 *
 * Created on 21. Juli 2014, 09:44
 */

#ifndef WORLD_H
#define	WORLD_H
#include <vector>
#include "System.h"
#include "DataContainer.h"

namespace ds {

	struct FlowEvents {

		int size;
		ID ids[1024];
		int types[1024];

		void add(ID id, int type) {
			int idx = size++;
			ids[idx] = id;
			types[idx] = type;
		}
		
	};

	class FlowManager {

		typedef std::vector<System*> Systems;

	public:
		FlowManager();
		FlowManager(const FlowManager& orig);
		virtual ~FlowManager();
		int registerSystem(System* system);

		int registerSystem(System* system, uint32* channels, int numChannels);

		void createChannels(DataContainerElement* elements);

		template<class T>
		int addChannel() {
			return m_Container.addDataChannel<T>();
		}

		void connectChannel(int systemIndex, uint32 input, uint32 channel) {
			System* system = m_Systems[systemIndex];
			system->connect(m_Container, input, channel);
		}

		void tick(float dt);
		ID create();
		void remove(ID id);
		template<class T>
		void set(ID id, int channel, const T& v) {
			m_Container.set<T>(id, channel, v);
		}

		template<class T>
		T& get(ID id, int channel) {
			return m_Container.get<T>(id, channel);
		}

		template<class T>
		const T& get(ID id, int channel) const {
			return m_Container.get<T>(id, channel);
		}
		const bool contains(ID id) const {
			return m_Container.contains(id);
		}
		void connect(ID id, int systemIndex);
		void connect(ID id, int* systemIndices, int num);
		void disconnect(ID id, int systemIndex);
		const bool isConnected(ID id, int systemIndex) const;
		void debug(ID id);
		const FlowEvents& getEvents() const {
			return m_Events;
		}
		const bool hasEvents() const {
			return m_Events.size > 0;
		}
		DataContainer& getDataContainer() {
			return m_Container;
		}
		void debugContainer() {
			m_Container.debug();
		}
	private:
		Systems m_Systems;
		DataContainer m_Container;
		FlowEvents m_Events;
	};

}
#endif	/* WORLD_H */

