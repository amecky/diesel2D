#pragma once

namespace ds {

class Bitset {

public:
	Bitset() : m_Bits(0) {}
	~Bitset() {}
	void set(int index) {
		m_Bits |= 1 << index;
	}
	void clear(int index) {
		m_Bits &=  ~(1 << index);
	}
	const bool isSet(int index) const {
		int v = 1 << index;
		return ( m_Bits & v) == v;
	}
	void toggle(int index) {
		m_Bits ^= 1 << index;
	}
	void clear() {
		m_Bits = 0;
	}
	void fill(unsigned int value) {
		m_Bits = value;
	}
	const unsigned int getValue() const {
		return m_Bits;
	}
private:
	unsigned int m_Bits;
};

}
