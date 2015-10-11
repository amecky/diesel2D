#pragma once
#include <vector>
#include <string>
#include <Windows.h>

namespace ds {

struct FSFile {

	char directory[128];
	char name[128];
	size_t size;
	FILETIME time;
};

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

	void listDirectory(const char* dir,std::vector<std::string>& files);
	
	bool loadTextFile(const char* name,std::vector<std::string>& lines);
	
	void getFileTime(const char* fileName,FILETIME& time);

	bool compareFileTime(const char* fileName,const FILETIME& time);

	bool isOlder(const char* firstFile, const char* secondFile);
	//! Check if a file exists
	/*
		\param name the actual file include directories
	*/
	bool fileExists(const char* name);

	void copyFile(const char* source,const char* dest);

	void logFileTime(const char* fileName);

	void logFileTime(const FILETIME& time);

	template<class T>
	void saveBinary(const char* fileName, const T& t) {
		IdString str = ds::string::murmur_hash(fileName);
		char buffer[128];
		sprintf_s(buffer, 128, "data\\%d", str);
		FILE* f = fopen(buffer, "wb");
		fwrite(&t, sizeof(T), 1, f);
		fclose(f);
	}

	template<class T>
	void loadBinary(const char* fileName, T* t) {
		IdString str = ds::string::murmur_hash(fileName);
		char buffer[128];
		sprintf_s(buffer, 128, "data\\%d", str);
		FILE* f = fopen(buffer, "rb");
		fread(&t, sizeof(T), 1, f);
		fclose(f);
	}
}

class FSDirectory {

	typedef std::vector<std::string> Files;

public:
	FSDirectory(const char* dirName) : m_Dir(dirName) {}
	~FSDirectory() {}
	const char* getDirName() const {
		return m_Dir;
	}
	void list() {
		file::listDirectory(m_Dir,m_Files);
	}
	int numFiles() {
		return m_Files.size();
	}
	const std::string& getFileName(int idx) const {
		return m_Files[idx];
	}
	std::string getAbsoluteFileName(int idx) {
		char buffer[1024];
		sprintf(buffer,"%s\\%s",m_Dir,m_Files[idx].c_str());
		return std::string(buffer);
	}
	std::string getEnding(int idx) {
		std::string fileName = m_Files[idx];
		if ( fileName.find(".") != std::string::npos ) {
			return fileName.substr(fileName.find(".")+1);
		}
		return m_Files[idx];
	}

	std::string getFileNameNoEnding(int idx) {
		std::string fileName = m_Files[idx];
		if ( fileName.find(".") != std::string::npos ) {
			return fileName.substr(0,fileName.find("."));
		}
		return m_Files[idx];
	}
private:
	const char* m_Dir;
	Files m_Files;
};

};

