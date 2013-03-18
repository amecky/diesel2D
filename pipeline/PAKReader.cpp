#include "PAKReader.h"
#include "..\utils\Log.h"

namespace ds {

PAKReader::PAKReader() {
}


PAKReader::~PAKReader() {
	close();
}

bool PAKReader::open(const char* fileName) {
	m_FileName = std::string(fileName);
	m_File = fopen(fileName,"rb");
	return m_File != 0;
}

void PAKReader::close() {
	if ( m_File != 0 ) {
		fclose(m_File);
		m_File = 0;
	}
}

bool PAKReader::verifyHeader() {
	// check that it starts 
	char d = readChar();
	char s = readChar();
	char b = readChar();
	return d == 'd' && s == 's' && b == 'b';
}

int PAKReader::readInt() const {
	int v;
	size_t cnt = fread(&v,sizeof(int),1,m_File);
	return v;
}

uint32 PAKReader::readUInt() const {
	uint32 v;
	size_t cnt = fread(&v,sizeof(uint32),1,m_File);
	return v;
}

ulong PAKReader::readULong() const {
	ulong v;
	size_t cnt = fread(&v,sizeof(ulong),1,m_File);
	return v;
}

bool PAKReader::readBool() const {
	int c = readInt();
	return c == 0 ? false : true;
}

uint32 PAKReader::readString(std::string& str) const {
	str = "";
	uint32 length = readUInt();	
	for ( uint32 i = 0; i < length; ++i ) {
		str += readChar();
	}
	return length;
}

uint32 PAKReader::readString(String& str) const {
	uint32 length = readUInt();	
	for ( uint32 i = 0; i < length; ++i ) {
		str.append(readChar());
	}
	return length;
}

char PAKReader::readChar() const {
	char c;
	size_t cnt = fread(&c,sizeof(char),1,m_File);
	return c;
}

uchar PAKReader::readUChar() const {
	uchar c;
	size_t cnt = fread(&c,sizeof(uchar),1,m_File);
	return c;
}

float PAKReader::readFloat() const {
	float v;
	size_t cnt = fread(&v,sizeof(float),1,m_File);
	return v;
}

Vec2 PAKReader::readVec2() const {    
	Vec2 vec;
	vec.x = readFloat();
	vec.y = readFloat();
	return vec;
}

Vec3 PAKReader::readVec3() const {
	Vec3 vec;
	vec.x = readFloat();
	vec.y = readFloat();
	vec.z = readFloat();
	return vec;
}

Color PAKReader::readColor() const {
	Color color;
	color.r = readFloat();
	color.g = readFloat();
	color.b = readFloat();
	color.a = readFloat();
	return color;
}

Rect PAKReader::readRect() const {
	int top = readInt();
	int left = readInt();
	int width = readInt();
	int height = readInt();
	Rect r = Rect(top,left,width,height);
	return r;
}


}
