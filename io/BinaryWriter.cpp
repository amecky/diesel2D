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
        int length = pos - m_StartPos;
        fseek(m_Stream, m_StartPos + 8, SEEK_SET);
        if (fwrite(&length, sizeof ( int), 1, m_Stream) != 1) {
            m_ErrorCode = IO_WRITE_ERROR;
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

uint32 BinaryWriter::write(uint32 value) {
	if (fwrite(&value, sizeof ( uint32), 1, m_Stream) != 1) {
		m_ErrorCode = IO_WRITE_ERROR;
		return m_ErrorCode;
	}
	return IO_OK;
}

uint32 BinaryWriter::write(float value) {
	if (fwrite(&value, sizeof ( float), 1, m_Stream) != 1) {
		m_ErrorCode = IO_WRITE_ERROR;
		return m_ErrorCode;
	}
	return IO_OK;
}

uint32 BinaryWriter::write(bool value) {
	if (fwrite(&value, sizeof(bool), 1, m_Stream) != 1) {
		m_ErrorCode = IO_WRITE_ERROR;
		return m_ErrorCode;
	}
	return IO_OK;
}

uint32 BinaryWriter::write(const Vector2f& v) {
	fwrite(&v.x, sizeof (float), 1, m_Stream);
	fwrite(&v.y, sizeof (float), 1, m_Stream);
	return IO_OK;
}

uint32 BinaryWriter::write(const Vector3f& v) {
	fwrite(&v.x, sizeof (float), 1, m_Stream);
	fwrite(&v.y, sizeof (float), 1, m_Stream);
	fwrite(&v.z, sizeof (float), 1, m_Stream);
	return IO_OK;
}

uint32 BinaryWriter::write(const Vector4f& v) {
	fwrite(&v.x, sizeof (float), 1, m_Stream);
	fwrite(&v.y, sizeof (float), 1, m_Stream);
	fwrite(&v.z, sizeof (float), 1, m_Stream);
	fwrite(&v.w, sizeof (float), 1, m_Stream);
	return IO_OK;
}

uint32 BinaryWriter::write(const std::string& v) {
	int length = v.length();
	write(length);
	for ( int i = 0; i < length; ++i ) {
		char c = v[i] - 32;
		fwrite(&c, sizeof (char), 1, m_Stream);
	}	
	return IO_OK;
}

uint32 BinaryWriter::write(const char* v) {
	int length = strlen(v);
	write(length);
	for (int i = 0; i < length; ++i) {
		char c = v[i] - 32;
		fwrite(&c, sizeof(char), 1, m_Stream);
	}
	return IO_OK;
}

uint32 BinaryWriter::write(const ds::Color& v) {
	ds::Color tmp = v;
	if (v.r > 1.0f) {
		tmp.r /= 255.0f;
		tmp.g /= 255.0f;
		tmp.b /= 255.0f;
		tmp.a /= 255.0f;

	}
	fwrite(&tmp.r, sizeof (float), 1, m_Stream);
	fwrite(&tmp.g, sizeof (float), 1, m_Stream);
	fwrite(&tmp.b, sizeof (float), 1, m_Stream);
	fwrite(&tmp.a, sizeof (float), 1, m_Stream);
	return IO_OK;
}

uint32 BinaryWriter::write(const ds::ColorPath& v) {
	int size = v.size();
	fwrite(&size,sizeof(int),1,m_Stream);
	for ( int i = 0;i < size; ++i ) {
		float ts = v.getTimeStep(i);
		fwrite(&ts,sizeof(float),1,m_Stream);
		ds::Color c = v.getColor(i);
		if (c.r > 1.0f) {
			c.r /= 255.0f;
			c.g /= 255.0f;
			c.b /= 255.0f;
			c.a /= 255.0f;

		}
		fwrite(&c.r, sizeof (float), 1, m_Stream);
		fwrite(&c.g, sizeof (float), 1, m_Stream);
		fwrite(&c.b, sizeof (float), 1, m_Stream);
		fwrite(&c.a, sizeof (float), 1, m_Stream);
	}
	return IO_OK;
}

uint32 BinaryWriter::write(const ds::Vector2fPath& v) {
	int size = v.size();
	fwrite(&size,sizeof(int),1,m_Stream);
	for ( int i = 0;i < size; ++i ) {
		float ts = v.getTimeStep(i);
		fwrite(&ts,sizeof(float),1,m_Stream);
		Vector2f c = v.getVec2(i);
		fwrite(&c.x, sizeof (float), 1, m_Stream);
		fwrite(&c.y, sizeof (float), 1, m_Stream);
	}
	return IO_OK;
}

uint32 BinaryWriter::write(const ds::FloatArray& v) {
	int size = v.size();
	fwrite(&size, sizeof(int), 1, m_Stream);
	for (int i = 0; i < size; ++i) {
		float ts = v.key(i);
		fwrite(&ts, sizeof(float), 1, m_Stream);
		float c = v.value(i);
		fwrite(&c, sizeof (float), 1, m_Stream);
	}
	return IO_OK;
}

uint32 BinaryWriter::write(const ds::Rect& r) {
	fwrite(&r.top, sizeof (float), 1, m_Stream);
	fwrite(&r.left, sizeof (float), 1, m_Stream);
	fwrite(&r.bottom, sizeof (float), 1, m_Stream);
	fwrite(&r.right, sizeof (float), 1, m_Stream);
	return IO_OK;
}


void BinaryWriter::close() {
    if (m_Stream) {
        fclose(m_Stream);
    }
    m_Stream = 0;
}