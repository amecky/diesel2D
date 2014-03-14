#pragma once
#include "..\dxstdafx.h"
#include "..\math\math_types.h"
#include <string>
#include "..\lib\container\String.h"
#include "..\utils\Color.h"

namespace ds {

class PAKReader {

public:
	PAKReader(void);
	~PAKReader(void);
	bool open(const char* fileName);
	void close();
	int readInt() const;
	uint32 readUInt() const;
	bool readBool() const;
	uint32 readString(std::string& str) const;
	uint32 readString(String& str) const;
	char readChar() const;
	float readFloat() const;
	Vector2f readVector2f() const;
	Vector3f readVector3f() const;
	ulong readULong() const;
	Color readColor() const;
	uchar readUChar() const;
	Rect readRect() const;
	bool isEOF() const {
		return feof(m_File) != 0;
	}
	bool verifyHeader();
	const std::string& getFileName() const {
		return m_FileName;
	}
	FILE* getFile() {
		return m_File;
	}
private:
	FILE* m_File;
	std::string m_FileName;
};

}

