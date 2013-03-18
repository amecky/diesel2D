#include "PAKWriter.h"
#include <iostream>

namespace ds {

PAKWriter::PAKWriter(void) {
}


PAKWriter::~PAKWriter(void) {
	close();
}

// ----------------------------------------------
//
// ----------------------------------------------
bool PAKWriter::open(const char* fileName) {
	m_File = fopen(fileName,"wb");	
	return m_File != 0;
}

void PAKWriter::writeHeader(uint32 resourceType) {
	writeChar('d');
	writeChar('s');
	writeChar('b');
	writeUInt(resourceType);
}
// ----------------------------------------------
//
// ----------------------------------------------
void PAKWriter::close() {
	if ( m_File != 0 ) {
		fclose(m_File);
		m_File = 0;
	}
}

// ----------------------------------------------
//
// ----------------------------------------------
void PAKWriter::writeInt(int value) {
	fwrite(&value,sizeof(int),1,m_File);
}

// ----------------------------------------------
//
// ----------------------------------------------
void PAKWriter::writeUInt(uint32 value) {
	fwrite(&value,sizeof(uint32),1,m_File);
}

// ----------------------------------------------
//
// ----------------------------------------------
void PAKWriter::writeULong(ulong value) {
	fwrite(&value,sizeof(ulong),1,m_File);
}

// ----------------------------------------------
//
// ----------------------------------------------
void PAKWriter::writeBool(bool value) {
	if ( value ) {
		writeInt(1);
	}
	else {
		writeInt(0);
	}
}

// ----------------------------------------------
//
// ----------------------------------------------
void PAKWriter::writeChar(char c) {
	fputc((char)c,m_File);
}

// ----------------------------------------------
//
// ----------------------------------------------
void PAKWriter::writeUChar(uchar c) {
	fputc((uchar)c,m_File);
}
// ----------------------------------------------
//
// ----------------------------------------------
void PAKWriter::writeString(const std::string& str) {
	uint32 length = str.length();
	writeUInt(length);
	for ( int i = 0; i <length; ++i ) {
		writeChar(str[i]);
	}
}

// ----------------------------------------------
//
// ----------------------------------------------
void PAKWriter::writeString(const String& str) {
	uint32 length = str.length();
	writeUInt(length);
	char c;
	for ( int i = 0; i <length; ++i ) {
		str.at(c,i);
		writeChar(c);
	}
}

// ----------------------------------------------
//
// ----------------------------------------------
void PAKWriter::writeString(const char* str) {
	uint32 length = strlen(str);
	writeUInt(length);
	char c;
	for ( int i = 0; i <length; ++i ) {
		writeChar(*str);
		++str;
	}
}

// ----------------------------------------------
//
// ----------------------------------------------
void PAKWriter::writeFloat(float value) {
	fwrite(&value,sizeof(float),1,m_File);
}

// ----------------------------------------------
//
// ----------------------------------------------
void PAKWriter::writeVec2(const Vec2& v) {
	writeFloat(v.x);
	writeFloat(v.y);
}

// ----------------------------------------------
//
// ----------------------------------------------
void PAKWriter::writeVec3(const Vec3& v) {
	writeFloat(v.x);
	writeFloat(v.y);
	writeFloat(v.z);
}

// ----------------------------------------------
//
// ----------------------------------------------
void PAKWriter::writeColor(const Color& color) {
	writeFloat(color.r);
	writeFloat(color.g);
	writeFloat(color.b);
	writeFloat(color.a);
}
// ----------------------------------------------
//
// ----------------------------------------------
void PAKWriter::writeRect(const Rect& rect) {
	writeInt(rect.top);
	writeInt(rect.left);
	writeInt(rect.width());
	writeInt(rect.height());
}

}
