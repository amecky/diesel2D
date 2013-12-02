#pragma once

const int MAX_STREAM_SIZE = 4096;
const int MAX_EVENTS = 256;
const int EVENT_HEADER_SIZE = 12;

namespace ds {

class EventStream {

	struct EventHeader {
		int id;
		int type;
		size_t size;
	};    
public:
	EventStream();    
	virtual ~EventStream();
	void reset();
	void add(int type,void* p,size_t size);
	const bool get(int index,void* p) const;
	const int getType(int index) const;
	const bool containsType(int type) const;
	const int num() const {
		return m_Count;
	}
private:
	EventStream(const EventStream& orig) {}
	char* m_Data;
	int m_Mapping[MAX_EVENTS];
	int m_Index;
	int m_Count;
};

}

