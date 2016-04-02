#include "EventStream.h"
#include "..\utils\Log.h"
#include "..\memory\DefaultAllocator.h"

namespace ds {

EventStream::EventStream() {
	m_Data = (char*)ALLOC(4096);
	reset();
}

EventStream::~EventStream() {
	DEALLOC(m_Data);
}

// -------------------------------------------------------
// reset
// -------------------------------------------------------
void EventStream::reset() {
	_mappings.clear();
	_index = 0;
}

// -------------------------------------------------------
// add event
// -------------------------------------------------------
void EventStream::add(uint32_t type, void* p, size_t size) {
	addHeader(type, size);	
	char* data = m_Data + _index + EVENT_HEADER_SIZE;
	memcpy(data,p,size);
	_mappings.push_back(_index);
	_index += EVENT_HEADER_SIZE + size;
}

// -------------------------------------------------------
// add event
// -------------------------------------------------------
void EventStream::add(uint32_t type) {
	addHeader(type, 0);
	char* data = m_Data + _index;
	_mappings.push_back(_index);
	_index += EVENT_HEADER_SIZE;
}

// -------------------------------------------------------
// add header
// -------------------------------------------------------
void EventStream::addHeader(uint32_t type, size_t size) {
	//LOG << "creating event - type: " << type << " size: " << size;
	EventHeader header;
	header.id = _mappings.size();;
	header.size = size;
	header.type = type;
	char* data = m_Data + _index;
	memcpy(data, &header, EVENT_HEADER_SIZE);
}

// -------------------------------------------------------
// get
// -------------------------------------------------------
const bool EventStream::get(uint32_t index, void* p) const {
	XASSERT(index < _mappings.size(), "Index out of range");
	int lookup = _mappings[index];
	char* data = m_Data + lookup;
	EventHeader* header = (EventHeader*)data;
	data += EVENT_HEADER_SIZE;
	memcpy(p,data,header->size);
	return true;
}

// -------------------------------------------------------
// get type
// -------------------------------------------------------
const int EventStream::getType(uint32_t index) const {
	XASSERT(index < _mappings.size(), "Index out of range");
	int lookup = _mappings[index];
	char* data = m_Data + lookup;
	EventHeader* header = (EventHeader*)data;
	return header->type;
}

// -------------------------------------------------------
// contains type
// -------------------------------------------------------
const bool EventStream::containsType(uint32_t type) const {
	for (int i = 0; i < _mappings.size(); ++i) {
		if ( getType(i) == type ) {
			return true;
		}
	}
	return false;
}


}