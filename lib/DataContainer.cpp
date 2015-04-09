#include "DataContainer.h"
#include "..\utils\Log.h"

namespace ds {

DataContainer::DataContainer() : m_Initialized(false) {
	
}

DataContainer::DataContainer(const DataContainer& orig) {
}

DataContainer::~DataContainer() {
	Channels::iterator it = m_DataChannels.begin();
	while (it != m_DataChannels.end()) {
		delete (*it);
		it = m_DataChannels.erase(it);
	}
	if ( m_Initialized ) {
		delete[] m_Buffer;
		delete[] idTable;
		delete[] indices;
	}
}

void DataContainer::clear() {
	numObjects = 0;
	for (unsigned short i = 0; i < maxEntries; ++i) {
		indices[i].id = i;
		indices[i].next = i + 1;
	}
	free_dequeue = 0;
	free_enqueue = maxEntries - 1;
}

void DataContainer::init(const DataContainerElement* elements, int size) {
	maxEntries = size;
	indices = new Index[size];
	clear();
	idTable = new ID[size];

	const DataContainerElement* element = elements;
	int totalBytes = 0;
	while (element->type != -1) {
		switch (element->type) {
			case DAT_INT: {
				totalBytes += sizeof(int);
				AbstractChannel<int>* channel = new AbstractChannel<int>();
				m_DataChannels.push_back(channel);
				break;
			}
			case DAT_FLOAT: {
				totalBytes += sizeof(float);
				AbstractChannel<float>* channel = new AbstractChannel<float>();
				m_DataChannels.push_back(channel);
				break;
			}
			case DAT_VEC2: {
				totalBytes += sizeof(Vector2f);
				AbstractChannel<Vector2f>* channel = new AbstractChannel<Vector2f>();
				m_DataChannels.push_back(channel);
				break;
			}
			case DAT_COLOR: {
				totalBytes += sizeof(ds::Color);
				AbstractChannel<ds::Color>* channel = new AbstractChannel<ds::Color>();
				m_DataChannels.push_back(channel);
				break;
			}
			case DAT_TEXTURE: {
				totalBytes += sizeof(ds::Texture);
				AbstractChannel<ds::Texture>* channel = new AbstractChannel<ds::Texture>();
				m_DataChannels.push_back(channel);
				break;
			}
		}
		++element;
	}
	m_Buffer = new char[totalBytes * size];
	char* ptr = m_Buffer;
	for (size_t i = 0; i < m_DataChannels.size(); ++i) {
		DataChannel* dc = m_DataChannels[i];
		switch (dc->getType()) {
			case DAT_INT: {
				AbstractChannel<int>* channel = static_cast<AbstractChannel<int>*>(m_DataChannels[i]);
				channel->setPtr((int*)ptr);
				ptr += size * sizeof(int);
				break;
			}
			case DAT_FLOAT: {
				AbstractChannel<float>* channel = static_cast<AbstractChannel<float>*>(m_DataChannels[i]);
				channel->setPtr((float*)ptr);
				ptr += size * sizeof(float);
				break;
			}
			case DAT_VEC2: {
				AbstractChannel<Vector2f>* channel = static_cast<AbstractChannel<Vector2f>*>(m_DataChannels[i]);
				channel->setPtr((Vector2f*)ptr);
				ptr += size * sizeof(Vector2f);
				break;
			}
			case DAT_COLOR: {
				AbstractChannel<ds::Color>* channel = static_cast<AbstractChannel<ds::Color>*>(m_DataChannels[i]);
				channel->setPtr((ds::Color*)ptr);
				ptr += size * sizeof(ds::Color);
				break;
			}
			case DAT_TEXTURE: {
				AbstractChannel<ds::Texture>* channel = static_cast<AbstractChannel<ds::Texture>*>(m_DataChannels[i]);
				channel->setPtr((ds::Texture*)ptr);
				ptr += size * sizeof(ds::Texture);
				break;
			}
		}
	}

	m_Initialized = true;
}

const uint32 DataContainer::size() const {
	return numObjects;
}

const bool DataContainer::contains(ID id) const {
	assert(m_Initialized);
	const Index& in = indices[id & INDEX_MASK];
	return in.id == id && in.index != USHRT_MAX;
}

ID DataContainer::create() {
	assert(m_Initialized);
	assert(numObjects < (maxEntries - 1));
	Index &in = indices[free_dequeue];
	free_dequeue = in.next;
	in.index = numObjects++;
	for (int i = 0; i < m_DataChannels.size(); ++i) {
		m_DataChannels[i]->clear(in.index);
	}
	idTable[in.index] = in.id;
	return in.id;
}

void DataContainer::remove(ID id) {
	assert(m_Initialized);
	Index &in = indices[id & INDEX_MASK];
	assert(in.index != USHRT_MAX);
	int old = --numObjects;
	if (in.index != old) {
		for (int i = 0; i < m_DataChannels.size(); ++i) {
			m_DataChannels[i]->swap(in.index, old);
		}
	}
	indices[idTable[old & INDEX_MASK]].index = in.index;
	idTable[in.index] = idTable[old];	
	indices[free_enqueue].next = id & INDEX_MASK;
	free_enqueue = id & INDEX_MASK;
	in.index = USHRT_MAX;
}

void DataContainer::debug() {
	assert(m_Initialized);
	LOG << "------------------------------------------------";
	LOG << "numObjects  : " << numObjects;
	LOG << "free_enqueue: " << free_enqueue;
	LOG << "free_dequeue: " << free_dequeue;
	for ( int i = 0; i < maxEntries;++i ) {
		LOG << i << " = " << indices[i].id << " / " << indices[i].index;
	}
	for (int i = 0; i < numObjects; ++i) {
		LOG << i << " id: " << idTable[i] << " index: " << indices[idTable[i]].index;
		if (indices[idTable[i]].index != USHRT_MAX) {
			for (int j = 0; j < m_DataChannels.size(); ++j) {
				if (getDataType(j) == DAT_INT) {
					int tmp = get<int>(idTable[i], j);
					LOG << "  data " << j << " '" << getName(j) << "' (int)      : " << tmp;
				}
				else if (getDataType(j) == DAT_FLOAT) {
					float tmp = get<float>(idTable[i], j);
					LOG << "  data " << j << " '" << getName(j) << "' (float)    : " << tmp;
				}
				else if (getDataType(j) == DAT_VEC2) {
					Vector2f tmp = get<Vector2f>(idTable[i], j);
					LOG << "  data " << j << " '" << getName(j) << "' (Vector2f) : " << tmp.x << " " << tmp.y;
				}
				else if (getDataType(j) == DAT_TEXTURE) {
					ds::Texture tmp = get<ds::Texture>(idTable[i], j);
					LOG << "  data " << j << " '" << getName(j) << "' (Texture)  : " << tmp.dim.x << " " << tmp.dim.y;
				}
				else if (getDataType(j) == DAT_COLOR) {
					ds::Color tmp = get<ds::Color>(idTable[i], j);
					LOG << "  data " << j << " '" << getName(j) << "' (Color)    : " << DBG_CLR(tmp);
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