#pragma once

struct EventHeader {
	int type;
	int size;
	int offset;
};

class EventBuffer {

public:
	EventBuffer();
	~EventBuffer() {}
	void addEvent(int type,const void* e,int size);
	const int size() const;
	int getType(int idx);
	void getData(int idx,void* p,int size);
	void clear();
	void debug();
private:
	int m_Index;
	int m_Offset;
	char* m_Data;
	EventHeader m_Indices[256];

};
