#pragma once
#include <string>
#include <vector>
#include "..\dxstdafx.h"
#include "..\math\FloatArray.h"
#include "..\math\GameMath.h"

namespace ds {

struct Setting {
	IdString hash;
	std::string value;
};

class SettingsFile {

struct Category {
	IdString hash;
	std::string name;
	int index;
};



struct Cat2Setting {
	int categoryID;
	int settingIndex;
};

typedef std::vector<Category> Categories;
typedef std::vector<Setting> Settings;
typedef std::vector<Cat2Setting> Lookup;

public:
	SettingsFile(const char* fileName);
	~SettingsFile(void);
	bool parse();
	bool contains(const char* category);
	void get(const char* category,const char* name,std::string& ret);
	void get(const char* category,const char* first,const char* second,ds::FloatArray* array);
	void get(const char* category,const char* name,float* value);
	template<class T>
	void get(const char* category,const char* name,T* value) {
		Setting* set = find(category,name);
		if ( set != 0 ) {					
			T t;
			std::istringstream ist(set->value);
			ist >> t;
			*value = t;			
		}		
	}
private:
	template<class T>
	T convert(const std::string& value) {
		T t;
		std::istringstream ist(value);
		ist >> t;
		return t;
	}
	Setting* find(const char* category,const char* name);
	const char* m_FileName;
	Categories m_Categories;
	Settings m_Settings;
	Lookup m_Lookup;
};

}

