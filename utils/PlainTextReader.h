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
	ds::Vec2 getVec2(const std::string& name) {
		float x = getFloat(0,name);
		float y = getFloat(1,name);
		return ds::Vec2(x,y);
	}
	void getVec2(const std::string& name,ds::Vec2* ret) {
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
	ds::Vec3 getVec3(const std::string& name) {
		float x = getFloat(0,name);
		float y = getFloat(1,name);
		float z = getFloat(2,name);
		return ds::Vec3(x,y,z);
	}
	ds::Color getColor(const std::string& name) {
		assert(getElementCount(name) == 4);
		int r = getInt(0,name);
		int g = getInt(1,name);
		int b = getInt(2,name);
		int a = getInt(3,name);
		return ds::Color(r,g,b,a);
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
	float getRandomFloat(const char* name);
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

// -------------------------------------------------------
// NewSettingsReader
// -------------------------------------------------------
const std::string EMPTY = "EMPTY";

class NewSettingsReader {

struct Setting {
	char name[20];
	IdString hash;
	std::string value;
};

typedef std::vector<Setting> Settings;

public:
	NewSettingsReader() {}
	~NewSettingsReader() {}
	bool parse(const char* fileName);
	bool contains(const char* name) {
		return find(name) != 0;
	}
	const std::string& getString(const char* name) {
		Setting* setting = find(name);
		if ( setting != 0 ) {
			return setting->value;
		}
		return EMPTY;
	}
	void get(const char* first,const char* second,ds::FloatArray* array) {
		Setting* firstSetting = find(first);
		Setting* secondSetting = find(second);
		//LOG(logINFO) << "first " << firstSetting->value;
		//LOG(logINFO) << "second " << secondSetting->value;
		if ( firstSetting != 0 && secondSetting != 0 ) {
			array->reset();
			std::vector<std::string> firstEntries;
			ds::string::split(firstSetting->value,firstEntries,' ');
			std::vector<std::string> secondEntries;
			ds::string::split(secondSetting->value,secondEntries,' ');
			if ( firstEntries.size() == secondEntries.size() ) {
				for ( size_t i = 0; i < firstEntries.size(); ++i ) {
					float x = convert<float>(firstEntries[i]);
					float t = convert<float>(secondEntries[i]);
					array->add(t,x);
				}
			}
			else {
				LOGC(logINFO,"NewSettingsReader") << "Found uneven settings for: " << first << " and " << second;
			}
		}
		else {
			LOGC(logINFO,"NewSettingsReader") << "Cannot find matching settings for: " << first << " and " << second;
		}
	}
	void get(const char* name,float* value) {
		Setting* set = find(name);
		if ( set != 0 ) {		
			if ( set->value.find("[") != std::string::npos ) {
				std::string nv = set->value.substr(1,set->value.length()-2);
				std::vector<std::string> entries;
				ds::string::split(nv,entries,'-');
				float min = convert<float>(entries[0]);
				float max = convert<float>(entries[1]);
				*value = ds::math::random(min,max);
			}
			else {
				get<float>(name,value);
			}
		}		
	}
	template<class T>
	void get(const char* name,T* value) {
		Setting* set = find(name);
		if ( set != 0 ) {					
			T t;
			std::istringstream ist(set->value);
			ist >> t;
			*value = t;			
		}		
	}
	size_t num() {
		return m_Settings.size();
	}
private:
	Setting* NewSettingsReader::find(const char* name) {
		IdString hash = ds::string::murmur_hash(name);
		for ( size_t i = 0; i < m_Settings.size(); ++i ) {
			Setting* set = &m_Settings[i];
			if ( set->hash == hash ) {
				return set;
			}
		}
		LOGC(logINFO,"NewSettingsReader") << "Sorry - but " << name << " was not found";
		return 0;
	}
	template<class T>
	T convert(const std::string& value) {
		T t;
		std::istringstream ist(value);
		ist >> t;
		return t;
	}
	Settings m_Settings;
};