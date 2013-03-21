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
	std::string& getName() {
		return m_Name;
	}
	void addProperty(const std::string& name,const std::string& value) {
		m_Properties[name] = value;
	}
	std::string getProperty(const std::string& name) {
		return m_Properties[name];
	}
	ds::Vec2 getVec2(const std::string& name) {
		float x = getFloat(0,name);
		float y = getFloat(1,name);
		return ds::Vec2(x,y);
	}
	ds::Vec3 getVec3(const std::string& name) {
		float x = getFloat(0,name);
		float y = getFloat(1,name);
		float z = getFloat(2,name);
		return ds::Vec3(x,y,z);
	}
	ds::Color getColor(const std::string& name) {
		int r = getInt(0,name);
		int g = getInt(1,name);
		int b = getInt(2,name);
		int a = getInt(3,name);
		return ds::Color(r,g,b,a);
	}
	ds::Rect getRect(const std::string& name) {
		int top = getInt(0,name);
		int left = getInt(1,name);
		int width = getInt(2,name);
		int height = getInt(3,name);
		return ds::Rect(top,left,width,height);
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
private:
	float getFloat(int index,const std::string& name) {
		std::string s = m_Properties[name];
		std::vector<std::string> values = ds::string::split(s);
		float v;
		std::istringstream ist(values[index]);
		ist >> v;
		return v;
	}
	int getInt(int index,const std::string& name) {
		std::string s = m_Properties[name];
		std::vector<std::string> values = ds::string::split(s);
		int v;
		std::istringstream ist(values[index]);
		ist >> v;
		return v;
	}
	std::string m_Name;
	std::map<std::string,std::string> m_Properties;
	std::vector<Category*> m_Children;
};

class PlainTextReader {

public:
	PlainTextReader();
	virtual ~PlainTextReader() {
		delete m_Root;
	}
	bool readFile(const char* fileName);
	Category* getRoot() {
		return m_Root;
	}
private:
	Category* m_Root;
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
	void clear();
private:
	std::vector<Category*> m_Categories;
};

// -------------------------------------------------------
// Settings reader
// -------------------------------------------------------
class SettingsReader {

typedef std::map<std::string,std::string> Settings;

public:
	SettingsReader();
	~SettingsReader();
	bool parse(const char* fileName);
	std::string& getValue(const std::string& name);
	void getNames(std::vector<std::string>& names);
	template<class T> T get(const std::string& name,const T& defaultValue) {
		if ( m_Settings.find(name) == m_Settings.end() ) {
			return defaultValue;
		}
		std::string s = m_Settings[name];
		T t;
		std::istringstream ist(s);
		ist >> t;
		return t;
	}
private:
	Settings m_Settings;
};