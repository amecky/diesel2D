#pragma once
#include <string>
#include <vector>
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

// -------------------------------------------------------
// Category
// -------------------------------------------------------
class Category {

typedef std::map<std::string,std::string> PropertyMap;
typedef std::vector<Category*>::iterator CategoryIterator;

public:
	Category(const std::string& name) : m_Name(name) { }
	~Category() {
		std::cout << "deleting category " << m_Name << std::endl;
		CategoryIterator it = m_Children.begin();
		while ( it != m_Children.end() ) {
			delete *(it);
			it = m_Children.erase(it);
		}
	}
	const std::string& getName() const {
		return m_Name;
	}
	void addProperty(const std::string& name,const std::string& value) {
		m_Properties[name] = value;
	}
	std::string getProperty(const std::string& name) {
		return m_Properties[name];
	}
	Vector2f getVector2f(const std::string& name,const Vector2f& defaultValue = Vector2f(1.0f,1.0f)) {
		if ( hasProperty(name)) {
			float x = getFloat(0,name);
			float y = getFloat(1,name);
			return Vector2f(x,y);
		}
		return defaultValue;
	}
	void getVector2f(const std::string& name,Vector2f* ret) {
		if ( hasProperty(name)) {
			ret->x = getFloat(0,name);
			ret->y = getFloat(1,name);
		}
	}
void getIdString(const std::string& name,IdString* ret) {
		if ( hasProperty(name)) {
			*ret = ds::string::murmur_hash(getProperty(name).c_str());
		}
	}
	Vector3f getVector3f(const std::string& name) {
		float x = getFloat(0,name);
		float y = getFloat(1,name);
		float z = getFloat(2,name);
		return Vector3f(x,y,z);
	}
	ds::Color getColor(const std::string& name,const ds::Color& defaultValue = ds::Color::WHITE) {
		if ( hasProperty(name)) {
			assert(getElementCount(name) == 4);
			int r = getInt(0,name);
			int g = getInt(1,name);
			int b = getInt(2,name);
			int a = getInt(3,name);
			return ds::Color(r,g,b,a);
		}
		return defaultValue;
	}

	void getColor(const std::string& name,ds::Color* color) {
		if ( hasProperty(name)) {
			assert(getElementCount(name) == 4);
			int r = getInt(0,name);
			int g = getInt(1,name);
			int b = getInt(2,name);
			int a = getInt(3,name);
			color->r = static_cast<float>(r) / 255.0f;
			color->g = static_cast<float>(g) / 255.0f;
			color->b = static_cast<float>(b) / 255.0f;
			color->a = static_cast<float>(a) / 255.0f;
		}
	}
	ds::Rect getRect(const std::string& name) {
		float top = static_cast<float>(getInt(0,name));
		float left = static_cast<float>(getInt(1,name));
		float width = static_cast<float>(getInt(2,name));
		float height = static_cast<float>(getInt(3,name));
		return ds::Rect(top,left,width,height);
	}
	void getRect(const std::string& name,ds::Rect* rect) {
		if ( hasProperty(name)) {
			rect->top = static_cast<float>(getInt(0,name));
			rect->left = static_cast<float>(getInt(1,name));
			rect->right = rect->left + static_cast<float>(getInt(2,name));
			rect->bottom = rect->top + static_cast<float>(getInt(3,name));
		}		
	}
	bool getBool(const std::string& name,bool defaultValue) {
		if ( m_Properties.find(name) == m_Properties.end() ) {
			return defaultValue;
		}
		std::string s = m_Properties[name];
		if ( s == "true") {
			return true;
		}
		return false;
	}
	void getBool(const std::string& name,bool* ret) {
		if ( hasProperty(name) ) {		
			std::string s = m_Properties[name];
			if ( s == "true") {
				*ret = true;
			}
			else {
				*ret = false;
			}
		}
	}
	template<class T> T read(const std::string& name,const T& defaultValue) {
		if ( m_Properties.find(name) == m_Properties.end() ) {
			return defaultValue;
		}
		std::string s = m_Properties[name];
		T t;
		std::istringstream ist(s);
		ist >> t;
		return t;
	}	
	bool hasProperty(const std::string& name) const {
		return m_Properties.find(name) != m_Properties.end();
	}
	void addCategory(Category* cat) {
		m_Children.push_back(cat);
	}
	std::vector<Category*>& getChildren() {
		return m_Children;
	}
	void getPropertyNames(std::vector<std::string>& propertyNames) {
		PropertyMap::iterator it = m_Properties.begin();
		while ( it != m_Properties.end()) {
			propertyNames.push_back(it->first);
			++it;
		}
	}
	void getFloat(const std::string& name,float* ret) {
		if ( hasProperty(name)) {
			*ret = getFloat(0,name);
		}
	}
	float getFloat(const std::string& name,float defaultValue) {
		if ( hasProperty(name)) {
			return getFloat(0,name);
		}
		return defaultValue;
	}
	uint32 getUInt32(const std::string& name,uint32 defaultValue) {
		if ( hasProperty(name)) {
			return static_cast<uint32>(getInt(0,name));
		}
		return defaultValue;
	}
	float getFloat(int index,const std::string& name) {
		std::string s = m_Properties[name];
		std::vector<std::string> values = ds::string::split(s);
		float v;
		std::istringstream ist(values[index]);
		ist >> v;
		return v;
	}
	void getInt(const std::string& name,int* value) {
		if ( hasProperty(name)) {
			*value = getInt(0,name);
		}
	}
	int getInt(const std::string& name,int defaultValue) {
		if ( hasProperty(name)) {
			return getInt(0,name);
		}
		return defaultValue;
	}
	void getInt(const std::string& name,uint32* value) {
		if ( hasProperty(name)) {
			*value = static_cast<uint32>(getInt(0,name));
		}
	}
	int getInt(int index,const std::string& name) {
		std::string s = m_Properties[name];
		std::vector<std::string> values = ds::string::split(s);
		int v;
		std::istringstream ist(values[index]);
		ist >> v;
		return v;
	}
	int getElementCount(const std::string& name) {
		std::string s = m_Properties[name];
		std::vector<std::string> values = ds::string::split(s);
		return values.size();
	}
private:
	std::string m_Name;
	std::map<std::string,std::string> m_Properties;
	std::vector<Category*> m_Children;
};

// -------------------------------------------------------
// JSON Reader
// -------------------------------------------------------
class JSONReader {

public:
	JSONReader();
	virtual ~JSONReader();
	bool parse(const char* fileName);   
	std::vector<Category*>& getCategories() {
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
	std::vector<Category*> m_Categories;
};

