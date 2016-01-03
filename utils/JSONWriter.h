#pragma once
#include <stdio.h>
#include <Vector.h>
#include "..\renderer\render_types.h"
#include "..\math\FloatArray.h"

class JSONWriter {

public:
	JSONWriter();
	~JSONWriter();
	bool open(const char* fileName);
	void startCategory(const char* name);
	void write(const char* name, int value);
	void write(const char* name, uint32 value);
	void write(const char* name, float value);
	void write(const char* name, bool value);
	void write(const char* name, const v2& value);
	void write(const char* name, const v3& value);
	void write(const char* name, const ds::Color& value);
	void write(const char* name, const ds::Rect& value);
	void write(const char* name, const char* value);
	void write(const ds::ColorPath& path);
	void write(const ds::Vector2fPath& path);
	void write(const ds::FloatArray& path);
	void endCategory();
private:
	FILE* f;
	void writeIdent();
	void writeLineIdent();
	int _ident;
	char _temp[128];
	bool _open;
	int _items;
};

