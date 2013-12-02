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

// -------------------------------------------------------
// Sprite settings
// -------------------------------------------------------
struct SpriteSetting {
	char fileName[32];
	IdString name;
	Sprite* sprite;
	FILETIME fileTime;
};


typedef std::vector<SpriteSetting> SpriteSettings;
typedef std::vector<SettingsFile> SettingsFiles;

public:
	SettingsManager(void);
	~SettingsManager(void);
	void loadSettings(const char* fileName,DynamicSettings* settings,bool check = true);
	bool hasSettings(const char* fileName);
	DynamicSettings* getSettings(const char* fileName);
	void checkSettingsFiles();

	ds::Sprite* loadSpriteSettings(const char* name);	
	ds::Sprite* getSpriteSettings(const char* name);
	bool hasSpriteSettings(const char* name);
private:

	void loadInternalSpriteSettings(SpriteSetting* setting);
	void loadBinarySettings(SpriteSetting* setting);
	void saveBinarySettings(SpriteSetting* setting);
	void reloadSpriteSettings();

	SettingsFiles m_Files;
	SpriteSettings m_Settings;
};

}

