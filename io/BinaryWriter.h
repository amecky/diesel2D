/* 
 * File:   Serializer.h
 * Author: meckya
 *
 * Created on 9. Januar 2014, 08:59
 */

#ifndef SERIALIZER_H
#define	SERIALIZER_H
#include <stdio.h>
#include <Vector.h>
#include "..\utils\Color.h"
#include "..\math\math_types.h"
#include <string>
#include "..\math\FloatArray.h"

typedef unsigned int uint32;

enum IOCode {
    IO_OK = 0,
    IO_ALREADY_OPEN,
    IO_WRITE_ERROR,
    IO_NO_STREAM,
    IO_INVALID_FILE,
    IO_ERROR,
    IO_INVALID_DATA
};

struct ChunkHeader {
    uint32 id;
    uint32 version;
    uint32 size;    
};

class BinaryWriter {
    
public:
    BinaryWriter();
    BinaryWriter(const BinaryWriter& orig);
    virtual ~BinaryWriter();
    uint32 open(const char* name,int* signature,uint32 signatureLength);
    void close();
    uint32 startChunk(uint32 id,uint32 version);
    void closeChunk();
    uint32 write(int value);
	uint32 write(uint32 value);
	uint32 write(float value);
	uint32 write(bool value);
	uint32 write(const Vector2f& v);
	uint32 write(const Vector3f& v);
	uint32 write(const Vector4f& v);
	uint32 write(const ds::Color& color);
	uint32 write(const ds::Rect& r);
	uint32 write(const std::string& str);
	uint32 write(const ds::ColorPath& path);
	uint32 write(const ds::Vector2fPath& path);
	uint32 write(const ds::FloatArray& path);
    const uint32 getStatus() const {
        return m_ErrorCode;
    }
private:
    FILE* m_Stream;
    uint32 m_ErrorCode;
    uint32 m_Index;
    uint32 m_StartPos;
};

#endif	/* SERIALIZER_H */

