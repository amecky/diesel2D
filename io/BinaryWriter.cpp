/* 
 * File:   Serializer.cpp
 * Author: meckya
 * 
 * Created on 9. Januar 2014, 08:59
 */

#include "BinaryWriter.h"

BinaryWriter::BinaryWriter() : m_Stream(0) {
}

BinaryWriter::BinaryWriter(const BinaryWriter& orig) {
}

BinaryWriter::~BinaryWriter() {
    close();
}

uint32 BinaryWriter::open(const char* name, int* signature, uint32 signatureLength) {
    if (m_Stream != 0) {
        m_ErrorCode = IO_ALREADY_OPEN;
        return m_ErrorCode;
    }

    // open stream
    m_Stream = fopen(name, "wb");
    if (!m_Stream) {
        m_ErrorCode = IO_ERROR;
        return m_ErrorCode;
    }

    m_Index = 0;

    // write file header
    if (fwrite(signature, signatureLength * sizeof(int), 1, m_Stream) != 1) {
        m_ErrorCode = IO_WRITE_ERROR;
        return m_ErrorCode;
    }
    return IO_OK;
}

uint32 BinaryWriter::startChunk(uint32 id, uint32 version) {
    if (!m_Stream) {
        m_ErrorCode = IO_NO_STREAM;
        return m_ErrorCode;
    }
    ChunkHeader header;
    header.id = id;
    header.version = version;
    header.size = 0;
    m_StartPos = ftell(m_Stream);
    //printf("start pos %d\n", m_StartPos);
    if (fwrite(&header, sizeof (ChunkHeader), 1, m_Stream) != 1) {
        m_ErrorCode = IO_WRITE_ERROR;
        return m_ErrorCode;
    }

    return IO_OK;
}

void BinaryWriter::closeChunk() {
    if (m_Stream) {
        int pos = ftell(m_Stream);
        //printf("new pos %d\n",pos);
        int length = pos - m_StartPos;
        //printf("length %d\n",length);
        fseek(m_Stream, m_StartPos + 8, SEEK_SET);

        if (fwrite(&length, sizeof ( int), 1, m_Stream) != 1) {
            m_ErrorCode = IO_WRITE_ERROR;
            //return m_ui32Error;
        }

        fseek(m_Stream, pos, SEEK_SET);
    }
}

uint32 BinaryWriter::write(int value) {
    if (fwrite(&value, sizeof ( int), 1, m_Stream) != 1) {
            m_ErrorCode = IO_WRITE_ERROR;
            return m_ErrorCode;
        }
    return IO_OK;
}

void BinaryWriter::close() {
    if (m_Stream) {
        fclose(m_Stream);
    }
    m_Stream = 0;
}