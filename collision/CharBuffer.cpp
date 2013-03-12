#include "CharBuffer.h"

namespace ds {

CharBuffer::CharBuffer(uint32 size) : m_Size(size) , m_Total(0) , m_Index(0) {
    m_Buffer = new char[size];
}

CharBuffer::~CharBuffer() {
	delete m_Buffer;
}

void CharBuffer::write(const void* p, uint32 size) {
    char *ptr = (char *) p;
    uint32 to_write = size;
    char* buffer = (m_Buffer + m_Index);
    m_Index += to_write;
    while (to_write > 0) {
        *buffer = *ptr;
        to_write--;
        ++ptr;
        ++buffer;
    }
    ++m_Total;
}

uint32 CharBuffer::read(void *p, size_t size, uint32 index) {
    char *ptr = (char *) p;
    int to_read;
    to_read = size;
    char* buffer = (m_Buffer + index * size);
    while (to_read > 0) {
        *ptr = *buffer;
        to_read--;
        ++ptr;
        ++buffer;
    }
    return (to_read / size);
}

}

