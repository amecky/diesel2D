#include "EventStream.h"
#include <string.h>

namespace ds {

EventStream::EventStream() {
	m_Data = new char[4096];
	reset();
}

EventStream::~EventStream() {
	delete m_Data;
}

void EventStream::reset() {
	m_Index = 0;
	m_Count = 0;
}

void EventStream::add(int type,void* p, size_t size) {
	if ( m_Count < MAX_EVENTS) {
		EventHeader header;
		header.id = m_Count;
		header.size = size;
		header.type = type;
		char* data = m_Data + m_Index;
		memcpy(data,&header,EVENT_HEADER_SIZE);
		data += EVENT_HEADER_SIZE;
		memcpy(data,p,size);
		m_Mapping[m_Count] = m_Index;
		m_Index += EVENT_HEADER_SIZE + size;
		++m_Count;
	}
}

void EventStream::add(int type) {
	if ( m_Count < MAX_EVENTS) {
		EventHeader header;
		header.id = m_Count;
		header.size = 0;
		header.type = type;
		char* data = m_Data + m_Index;
		memcpy(data,&header,EVENT_HEADER_SIZE);		
		m_Mapping[m_Count] = m_Index;
		m_Index += EVENT_HEADER_SIZE;
		++m_Count;
	}
}

const bool EventStream::get(int index,void* p) const {
	if ( index < 0 || index >= m_Count ) {
		return false;
	}
	int lookup = m_Mapping[index];
	char* data = m_Data + lookup;
	EventHeader* header = (EventHeader*)data;
	data += EVENT_HEADER_SIZE;
	memcpy(p,data,header->size);
	return true;
}

const int EventStream::getType(int index) const {
	if ( index < 0 || index >= m_Count ) {
		return -1;
	}
	int lookup = m_Mapping[index];
	char* data = m_Data + lookup;
	EventHeader* header = (EventHeader*)data;
	return header->type;
}

const bool EventStream::containsType(int type) const {
	for ( int i = 0; i < m_Count; ++i ) {
		if ( getType(i) == type ) {
			return true;
		}
	}
	return false;
}


}