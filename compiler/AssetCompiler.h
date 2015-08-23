#pragma once
#include "Converter.h"
#include <map>
#include "..\io\Serializer.h"
#include <vector>
#include "..\renderer\shader.h"

namespace ds {

const uint32 CVT_HUD                 = 0;
const uint32 CVT_GUI                 = 1;
const uint32 CVT_DIALOG              = 2;
const uint32 CVT_FONT                = 3;
const uint32 CVT_PARTICLESYSTEM      = 4;
const uint32 CVT_SPRITE              = 5;
const uint32 CVT_PARTICLEMANAGER     = 6;
const uint32 CVT_BLOOM_COMPONENT     = 7;
const uint32 CVT_SPRITES_DESCRIPTION = 8;
const uint32 CVT_SCRIPT              = 9;
const uint32 CVT_BLUEPRINT           = 10;
const uint32 CVT_NPS                 = 11;
const uint32 CVT_PATH                = 14;
const uint32 CVT_STRAIGHT_PATH       = 15;
const uint32 CVT_DESCRIPTOR          = 18;
const uint32 CVT_MESH                = 19;
const uint32 CVT_SHADER              = 20;
const uint32 BINL_TEXTURE            = 0;

// -------------------------------------------------------
// Asset database
// -------------------------------------------------------
class AssetDB {

struct AssetDBEntry {

	char fileName[256];
	FILETIME fileTime;

};

typedef std::vector<AssetDBEntry> Entries;

public:
	AssetDB() {}
	~AssetDB() {}
	void readDB();
	void saveDB();
	void add(const char* name,FILETIME fileTime);
	void update(const char* name,FILETIME fileTime);
	bool hasChanged(const char* fileName);
	bool contains(const char* fileName);
private:
	
	Entries m_Entries;
};

// -------------------------------------------------------
// Asset compiler
// -------------------------------------------------------
class AssetCompiler {

struct FileWatch {
	char jsonName[256];
	char binaryName[256];
	Serializer* serializer;
	FILETIME fileTime;
	uint32 type;
	IdString hash;
	bool binary;
};

typedef std::map<uint32,Converter*> ConverterMapping;
typedef std::map<uint32,BinarySerializer*> BinarySerializerMapping;
typedef std::vector<FileWatch> WatchList;

public:
	AssetCompiler();
	~AssetCompiler();
	void update();
	uint32 registerConverter(Converter* converter);	
	void load(const char* fileName,Serializer* serializer,uint32 type);
	int loadBinary(const char* fileName,uint32 type);
	uint32 getConvertedID(const char* name);
private:
	uint32 m_CustomIndex;
	void reload(FileWatch* watch);
	ConverterMapping m_Mapping;
	BinarySerializerMapping m_BinarySerializerMapping;
	WatchList m_WatchList;
	AssetDB m_Database;
};

class ParticleManager;

namespace assets {

	void load(const char* fileName, Serializer* serializer, uint32 type);

	void loadParticleSystem(const char* fileName, ParticleManager* particleManager);

	void loadSpriteTemplates(const char* fileName);

	int loadShader(const char* fileName);

	uint32 registerConverter(Converter* converter);

	void update();
}

}

