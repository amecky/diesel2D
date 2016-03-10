#pragma once
#include "..\lib\collection_types.h"
#include "..\math\math_types.h"
#include "..\utils\Log.h"

namespace ds {

// -------------------------------------------------------
// Dropped Cell
// -------------------------------------------------------
template<class T>
struct DroppedCell {

	Point from;
	Point to;
	T data;
};
// ------------------------------------------------
// Grid
// ------------------------------------------------
template<class T>
class Grid {

struct GridNode {
	Point point;
    T data;
    bool used;

	GridNode() : point(-1, -1), used(false) {}

	GridNode(const GridNode& other) : point(other.point), data(other.data), used(other.used) {}

};

public:
    Grid(int width,int height);
    virtual ~Grid() {
        delete[] m_Data;
    }
	void clear();
    void clear(const T& t);
    const T& get(int x,int y) const;
	T& get(int x,int y);
	const T& get(const Point& p) const;
	T& get(const Point& p);
    void set(int x,int y,const T& t);
    bool remove(int x,int y);    
    void remove(const Array<Point>& points,bool shift);  
    const int width() const {
        return m_Width;
    }
    const int height() const {
        return m_Height;
    }
	bool isValid(int x, int y) const {
		if (x >= 0 && x < m_Width && y >= 0 && y < m_Height) {
			return true;
		}
		return false;
	}
	void findMatchingNeighbours(int x, int y, Array<Point>& entries);
	void findMatchingNeighbours(int x, int y, const T& node, Array<Point>& entries);
    void fillRow(int row,const T& t);
    void fillColumn(int column,const T& t);
    void copyRow(int oldRow,int newRow);
    void copyColumn(int oldColumn,int newColumn);    
    void clearColumn(int column);
    void shiftColumns(int startColumn);
    T& operator() (int x,int y);    
	const T& operator() (int x, int y) const;
    const bool isFree(int x,int y) const;
	bool isColumnEmpty(int col);
    void dropRow(int x);
    void dropCell(int x,int y);
	void dropCells(Array<DroppedCell<T>>& droppedCells);
	void swap(const Point& first, const Point& second);
protected:
    virtual bool isMatch(const T& first,const T& right) = 0;
private:
    const int getIndex(int x,int y) const;
	const int getIndex(const Point& p) const;	
	void findMatching(int x, int y, GridNode* providedNode, Array<GridNode*>& gridNodes);
	void simpleFindMatching(int x, int y, GridNode* providedNode, Array<GridNode*>& gridNodes);
    int m_Width;
    int m_Height;
    int m_Size;
    GridNode* m_Data;
};

// ------------------------------------------------
// Create new grid with desired dimension
// ------------------------------------------------
template<class T>
Grid<T>::Grid(int width,int height) : m_Width(width) , m_Height(height) {
    m_Size = width * height;
    m_Data = new GridNode[m_Size];
    for ( int x = 0; x < width; ++x ) {
        for ( int y = 0; y < height; ++y ) {
            int index = getIndex(x,y);
            GridNode* node = &m_Data[index];
			node->point = Point(x, y);
            node->used = false;
        }
    }
}

// ------------------------------------------------
//
// ------------------------------------------------
template<class T>
inline T& Grid<T>::operator() (int x,int y) {
    int index = getIndex(x,y);
    return m_Data[index].data;
}

// ------------------------------------------------
//
// ------------------------------------------------
template<class T>
inline const T& Grid<T>::operator() (int x, int y) const {
	int index = getIndex(x, y);
	return m_Data[index].data;
}


// ------------------------------------------------
// Clears the entire grid with given object
// ------------------------------------------------
template<class T>
inline void Grid<T>::clear() {
	for ( int i = 0; i < m_Size; ++i ) {
		m_Data[i].used = false;
	}
}

// ------------------------------------------------
// Clears the entire grid with given object
// ------------------------------------------------
template<class T>
inline void Grid<T>::clear(const T& t) {
    for ( int i = 0; i < m_Size; ++i ) {
        m_Data[i].data = t;
        m_Data[i].used = true;
    }
}

// ------------------------------------------------
// Gets the object at given position
// ------------------------------------------------
template<class T>
inline const T& Grid<T>::get(int x,int y) const {
    int idx = getIndex(x,y);
    return m_Data[idx].data;
}

// ------------------------------------------------
// Gets the object at given position
// ------------------------------------------------
template<class T>
inline T& Grid<T>::get(const Point& p) {
	int idx = getIndex(p);
	return m_Data[idx].data;
}

// ------------------------------------------------
// Gets the object at given position
// ------------------------------------------------
template<class T>
inline const T& Grid<T>::get(const Point& p) const {
	int idx = getIndex(p);
	return m_Data[idx].data;
}

// ------------------------------------------------
// Gets the object at given position
// ------------------------------------------------
template<class T>
inline T& Grid<T>::get(int x, int y) {
	int idx = getIndex(x, y);
	return m_Data[idx].data;
}
// ------------------------------------------------
//
// ------------------------------------------------
template<class T>
inline void Grid<T>::set(int x,int y,const T& t) {
    int idx = getIndex(x,y);
    if ( idx != -1 ) {
        GridNode* node = &m_Data[idx];
        node->data = t;
        node->used = true;
    }
}

// ------------------------------------------------
// Checks if cell is free
// ------------------------------------------------
template<class T>
inline bool Grid<T>::remove(int x,int y) {
    int idx = getIndex(x,y);
    if ( idx != -1 && m_Data[idx].used ) {
        m_Data[idx].used = false;
        return true;
    }
    return false;
}

// ------------------------------------------------
// Checks if cell is free
// ------------------------------------------------
template<class T>
inline const bool Grid<T>::isFree(int x,int y) const {
    int idx = getIndex(x,y);
    if ( idx != -1 ) {
        return !m_Data[idx].used;
    }
    return false;
}
// ------------------------------------------------
// Returns the index into array if valid or -1
// ------------------------------------------------
template<class T>
inline const int Grid<T>::getIndex(int x, int y) const {
    if ( x < 0 || x >= m_Width ) {
        return -1;
    }
    if ( y < 0 || y >= m_Height ) {
        return -1;
    }
    return y * m_Width + x;
}

// ------------------------------------------------
// Returns the index into array if valid or -1
// ------------------------------------------------
template<class T>
inline const int Grid<T>::getIndex(const Point& p) const {
	return getIndex(p.x, p.y);
}

// ------------------------------------------------
// findMatchingNeighbours
// ------------------------------------------------
template<class T>
inline void Grid<T>::findMatchingNeighbours(int x, int y, Array<Point>& entries) {
    int idx = getIndex(x,y);
	Array<GridNode*> gridNodes;
    if ( idx != -1 ) {    
        GridNode* provided = &m_Data[idx];
        if ( provided->used) {
            findMatching(x,y,provided,gridNodes);
        }
    }
    if ( !gridNodes.empty()) {
        for ( std::size_t i = 0; i < gridNodes.size(); ++i ) {
            GridNode* node = gridNodes[i];
            entries.push_back(node->point);
        }
    }
}

template<class T>
inline void Grid<T>::findMatchingNeighbours(int x, int y, const T& node, Array<Point>& entries) {
	Array<GridNode*> gridNodes;
	if ( !isFree(x,y)) {
		GridNode tmp;
		tmp.point = Point(x, y);
		tmp.data = node;
		tmp.used = true;
		simpleFindMatching(x,y,&tmp,gridNodes);
	}
	if ( !gridNodes.empty()) {
		for ( std::size_t i = 0; i < gridNodes.size(); ++i ) {
			GridNode* node = gridNodes[i];
			entries.push_back(node->point);
		}
	}
}

// ------------------------------------------------
// internal findMatching
// ------------------------------------------------
template<class T>
inline void Grid<T>::simpleFindMatching(int x, int y, GridNode* providedNode, Array<GridNode*>& gridNodes) {
	if ( !isFree(x-1,y)) {
		findMatching(x-1,y,providedNode,gridNodes);
	}
	if ( !isFree(x+1,y)) {
		findMatching(x+1,y,providedNode,gridNodes);
	}
	if ( !isFree(x,y-1)) {
		findMatching(x,y-1,providedNode,gridNodes);
	}
	if ( !isFree(x,y+1)) {
		findMatching(x,y+1,providedNode,gridNodes);
	}
}

// ------------------------------------------------
// internal findMatching
// ------------------------------------------------
template<class T>
inline void Grid<T>::findMatching(int x, int y, GridNode* providedNode, Array<GridNode*>& gridNodes) {
    int idx = getIndex(x,y);
    if ( idx != -1 ) {
        GridNode* currentNode = &m_Data[idx];        
        if ( currentNode->used && isMatch(currentNode->data,providedNode->data)) {
            // check if already here
            bool found = false;
            for ( size_t i = 0; i < gridNodes.size(); ++i ) {
                GridNode* savedNode = gridNodes[i];
                if ( savedNode->point == currentNode->point ) {
                    found = true;
                }
            }
            if ( !found ) {
                gridNodes.push_back(currentNode);
                if ( !isFree(x-1,y)) {
                    findMatching(x-1,y,currentNode,gridNodes);
                }
                if ( !isFree(x+1,y)) {
                    findMatching(x+1,y,currentNode,gridNodes);
                }
                if ( !isFree(x,y-1)) {
                    findMatching(x,y-1,currentNode,gridNodes);
                }
                if ( !isFree(x,y+1)) {
                    findMatching(x,y+1,currentNode,gridNodes);
                }
            }
        }
    }
}

// ------------------------------------------------
//
// ------------------------------------------------
template<class T>
inline void Grid<T>::copyRow(int oldRow, int newRow) {
    for ( int x = 0; x < m_Width; ++x ) {
        int oldIndex = getIndex(x,oldRow);
        int newIndex = getIndex(x,newRow);
        m_Data[newIndex] = m_Data[oldIndex];
    }
}

// ------------------------------------------------
//
// ------------------------------------------------
template<class T>
inline void Grid<T>::copyColumn(int oldColumn, int newColumn) {
    for ( int y = 0; y < m_Height; ++y ) {
        int oldIndex = getIndex(oldColumn,y);
		int newIndex = getIndex(newColumn,y);
		if ( m_Data[oldIndex].used ) {			
			m_Data[newIndex].data = m_Data[oldIndex].data;
			m_Data[newIndex].used = true;
		}
		else {
			m_Data[newIndex].used = false;
		}
    }
}

// ------------------------------------------------
//
// ------------------------------------------------
template<class T>
inline void Grid<T>::fillRow(int row,const T& t) {
    if ( row >= 0 && row < m_Height ) {
        for ( int x = 0; x < m_Width; ++x ) {
            int idx = getIndex(x,row);
            if ( idx != -1 ) {
                m_Data[idx].data = t;
                m_Data[idx].used = true;
            }
        }
    }
}

// ------------------------------------------------
// shift columns
// ------------------------------------------------
template<class T>
inline void Grid<T>::shiftColumns(int startColumn) {    
    if ( startColumn >= 0 && startColumn < m_Width ) {
        int sx = startColumn -1;
        if ( sx < 0 ) {
            sx = 0;
        }
        for ( int x = sx; x < m_Width -1 ; ++x ) {
            copyColumn(x+1,x);
        }
        clearColumn(m_Width-1);        
    }
}
// ------------------------------------------------
// clear column
// ------------------------------------------------
template<class T>
inline void Grid<T>::clearColumn(int column) {
    if ( column >= 0 && column < m_Width ) {
        for ( int y = 0; y < m_Height; ++y ) {
            int idx = getIndex(column,y);
            m_Data[idx].used = false;
        }
    }
}
// ------------------------------------------------
//
// ------------------------------------------------
template<class T>
inline void Grid<T>::fillColumn(int column, const T& t) {
    for ( int y = 0; y < m_Height; ++y ) {
        int idx = getIndex(column,y);
        m_Data[idx].data = t;
        m_Data[idx].used = true;
    }
}

// ------------------------------------------------
// Drop row
// ------------------------------------------------
template<class T>
inline void Grid<T>::dropRow(int x) {
    for ( int y = (m_Height-1); y >= 0; --y ) {
        dropCell(x,y);
    }
}

// ------------------------------------------------
// Drop cell
// ------------------------------------------------
template<class T>
inline void Grid<T>::dropCell(int x, int y) {
    int idx = getIndex(x,y);
    if ( !isFree(x,y)) {
        int finalY = 0;
        for ( int yp = y +1; yp < m_Height; ++yp ) {
            if ( isFree(x,yp) ) {
                //finalY = yp;
				++finalY;
            }
            else {
                break;
            }
        }
        if ( finalY != 0 ) {
            int nidx = getIndex(x,finalY);
            m_Data[nidx].data = m_Data[idx].data;
            m_Data[nidx].used = true;
            m_Data[idx].used = false;
        }
    }
}

// -------------------------------------------------------
// Drop cells - remove empty cells in between
// -------------------------------------------------------
template<class T>
inline void Grid<T>::dropCells(Array<DroppedCell<T>>& droppedCells) {
	for ( int x = 0; x < m_Width; ++x ) {
		for ( int y = 0 ; y < m_Height - 1; ++y ) {
			if ( isFree(x,y) ) {
				int sy = y + 1;
				while ( isFree(x,sy) && sy < m_Height - 1 ) {
					++sy;
				}			
				if ( !isFree(x,sy)) {
					DroppedCell<T> dc;
					dc.data = get(x, sy);
					dc.from = Point(x,sy);
					dc.to = Point(x,y);
					droppedCells.push_back(dc);
					set(x,y,get(x,sy));
					remove(x,sy);							
				}
			}
		}
	}
}

// -------------------------------------------------------
// Is column empty
// -------------------------------------------------------
template<class T>
bool Grid<T>::isColumnEmpty(int col) {
	int count = 0;
	for (int i = 0; i < m_Height; ++i ) {
		if ( !isFree(col,i)) {
			++count;
		}
	}
	return count == 0;
}

// ------------------------------------------------
// Remove grid points
// ------------------------------------------------
template<class T>
inline void Grid<T>::remove(const Array<Point>& points,bool shift) {
    for ( std::size_t i = 0; i < points.size(); ++i ) {
        Point gp = points[i];
		if (!remove(gp.x, gp.y)) {
			LOG << "cannot remove cell at " << gp.x << " " << gp.y;
		}
    }
	if (shift) {
		int moved = 0;
		for (int i = 0; i < m_Width; ++i) {
			if (isColumnEmpty(i)) {
				shiftColumns(i + 1);
			}
		}
	}
}

template<class T>
inline void Grid<T>::swap(const Point& first, const Point& second) {
	int fi = getIndex(first);
	int si = getIndex(second);
	GridNode n = m_Data[fi];
	m_Data[fi] = m_Data[si];
	m_Data[si] = n;
}

}

