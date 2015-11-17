#include "JSONWriter.h"


JSONWriter::JSONWriter() {
	f = 0;
	_ident = 0;
}


JSONWriter::~JSONWriter() {
	if (f != 0) {
		fclose(f);
	}
}

// ----------------------------------------------------------
// open
// ----------------------------------------------------------
bool JSONWriter::open(const char* fileName) {
	f = fopen(fileName,"w");
	return true;
}

// ----------------------------------------------------------
// start category
// ----------------------------------------------------------
void JSONWriter::startCategory(const char* name) {
	writeIdent();
	fprintf(f, "\"%s\" { ", name);
	_open = true;
	_items = 0;
	_ident += 4;
}

// ----------------------------------------------------------
// write int
// ----------------------------------------------------------
void JSONWriter::write(const char* name, int value) {
	
	writeLineIdent();
	fprintf_s(f, "\"%s\" : \"%d\"", name, value);
	++_items;
}

// ----------------------------------------------------------
// write float
// ----------------------------------------------------------
void JSONWriter::write(const char* name, float value) {
	
	writeLineIdent();
	fprintf(f, "\"%s\" : \"%.2f\"", name, value);
	++_items;
}

// ----------------------------------------------------------
// write string
// ----------------------------------------------------------
void JSONWriter::write(const char* name, const char* value) {

	writeLineIdent();
	fprintf(f, "\"%s\" : \"%s\"", name, value);
	++_items;
}

// ----------------------------------------------------------
// write boolean
// ----------------------------------------------------------
void JSONWriter::write(const char* name, bool value) {

	writeLineIdent();
	if (value) {
		fprintf(f, "\"%s\" : \"true\"", name, value);
	}
	else {
		fprintf(f, "\"%s\" : \"false\"", name);
	}
	++_items;
}

// ----------------------------------------------------------
// write v2
// ----------------------------------------------------------
void JSONWriter::write(const char* name, const v2& value) {
	
	writeLineIdent();
	int x = value.x;
	int y = value.y;
	fprintf(f, "\"%s\" : \"%d,%d\"", name, x,y);
	++_items;

}

// ----------------------------------------------------------
// write color
// ----------------------------------------------------------
void JSONWriter::write(const char* name, const ds::Color& value) {
	
	writeLineIdent();
	int r = value.r * 255.0f;
	int g = value.g * 255.0f;
	int b = value.b * 255.0f;
	int a = value.a * 255.0f;
	fprintf(f, "\"%s\" : \"%d,%d,%d,%d\"", name, r, g, b, a);
	++_items;
}

// ----------------------------------------------------------
// write rect
// ----------------------------------------------------------
void JSONWriter::write(const char* name, const ds::Rect& value) {
	
	writeLineIdent();
	int top = value.top;
	int left = value.left;
	int w = value.width();
	int h = value.height();
	fprintf(f, "\"%s\" : \"%d,%d,%d,%d\"", name, top, left, w, h);
	++_items;
}

// ----------------------------------------------------------
// end category
// ----------------------------------------------------------
void JSONWriter::endCategory() {
	writeIdent();
	fprintf_s(f, "\n}\n");
	_open = false;
	_ident -= 4;
}

// ----------------------------------------------------------
// write line ident
// ----------------------------------------------------------
void JSONWriter::writeLineIdent() {
	if (_items != 0) {
		fprintf(f, " , \n");
	}
	else {
		fprintf(f, "\n");
	}
	for (int i = 0; i < _ident; ++i) {
		fprintf(f, " ");
	}
}

// ----------------------------------------------------------
// write ident
// ----------------------------------------------------------
void JSONWriter::writeIdent() {
	for (int i = 0; i < _ident; ++i) {
		fprintf(f, " ");
	}
}