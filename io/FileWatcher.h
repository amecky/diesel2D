#pragma once
#include "Serializer.h"
#include <vector>
#include "..\dxstdafx.h"

namespace ds {

class FileWatcher {

struct FileWatch {
	IdString hashName;
	char filename[256];
	Serializer* serializer;
	FILETIME fileTime;
};

typedef std::vector<FileWatch> WatchList;

public:
	FileWatcher(void);
	~FileWatcher(void);
	void registerFile(const char* fileName,Serializer* serializer);
	void update();
	bool contains(const char* fileName);
private:
	WatchList m_WatchList;
};

extern FileWatcher* gFileWatcher;

}