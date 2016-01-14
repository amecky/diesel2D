#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include "..\math\math_types.h"
#include "StringUtils.h"
#include "Color.h"
#include "..\dxstdafx.h"
#include "Log.h"
#include "..\math\GameMath.h"
#include "..\math\FloatArray.h"
#include "..\lib\collection_types.h"

// -------------------------------------------------------
// Category
// -------------------------------------------------------
enum SectionType {
	ST_FLOAT,
	ST_STRING,
	ST_VECTOR3,
	ST_VECTOR2,
	ST_RECT,
	ST_INT,
	ST_EOL
};

struct SectionEntry {

	IdString hash;
	char name[32];
	SectionType type;
	char str[256];
	float values[4];
	int count;

	SectionEntry() : hash(0), type(ST_EOL), count(0) {}

};

class Category {

typedef ds::Array<SectionEntry*> Entries;

public:
	Category(const std::string& name);
	~Category();
	const size_t numProperties() const;
	const std::string& getName() const;
	void addProperty(const char* name, const char* value);
	const char* getPropertyName(int index) const;
	const char* getProperty(const char* name) const;
	Vector2f getVector2f(const char* name, const Vector2f& defaultValue = Vector2f(1.0f, 1.0f));
	void getVector2f(const char* name, Vector2f* ret);
	void getIdString(const char* name, IdString* ret);
	Vector3f getVector3f(const char* name);
	Vector3f getVector3f(const char* name, const Vector3f& defaultValue);
	ds::Color getColor(const char* name, const ds::Color& defaultValue = ds::Color::WHITE);
	void getColor(const char* name, ds::Color* color);
	ds::Rect getRect(const char* name);
	void getRect(const char* name, ds::Rect* rect);
	bool getBool(const char* name, bool defaultValue = false);
	void getBool(const char* name, bool* ret);
	void getColorPath(ds::ColorPath* path);
	bool hasProperty(const char* name) const;
	void addCategory(Category* cat);
	const ds::Array<Category*>& getChildren() const;
	Category* getChild(const char* name) const;
	void getFloat(const char* name, float* ret);
	float getFloat(const char* name, float defaultValue = 0.0f);
	uint32 getUInt32(const char* name, uint32 defaultValue);	
	void getInt(const char* name, int* value);
	int getInt(const char* name, int defaultValue = 0);
	void getInt(const char* name, uint32* value);	
	int getElementCount(const char* name);
private:
	int getInt(int index, const char* name);
	float getFloat(int index, const char* name);
	SectionEntry* getEntry(const char* name) const;
	Entries m_Entries;
	std::string m_Name;
	ds::Array<Category*> m_Children;
};

// -------------------------------------------------------
// JSON Reader
// -------------------------------------------------------
class JSONReader {

public:
	JSONReader();
	virtual ~JSONReader();
	bool parse(const char* fileName);   
	ds::Array<Category*>& getCategories() {
		return m_Categories;
	}
	Category* getCategory(const std::string& name);
	const uint32 numCategories() const {
		return m_Categories.size();
	}
	Category* getCategory(uint32 index) {
		assert(index < m_Categories.size());
		return m_Categories[index];
	}
	void clear();
private:
	ds::Array<Category*> m_Categories;
};

// -------------------------------------------------------
// JSON Reader
// -------------------------------------------------------
namespace json {

	Category* read_simplified_json(const char* file_name);

	bool read_simplified_json(const char* file_name,Category* root);

}
