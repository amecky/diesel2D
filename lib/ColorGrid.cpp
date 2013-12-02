/* 
 * File:   ColorGrid.cpp
 * Author: meckya
 * 
 * Created on 28. März 2013, 10:40
 */

#include "ColorGrid.h"
#include <stdio.h>

bool ColorGrid::isMatch(const Block& first,const Block& right) {
    if ( first.color == right.color ) {
        return true;
    }
    return false;
}

void ColorGrid::debug() {
    int dx = width();
    int dy = height();
    printf("dimension: %d %d\n",dx,dy);
    for ( int y = 0; y < dy; ++y ) {
        for ( int x = 0; x < dx ; ++x ) {
            if ( !Grid::isFree(x,y)) {
                Block b = get(x,y);
                printf("%2d ",b.color);
            }
            else {
                printf("-1 ");
            }
        }
        printf("\n");
    }     
}