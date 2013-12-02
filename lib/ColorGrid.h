/* 
 * File:   ColorGrid.h
 * Author: meckya
 *
 * Created on 28. März 2013, 10:40
 */

#ifndef COLORGRID_H
#define	COLORGRID_H
#include "Grid.h"

struct Block {
    
    int color;
    
};

class ColorGrid : public Grid<Block> {
    
public:
    ColorGrid(int sizeX,int sizeY) : Grid<Block>(sizeX,sizeY) {}
    virtual ~ColorGrid() {}
    bool isMatch(const Block& first,const Block& right);
    void debug();
private:

};

#endif	/* COLORGRID_H */

