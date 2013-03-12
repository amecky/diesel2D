#ifndef CHARBUFFER_H
#define	CHARBUFFER_H
#include <stdio.h>
#include "..\dxstdafx.h"

namespace ds {

class CharBuffer {
    
public:
    CharBuffer(uint32 size);    
    virtual ~CharBuffer();
    void reset() {
        m_Index = 0;
        m_Total = 0;
    }
    uint32 size() const {
        return m_Total;
    }
    void write(const void* p, uint32 size);
    uint32 read(void* p, size_t size,uint32 index);
private:
    CharBuffer(const CharBuffer& orig) {}
    char* m_Buffer;
    uint32 m_Index;
    uint32 m_Total;
    uint32 m_Size;
};

}
#endif	/* CHARBUFFER_H */

