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

bool JSONWriter::open(const char* fileName) {
	f = fopen(fileName,"w");
	return true;
}

void JSONWriter::startCategory(const char* name) {
	writeIdent();
	fprintf(f, "\"%s\" { ", name);
	_open = true;
	_items = 0;
	_ident += 4;
}

void JSONWriter::write(const char* name, int value) {
	
	if (_items != 0) {
		fprintf(f," , \n");
	}
	else {
		fprintf(f, "\n");
	}
	writeIdent();
	fprintf_s(f, "\"%s\" : \"%d\"", name, value);
	++_items;
}

void JSONWriter::write(const char* name, float value) {
	
	if (_items != 0) {
		fprintf(f, " , \n");
	}
	else {
		fprintf(f, "\n");
	}
	writeIdent();
	fprintf(f, "\"%s\" : \"%.2f\"", name, value);
	++_items;
}

void JSONWriter::write(const char* name, const char* value) {

	if (_items != 0) {
		fprintf(f, " , \n");
	}
	else {
		fprintf(f, "\n");
	}
	writeIdent();
	fprintf(f, "\"%s\" : \"%s\"", name, value);
	++_items;
}

void JSONWriter::write(const char* name, bool value) {

	if (_items != 0) {
		fprintf(f, " , \n");
	}
	else {
		fprintf(f, "\n");
	}
	writeIdent();
	if (value) {
		fprintf(f, "\"%s\" : \"true\"", name, value);
	}
	else {
		fprintf(f, "\"%s\" : \"false\"", name);
	}
	++_items;
}

void JSONWriter::write(const char* name, const v2& value) {
	
	if (_items != 0) {
		fprintf(f, " , \n");
	}
	else {
		fprintf(f, "\n");
	}
	writeIdent();
	int x = value.x;
	int y = value.y;
	fprintf(f, "\"%s\" : \"%d,%d\"", name, x,y);
	++_items;

}
void JSONWriter::write(const char* name, const ds::Color& value) {
	
	if (_items != 0) {
		fprintf(f, " , \n");
	}
	else {
		fprintf(f, "\n");
	}
	writeIdent();
	int r = value.r * 255.0f;
	int g = value.g * 255.0f;
	int b = value.b * 255.0f;
	int a = value.a * 255.0f;
	fprintf(f, "\"%s\" : \"%d,%d,%d,%d\"", name, r, g, b, a);
	++_items;
}
void JSONWriter::write(const char* name, const ds::Rect& value) {
	
	if (_items != 0) {
		fprintf(f, " , \n");
	}
	else {
		fprintf(f, "\n");
	}
	writeIdent();
	fprintf(f, "\"%s\" : \"%d,%d,%d,%d\"", name, value.top,value.left,value.width(),value.height());
	++_items;
}

void JSONWriter::endCategory() {
	writeIdent();
	fprintf_s(f, "\n}\n");
	_open = false;
	_ident -= 4;
}

void JSONWriter::writeIdent() {
	for (int i = 0; i < _ident; ++i) {
		fprintf(f, " ");
	}
}