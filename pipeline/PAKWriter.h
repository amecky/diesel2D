#pragma once
#include "..\dxstdafx.h"
#include "..\math\math_types.h"
#include <string>
#include "..\lib\container\String.h"
#include "..\utils\Color.h"

namespace ds {

class PAKWriter {

public:
	PAKWriter(void);
	~PAKWriter(void);
	bool open(const char* fileName);
	void close();
	void writeInt(int value);
	void writeUInt(uint32 value);
	void writeBool(bool value);
	void writeString(const std::string& str);
	void writeString(const String& str);
	void writeString(const char* str);
	void writeChar(char c);
	void writeUChar(uchar c);
	void writeULong(ulong v);
	void writeFloat(float value);
	void writeVec2(const Vec2& v);
	void writeVec3(const Vec3& v);
	void writeColor(const Color& color);
	void writeRect(const Rect& rect);
	void writeHeader(uint32 resourceType);
	FILE* getFile() {
		return m_File;
	}
private:
	FILE* m_File;
};

}

