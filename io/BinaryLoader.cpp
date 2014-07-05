/* 
 * File:   BinaryLoader.cpp
 * Author: meckya
 * 
 * Created on 9. Januar 2014, 09:51
 */

#include "BinaryLoader.h"

BinaryLoader::BinaryLoader() : m_Stream(0) {
    m_CurrentHeader = new ChunkHeader();
}

BinaryLoader::BinaryLoader(const BinaryLoader& orig) {	
}

BinaryLoader::~BinaryLoader() {
	if ( m_CurrentHeader != 0 ) {
		delete m_CurrentHeader;
	}
    close();
}

// ------------------------------------------------------
// Open file
// ------------------------------------------------------
uint32 BinaryLoader::open(const char* name, int* signature, uint32 signatureLength) {
    if (m_Stream != 0) {
        m_ErrorCode = IO_ALREADY_OPEN;
        return m_ErrorCode;
    }
    // open stream
    m_Stream = fopen(name, "rb");
    if (!m_Stream) {
        m_ErrorCode = IO_ERROR;
        return m_ErrorCode;
    }
    
    int* tmp = new int[signatureLength];
    m_CurrentPos = signatureLength * sizeof(int);
    if (fread(tmp, signatureLength * sizeof(int), 1, m_Stream) != 1) {
        m_ErrorCode = IO_WRITE_ERROR;
        return m_ErrorCode;
    }
    for ( int i = 0; i < signatureLength; ++i ) {
        if ( tmp[i] != signature[i] ) {
            delete[] tmp;
            close();
            return IO_INVALID_FILE;
        }
    }
    m_StartPos = ftell(m_Stream);
    delete[] tmp;
    return IO_OK;
}

// ------------------------------------------------------
// Open chunk
// ------------------------------------------------------
uint32 BinaryLoader::openChunk() {
    if ( m_Stream == 0 ) {
        m_ErrorCode = IO_NO_STREAM;
        return m_ErrorCode;
    }
    if ( fread(m_CurrentHeader,sizeof(ChunkHeader),1,m_Stream) != 1 ) {
        m_ErrorCode = IO_ERROR;
        return m_ErrorCode;
    }
    //printf("current header id: %d version %d size %d\n",m_CurrentHeader->id,m_CurrentHeader->version,m_CurrentHeader->size);
    return IO_OK;
}

void BinaryLoader::read(float* value) {
    readBuffer(value,sizeof(float));    
}

void BinaryLoader::read(Vector2f* value) {
	readBuffer(&value->x,sizeof(float));    
	readBuffer(&value->y,sizeof(float));    
}

void BinaryLoader::read(ds::Color* value) {
	readBuffer(&value->r,sizeof(float));    
	readBuffer(&value->g,sizeof(float));    
	readBuffer(&value->b,sizeof(float));    
	readBuffer(&value->a,sizeof(float));    
}

void BinaryLoader::read(ds::ColorPath* value) {
	int count = 0;
	readBuffer(&count,sizeof(int));
	for ( int i = 0; i < count; ++i ) {
		float ts = 0.0f;
		readBuffer(&ts,sizeof(float));
		ds::Color c;
		readBuffer(&c.r,sizeof(float));    
		readBuffer(&c.g,sizeof(float));    
		readBuffer(&c.b,sizeof(float));    
		readBuffer(&c.a,sizeof(float));    
		value->add(ts,c);
	}
}

void BinaryLoader::read(ds::Vector2fPath* value) {
	int count = 0;
	readBuffer(&count,sizeof(int));
	for ( int i = 0; i < count; ++i ) {
		float ts = 0.0f;
		readBuffer(&ts,sizeof(float));
		Vector2f c;
		readBuffer(&c.x,sizeof(float));    
		readBuffer(&c.y,sizeof(float));    
		value->add(ts,c);
	}
}

void BinaryLoader::read(ds::Rect* value) {
	readBuffer(&value->top,sizeof(float));    
	readBuffer(&value->left,sizeof(float));    
	readBuffer(&value->bottom,sizeof(float));    
	readBuffer(&value->right,sizeof(float));    
}

void BinaryLoader::read(int* value) {
    readBuffer(value,sizeof(int));    
}

void BinaryLoader::read(uint32* value) {
	readBuffer(value,sizeof(uint32));    
}

void BinaryLoader::read(std::string& str) {
	int length = 0;
	read(&length);
	char c;
	str.clear();
	for ( int i = 0; i < length; ++i ) {
		readBuffer(&c,sizeof(char));    
		char f = c + 32;
		str += f;
	}
}


// ------------------------------------------------------
// Close chunk
// ------------------------------------------------------
void BinaryLoader::closeChunk() {
    int pos = m_CurrentPos + m_CurrentHeader->size;
    fseek(m_Stream,pos,SEEK_SET);
    m_CurrentPos = pos;
}

// ------------------------------------------------------
// Close 
// ------------------------------------------------------
uint32 BinaryLoader::close() {
    if ( m_Stream != 0 ) {
        fclose(m_Stream);
    }
    m_Stream = 0;
    return IO_OK;
}

// ------------------------------------------------------
// Reset 
// ------------------------------------------------------
void BinaryLoader::reset() {
   fseek(m_Stream,m_StartPos,SEEK_SET);
   m_CurrentPos = m_StartPos;
}

// ------------------------------------------------------
// Read 
// ------------------------------------------------------
uint32 BinaryLoader::readBuffer(void* buffer, uint32 size) {
    if ( m_Stream == 0 ) {
        m_ErrorCode = IO_NO_STREAM;
        return m_ErrorCode;
    }
    int realSize = fread(buffer,size,1,m_Stream);
    if ( realSize != size ) {
        m_ErrorCode = IO_INVALID_DATA;
        return m_ErrorCode;
    }
    return IO_OK;
}
