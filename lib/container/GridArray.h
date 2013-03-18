#pragma once
#include "..\..\dxstdafx.h"
#include "..\..\utils\Log.h"

namespace ds {

template <class type> class GridArray {

public:
	GridArray() {
		m_Rows = 0;
		m_Cols = 0;
		m_Data = 0;
	}
	GridArray(uint32 rows,uint32 cols) {
		m_Rows = rows;
		m_Cols = cols;
		m_Data = new type[ rows * cols ];
	}
	GridArray(uint32 rows,uint32 cols,const type& clearValue) {
		m_Rows = rows;
		m_Cols = cols;
		m_Data = new type[ rows * cols ];
		clear(clearValue);
	}
	~GridArray() {
		if ( m_Data != 0 ) {
			LOG(logINFO) << "deleting data of grid array";
			delete[] m_Data;
		}
	}
	void create(uint32 rows,uint32 cols) {
		m_Rows = rows;
		m_Cols = cols;
		if ( m_Data != 0 ) {
			delete[] m_Data;
		}
		m_Data = new type[ rows * cols ];
	}
	void clear(const type& t) {
		uint32 sz = size();
		if ( sz > 0 ) {
			for ( uint32 i = 0; i < sz; ++i ) {
				m_Data[i] = t;
			}
		}
	}
	type& get(uint32 row, uint32 col) {
		return m_Data[row * m_Cols + col];
	}
	void set(uint32 row, uint32 col,const type& t) {
		m_Data[row * m_Cols + col] = t;
	}
	uint32 size() const {
		return m_Rows * m_Cols;
	}
	uint32 rows() const {
		return m_Rows;
	}
	uint32 columns() const {
		return m_Cols;
	}
	bool isValidRow(int row) const {
		if ( row >= 0 && row < (int)m_Rows ) {
			return true;
		}
		return false;
	}
	bool isValidColumn(int col) const {
		if ( col >= 0 && col < (int)m_Cols ) {
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
private:
	uint32 m_Rows;
	uint32 m_Cols;
	type* m_Data;
};

}