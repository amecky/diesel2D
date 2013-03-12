#pragma once
#include <vector>
#include <string>
#include <Windows.h>

namespace ds {

class FileUtils {

public:
	static std::vector<std::string> listDirectory(const std::string& dir);
	static void listDirectory(const std::string& dir,std::vector<std::string>& files,bool appendWildcard = true);
	static FILETIME* getFileTime(const std::string& fileName);
private:
	FileUtils();
	~FileUtils();
};

namespace file {

	bool loadTextFile(const char* name,std::vector<std::string>& lines);
}

};

