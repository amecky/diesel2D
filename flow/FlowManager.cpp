/* 
 * File:   World.cpp
 * Author: meckya
 * 
 * Created on 21. Juli 2014, 09:44
 */

#include "FlowManager.h"
#include "..\utils\Profiler.h"
#include "..\utils\Log.h"

namespace ds {

	FlowManager::FlowManager() {
	}

	FlowManager::FlowManager(const FlowManager& orig) {
	}

	FlowManager::~FlowManager() {
	}

	void FlowManager::remove(ID id) {
		m_Container.remove(id);
	}

	int FlowManager::registerSystem(System* system) {
		int current = m_Systems.size();
		system->setSystemIndex(current);
		m_Systems.push_back(system);
		return current;
	}

	int FlowManager::registerSystem(System* system, uint32* channels, int numChannels) {
		int current = m_Systems.size();
		system->setSystemIndex(current);
		m_Systems.push_back(system);
		for (int i = 0; i < numChannels; ++i) {
			system->connect(m_Container, i, channels[i]);
		}
		return current;
	}

	void FlowManager::tick(float dt) {
		PRS("World::tick")
		m_Events.size = 0;
		for (int i = 0; i < m_Systems.size(); ++i) {
			m_Systems[i]->tick(m_Container, dt,m_Events);
		}
		PRE("World::tick")
	}

	ID FlowManager::create() {
		return m_Container.create();
	}

	void FlowManager::connect(ID id, int systemIndex) {
		m_Container.connect(id, systemIndex);
	}

	void FlowManager::connect(ID id, int* systemIndices, int num) {
		for (int i = 0; i < num; ++i) {
			m_Container.connect(id, systemIndices[i]);
		}
	}

	void FlowManager::disconnect(ID id, int systemIndex) {
		m_Container.disconnect(id, systemIndex);
	}

	const bool FlowManager::isConnected(ID id, int systemIndex) const {
		return m_Container.isConnected(id, systemIndex);
	}

	void FlowManager::createChannels(DataContainerElement* elements) {
		int cnt = 0;
		while (elements[cnt].type != -1) {
			switch (elements[cnt].type) {
			case DAT_INT: m_Container.addDataChannel<int>(); break;
			case DAT_FLOAT: m_Container.addDataChannel<float>(); break;
			case DAT_VEC2: m_Container.addDataChannel<Vector2f>(); break;
			case DAT_TEXTURE: m_Container.addDataChannel<Texture>(); break;
			case DAT_COLOR: m_Container.addDataChannel<Color>(); break;
			}
			++cnt;
		}
	}

	void FlowManager::debug(ID id) {
		LOG << "Entity: " << id;
		for (int i = 0; i < m_Container.channels(); ++i) {
			if (m_Container.getDataType(i) == DAT_INT) {
				int tmp = m_Container.get<int>(id, i);
				LOG << i << " (int)      : " << tmp;
			}
			else if (m_Container.getDataType(i) == DAT_FLOAT) {
				float tmp = m_Container.get<float>(id, i);
				LOG << i << " (float)    : " << tmp;
			}
			else if (m_Container.getDataType(i) == DAT_VEC2) {
				Vector2f tmp = m_Container.get<Vector2f>(id, i);
				LOG << i << " (Vector2f) : " << tmp.x << " " << tmp.y;
			}
			else if (m_Container.getDataType(i) == DAT_TEXTURE) {
				Texture tmp = m_Container.get<Texture>(id, i);
				LOG << i << " (Texture)  : " << tmp.dim.x << " " << tmp.dim.y;
			}
		}
		for (int i = 0; i < m_Systems.size(); ++i) {
			if (isConnected(id, m_Systems[i]->getSystemIndex())) {
				LOG << "connected to:     " << m_Systems[i]->getSystemIndex();
			}
			else {
				LOG << "Not connected to: " << m_Systems[i]->getSystemIndex();
			}
		}
	}

}