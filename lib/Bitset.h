#pragma once

namespace ds {

template<int SIZE>
class Bitset {

public:
	Bitset() {
		numBuckets = SIZE / 32;
	}
	~Bitset() {}

	void set(int index) {
		int bucketIndex = index / 32;
		int pos = index % 32;
		m_Buckets[bucketIndex] |= 1 << pos;
	}
	void clear(int index) {
		int bucketIndex = index / 32;
		int pos = index % 32;
		m_Buckets[bucketIndex] &= ~(1 << pos);
	}
	bool test(int index) const {
		int bucketIndex = index / 32;
		int pos = index % 32;
		int bit = 1 << pos;
		return ( m_Buckets[bucketIndex] & bit ) == bit;
	}
	void reset() {
		for ( int i = 0; i < numBuckets; ++i ) {
			m_Buckets[i] = 0;
		}
	}
	int count() {
		return 0;
	}
	int size() {
		return SIZE;
	}
	bool any() {
		for ( int i = 0; i < SIZE; ++i ) {
			if ( test(i)) {
				return true;
			}
		}
		return false;
	}
	bool none() {
		return !any();
	}
	bool all() {
		return false;
	}
	void toggle(int index) {
		int bucketIndex = index / 32;
		int pos = index % 32;
		m_Buckets[bucketIndex] ^= 1 << pos;
	}
private:
	int numBuckets;
	int m_Buckets[SIZE / 32];
};

}