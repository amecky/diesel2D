#include "EventBuffer.h"
#include "..\utils\Log.h"

const int BUFFER_SIZE = 16384;

EventBuffer::EventBuffer() {
	m_Data = new char[BUFFER_SIZE];
	clear();
}

void EventBuffer::clear() {
	m_Index = 0;
	m_Offset = 0;
	for ( int i = 0; i < 256; ++i ) {
		m_Indices[i].type = -1;
		m_Indices[i].offset = -1;
		m_Indices[i].size = -1;
	}
}

void EventBuffer::addEvent(int type,const void* e,int size) {
	EventHeader *header = &m_Indices[m_Index];
	header->type = type;
	header->size = size;
	header->offset = m_Offset;
	char* data = m_Data + m_Offset;
	char *ptr = (char *)e;
	size_t to_write = size;	
	while (to_write > 0) {
		*data = *ptr;
		to_write--;
		ptr++;
		++data;
	}
	m_Offset += size;
	++m_Index;
}

const int EventBuffer::size() const {
	return m_Index;
}

void EventBuffer::getData(int idx,void* p,int size) {
	char *ptr = (char *) p;
	EventHeader* header = &m_Indices[idx];
	LOG << "getData : offset: " << header->offset << " size: " << header->size;
	char* data = m_Data + header->offset;
	int to_read = header->size;
	//assert( index < MAX_EB_SIZE);
	while (to_read > 0) {
		*ptr = *data;
		to_read--;
		ptr++;
		++data;
	}
}

void EventBuffer::debug() {
	for ( int i = 0; i < 256; ++i ) {
		EventHeader* header = &m_Indices[i];
		if ( header->offset != -1 ) {
			LOG << i << " : type: " << header->type << " offset: " << header->offset << " size: " << header->size;
		}
	}
}