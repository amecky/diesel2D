#pragma once

typedef unsigned int MAP_ID;

template<class VALUE>
class IDMap {

public:
	IDMap() : m_Buffer(0) , m_Total(0) {}
	~IDMap() {
		if ( m_Buffer != 0 ) {
			delete[] m_Buffer;
		}
	}
	void add(MAP_ID id,const VALUE& v) {
		if ( (m_Num + 1) >= m_Total ) {
			allocate(m_Total * 2);
		}
	}
	VALUE& get(MAP_ID id) {
		int idx = findIndex(id);
		return m_Values[idx];
	}
	const VALUE& get(MAP_ID) const {
		int idx = findIndex(id);
		return m_Values[idx];
	}
	void remove(MAP_ID id) {

	}
	const int num() const {
		return m_Num;
	}
	const int size() const {
		return m_Total;
	}
private:
	void allocate(int sz) {
		if ( sz > m_Total ) {
			int size = sz * (sizeof(VALUE) + sizeof(MAP_ID));
			char* buffer = new char[size];
			m_Values = (VALUE*)(buffer);
			m_IDs = (MAP_ID*)(m_Values + sz);
			if ( m_Buffer != 0 ) {

			}
			else {
				m_Num = 0;
			}
			m_Buffer = buffer;
			m_Total = sz;
			
		}
	}
	int findIndex(MAP_ID id) {
		return 0;
	}
	VALUE* m_Values;
	MAP_ID* m_IDs;
	char* m_Buffer;
	int m_Total;
	int m_Num;
};

