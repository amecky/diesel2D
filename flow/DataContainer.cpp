/* 
 * File:   DataContainer.cpp
 * Author: meckya
 * 
 * Created on 21. Juli 2014, 09:31
 */

#include "DataContainer.h"
#include "..\utils\Log.h"

namespace ds {

	DataContainer::DataContainer() {
		numObjects = 0;
		for (unsigned short i = 0; i < MAX_CONTAINER_OBJECTS; ++i) {
			indices[i].id = i;
			indices[i].next = i + 1;
		}
		free_dequeue = 0;
		free_enqueue = MAX_CONTAINER_OBJECTS - 1;
	}

	DataContainer::DataContainer(const DataContainer& orig) {
	}

	DataContainer::~DataContainer() {
		Channels::iterator it = m_DataChannels.begin();
		while (it != m_DataChannels.end()) {
			delete (*it);
			it = m_DataChannels.erase(it);
		}
	}

	uint32 DataContainer::size() {
		return numObjects;
	}

	const bool DataContainer::contains(ID id) const {
		const Index& in = indices[id & INDEX_MASK];
		return in.id == id && in.index != USHRT_MAX;
	}

	ID DataContainer::create() {
		assert(numObjects < (MAX_CONTAINER_OBJECTS - 1));
		//LOG << "create - free_dequeue: " << free_dequeue;
		Index &in = indices[free_dequeue];
		free_dequeue = in.next;
		in.index = numObjects++;
		for (int i = 0; i < m_DataChannels.size(); ++i) {
			m_DataChannels[i]->clear(in.index);
		}
		connections[in.index].reset();
		idTable[in.index] = in.id;
		//LOG << "create - new size: " << numObjects;
		return in.id;
	}

	void DataContainer::remove(ID id) {
		Index &in = indices[id & INDEX_MASK];
		assert(in.index != USHRT_MAX);
		int old = --numObjects;
		//LOG << "removing - id: " << id << " index: " << in.index << " old: " << old << " numObjects: " << numObjects;
		if (in.index != old) {
			for (int i = 0; i < m_DataChannels.size(); ++i) {
				m_DataChannels[i]->swap(in.index, old);
			}
			connections[in.index] = connections[old];
			indices[old & INDEX_MASK].index = in.index;
			idTable[in.index] = idTable[old];
		}
		indices[free_enqueue].next = id & INDEX_MASK;
		free_enqueue = id & INDEX_MASK;
		in.index = USHRT_MAX;
	}

	void DataContainer::debug() {
		LOG << "------------------------------------------------";
		LOG << "numObjects  : " << numObjects;
		LOG << "free_enqueue: " << free_enqueue;
		LOG << "free_dequeue: " << free_dequeue;
		for (int i = 0; i < numObjects; ++i) {
			LOG << i << " id: " << idTable[i] << " index: " << indices[idTable[i]].index;
			if (indices[idTable[i]].index != USHRT_MAX) {
				for (int j = 0; j < m_DataChannels.size(); ++j) {
					if (getDataType(j) == DAT_INT) {
						int tmp = get<int>(idTable[i], j);
						LOG << "  data " << j << " (int)      : " << tmp;
					}					
					else if (getDataType(j) == DAT_FLOAT) {
						float tmp = get<float>(idTable[i], j);
						LOG << "  data " << j << " (float)    : " << tmp;
					}
					else if (getDataType(j) == DAT_VEC2) {
						Vector2f tmp = get<Vector2f>(idTable[i], j);
						LOG << "  data " << j << " (Vector2f) : " << tmp.x << " " << tmp.y;
					}
					else if (getDataType(j) == DAT_TEXTURE) {
						Texture tmp = get<Texture>(idTable[i], j);
						LOG << "  data " << j << " (Texture)  : " << tmp.dim.x << " " << tmp.dim.y;
					}
				}
			}
			else {
				LOG << "  >> no data";
			}
		}
		LOG << "------------------------------------------------";
	}

}