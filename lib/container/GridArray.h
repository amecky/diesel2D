#pragma once
#include "..\..\dxstdafx.h"

namespace ds {

template <class type,uint32 WIDTH,uint32 HEIGHT> 
class GridArray {

public:
	GridArray() {}
	
	explicit GridArray(const type& clearValue) {
		clear(clearValue);
	}
	~GridArray() {}

	void clear(const type& t) {
		for ( uint32 x = 0; x < WIDTH; ++x ) {
			for ( uint32 y = 0; y < HEIGHT; ++y ) {
				m_Data[x][y] = t;
			}
		}		
	}

	type& get(uint32 row, uint32 col) {
		return m_Data[row][col];
	}

	const type& get(uint32 row, uint32 col) const {
		return m_Data[row][col];
	}

	void set(uint32 row, uint32 col,const type& t) {
		m_Data[row][col] = t;
	}
	
	bool isValidRow(uint32 row) const {
		if ( row >= 0 && row < WIDTH ) {
			return true;
		}
		return false;
	}

	bool isValidColumn(int col) const {
		if ( col >= 0 && col < HEIGHT ) {
			return true;
		}
		return false;
	}

	bool isValid(int row,int col) const {
		if ( !isValidRow(row)) {
			return false;
		}
		if ( !isValidColumn(col)) {
			return false;
		}
		return true;
	}
	const uint32 getWidth() const {
		return m_Width;
	}
	const uint32 getHeight() const {
		return m_Height;
	}
	uint32 getIndex(uint32 x,uint32 y) {
		return x + y * m_Height;
	}
private:
	uint32 m_Width;
	uint32 m_Height;
	type m_Data[WIDTH][HEIGHT];
};

}