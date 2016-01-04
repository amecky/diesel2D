#pragma once
#include <vector>
#include "..\dxstdafx.h"
/*
namespace ds {

class FileWatcher {

struct FileWatch {
	IdString hashName;
	char filename[256];
	char otherName[256];
	Serializer* serializer;
	FILETIME fileTime;
};

typedef std::vector<FileWatch> WatchList;

public:
	FileWatcher(void);
	~FileWatcher(void);
	void registerFile(const char* fileName,Serializer* serializer);
	void registerFile(const char* fileName,const char* name,Serializer* serializer);
	void update();
	bool contains(const char* fileName);
private:
	WatchList m_WatchList;
};

}

extern ds::FileWatcher* gFileWatcher;
*/