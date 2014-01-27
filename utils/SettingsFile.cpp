#include "SettingsFile.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include "Log.h"
#include "StringUtils.h"

namespace ds {

SettingsFile::SettingsFile(const char* fileName) : m_FileName(fileName) {
}


SettingsFile::~SettingsFile(void) {
}

// -------------------------------------------------------
// Parse file
// -------------------------------------------------------
bool SettingsFile::parse() {
	std::string line;
	Category* current = 0;
	std::ifstream infile(m_FileName, std::ios_base::in);
	if ( infile.is_open() ) {
		LOG(logINFO) << "File " << m_FileName << " opened for reading";
		while (std::getline(infile, line, '\n')) {
			string::trim(line);
			if ( line.length() > 0 && line.find("#") == std::string::npos ) {
				// check if we have a category
				if ( line.find("[") != std::string::npos ) {
					std::string catName = line.substr(1);
					catName = catName.substr(0,catName.length()-1);
					Category c;
					c.name = catName;
					c.hash = string::murmur_hash(catName.c_str());
					c.index = m_Categories.size();
					m_Categories.push_back(c);
					current = &m_Categories[c.index];
				}
				// it is a property
				else {
					std::string::size_type pos = line.find('=');
					if (pos != std::string::npos && current != 0 ) {
						std::string name = line.substr(0, pos);
						ds::string::trim(name);
						std::string value = line.substr(pos+1);
						ds::string::trim(value);
						Setting s;
						s.hash = string::murmur_hash(name.c_str());
						s.value = value;
						int idx = m_Settings.size();
						m_Settings.push_back(s);
						Cat2Setting c2s;
						c2s.categoryID = current->index;
						c2s.settingIndex = idx;
						m_Lookup.push_back(c2s);
					}
				}
			}
		}
		infile.close();		
	}
	else {
		LOG(logERROR) << "Cannot open file " << m_FileName;
		return false;
	}	
	return true;
}

// -------------------------------------------------------
// Contains category
// -------------------------------------------------------
bool SettingsFile::contains(const char* category) {
	IdString catHash = string::murmur_hash(category);
	for ( size_t i = 0; i < m_Categories.size(); ++i ) {
		Category* c = &m_Categories[i];
		if ( c->hash == catHash ) {
			return true;
		}
	}
	return false;
}

// -------------------------------------------------------
// Get string
// -------------------------------------------------------
void SettingsFile::get(const char* category,const char* name,std::string&  ret) {
	Setting* setting = find(category,name);
	if ( setting != 0 ) {
		ret = setting->value;
	}
}

// -------------------------------------------------------
// Find
// -------------------------------------------------------
Setting* SettingsFile::find(const char* category,const char* name) {
	IdString catHash = string::murmur_hash(category);
	IdString setHash = string::murmur_hash(name);
	for ( size_t i = 0; i < m_Categories.size(); ++i ) {
		Category* c = &m_Categories[i];
		if ( c->hash == catHash ) {
			for ( size_t j = 0; j < m_Lookup.size(); ++j ) {
				Cat2Setting* c2s = &m_Lookup[j];
				if ( c2s->categoryID == c->index ) {
					Setting* s = &m_Settings[c2s->settingIndex];
					if ( s->hash == setHash ) {
						return s;
					}					
				}
			}
		}
	}
	return 0;
}

// -------------------------------------------------------
// Get float array
// -------------------------------------------------------
void SettingsFile::get(const char* category,const char* first,const char* second,ds::FloatArray* array) {
	Setting* firstSetting = find(category,first);
	Setting* secondSetting = find(category,second);
	if ( firstSetting != 0 && secondSetting != 0 ) {
		array->reset();
		std::vector<std::string> firstEntries;
		string::split(firstSetting->value,firstEntries,' ');
		std::vector<std::string> secondEntries;
		string::split(secondSetting->value,secondEntries,' ');
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

// -------------------------------------------------------
// Get float
// -------------------------------------------------------
void SettingsFile::get(const char* category,const char* name,float* value) {
	Setting* set = find(category,name);
	if ( set != 0 ) {		
		if ( set->value.find("[") != std::string::npos ) {
			std::string nv = set->value.substr(1,set->value.length()-2);
			std::vector<std::string> entries;
			string::split(nv,entries,'-');
			float min = convert<float>(entries[0]);
			float max = convert<float>(entries[1]);
			*value = math::random(min,max);
		}
		else {
			get<float>(category,name,value);
		}
	}		
}

}
