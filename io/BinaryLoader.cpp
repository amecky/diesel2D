/* 
 * File:   BinaryLoader.cpp
 * Author: meckya
 * 
 * Created on 9. Januar 2014, 09:51
 */

#include "BinaryLoader.h"
#include "..\utils\Log.h"

BinaryLoader::BinaryLoader() : _data(0) {
    m_CurrentHeader = new ChunkHeader();
}

BinaryLoader::BinaryLoader(const BinaryLoader& orig) {	
}

BinaryLoader::~BinaryLoader() {
	if ( m_CurrentHeader != 0 ) {
		delete m_CurrentHeader;
	}
	if (_data != 0) {
		delete[] _data;
	}
}

// ------------------------------------------------------
// Open file
// ------------------------------------------------------
uint32 BinaryLoader::open(const char* name, int* signature, uint32 signatureLength) {
	_index = 0;
	HANDLE hFile = CreateFile(name, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE) {
		_size = GetFileSize(hFile, 0);
		_data = new char[_size];
		DWORD read;
		if (!ReadFile(hFile, _data, _size, &read, 0)) {
			delete[] _data;
			_data = 0;
		}
		CloseHandle(hFile);
		
	}
	else {
        m_ErrorCode = IO_ALREADY_OPEN;
        return m_ErrorCode;
    }
    int* tmp = new int[signatureLength];
	int current = 0;
	for (int i = 0; i < signatureLength; ++i) {
		read(&current);
		if (signature[i] != current) {
			return IO_INVALID_FILE;
		}
	}
	m_CurrentPos = _index;
	m_StartPos = _index;
    delete[] tmp;
    return IO_OK;
}

// ------------------------------------------------------
// Open chunk
// ------------------------------------------------------
uint32 BinaryLoader::openChunk() {
    if ( _data == 0 ) {
        m_ErrorCode = IO_NO_STREAM;
        return m_ErrorCode;
    }
	m_CurrentPos = _index;
	if (readBuffer(m_CurrentHeader, sizeof(ChunkHeader)) == IO_OK) {
		return IO_OK;
	}
	else {
		m_ErrorCode = IO_ERROR;
		return m_ErrorCode;
	}
}

void BinaryLoader::read(float* value) {
    readBuffer(value,sizeof(float));    
}

void BinaryLoader::read(Vector2f* value) {
	readBuffer(&value->x,sizeof(float));    
	readBuffer(&value->y,sizeof(float));    
}

void BinaryLoader::read(Vector3f* value) {
	readBuffer(&value->x, sizeof(float));
	readBuffer(&value->y, sizeof(float));
	readBuffer(&value->z, sizeof(float));
}

void BinaryLoader::read(Vector4f* value) {
	readBuffer(&value->x, sizeof(float));
	readBuffer(&value->y, sizeof(float));
	readBuffer(&value->z, sizeof(float));
	readBuffer(&value->w, sizeof(float));
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

void BinaryLoader::read(ds::FloatArray* value) {
	int count = 0;
	readBuffer(&count, sizeof(int));
	for (int i = 0; i < count; ++i) {
		float ts = 0.0f;
		readBuffer(&ts, sizeof(float));
		float c;
		readBuffer(&c, sizeof(float));
		value->add(ts, c);
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

void BinaryLoader::read(bool* value) {
	readBuffer(value, sizeof(bool));
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

void BinaryLoader::read(char* str) {
	int length = 0;
	read(&length);
	char c;
	for (int i = 0; i < length; ++i) {
		readBuffer(&c, sizeof(char));
		char f = c + 32;
		str[i] = f;
	}
	str[length] = '\0';
}

// ------------------------------------------------------
// Close chunk
// ------------------------------------------------------
void BinaryLoader::closeChunk() {
    int pos = m_CurrentPos + m_CurrentHeader->size;
	_index = pos;
}

// ------------------------------------------------------
// Reset 
// ------------------------------------------------------
void BinaryLoader::reset() {
   _index = m_StartPos;
}

// ------------------------------------------------------
// Read 
// ------------------------------------------------------
uint32 BinaryLoader::readBuffer(void* buffer, uint32 size) {
	if (_data == 0) {
		m_ErrorCode = IO_NO_STREAM;
		return m_ErrorCode;
	}
	if (_index + size <= _size) {
		memcpy(buffer, _data + _index, size);
		_index += size;
	}
	else {
        m_ErrorCode = IO_NO_STREAM;
        return m_ErrorCode;
    }
    return IO_OK;
}
