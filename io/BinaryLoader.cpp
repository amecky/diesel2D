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
    read(value,sizeof(float));    
}

void BinaryLoader::read(int* value) {
    read(value,sizeof(int));    
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
uint32 BinaryLoader::read(void* buffer, uint32 size) {
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
