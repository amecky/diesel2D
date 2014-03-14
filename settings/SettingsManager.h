#pragma once
#include "DynamicSettings.h"
#include "..\renderer\render_types.h"

namespace ds {

class SettingsManager {

struct SettingsFile {
	std::string name;
	IdString hashName;
	DynamicSettings* settings;
	FILETIME fileTime;
};

typedef std::vector<SettingsFile> SettingsFiles;

public:
	SettingsManager(void);
	~SettingsManager(void);
	void loadSettings(const char* fileName,DynamicSettings* settings,bool check = true);
	bool hasSettings(const char* fileName);
	DynamicSettings* getSettings(const char* fileName);
	void checkSettingsFiles();
private:
	SettingsFiles m_Files;
};

}

