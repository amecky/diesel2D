#include "FileUtils.h"
#include <fstream>
#include <sstream>
#include <windows.h>
#include <tchar.h> 
#include <stdio.h>
#include <strsafe.h>
#include "Log.h"

namespace ds {

FileUtils::FileUtils() { }

FileUtils::~FileUtils() { }

namespace file {

	bool fileExists(const char* name) {
		if (FILE * file = fopen(name, "r")) {
			fclose(file);
			return true;
		}
		return false;
	}

	bool loadTextFile(const char* name,std::vector<std::string>& lines) {    
		std::string line;
		std::ifstream myfile(name);
		if (myfile.is_open()) {
			while ( std::getline(myfile, line)) {
				if (line.find("#", 0) == std::string::npos && line.size() > 0) {					
					lines.push_back(line);                
				}
			}
			myfile.close();    
			return true;
		}		
		return false;
	}

	void getFileTime(const char* fileName,FILETIME& time) {
		WORD ret = -1;
		HANDLE hFile = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,OPEN_EXISTING, 0, NULL);
		if( hFile != INVALID_HANDLE_VALUE) {		
			// Retrieve the file times for the file.
			GetFileTime(hFile, NULL, NULL, &time);
			CloseHandle(hFile);    
		}            
	}

	void listDirectory(const char* dir,std::vector<std::string>& files) {
		std::string directory = dir;
		WIN32_FIND_DATAA ffd;
		//LARGE_INTEGER filesize;
		//TCHAR szDir[MAX_PATH];
		//size_t length_of_arg;
		HANDLE hFind = INVALID_HANDLE_VALUE;
		//DWORD dwError = 0;

		if ( directory.find_last_of("\\") != directory.length() ) {
			directory.append("\\");
		}		
		hFind = FindFirstFileA((directory+"*").c_str(), &ffd);		
		if ( INVALID_HANDLE_VALUE != hFind ) {      
			do {
				if ( !(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ) {         
					files.push_back(ffd.cFileName);				
				}
			}
			while (FindNextFileA(hFind, &ffd) != 0);
			FindClose(hFind);
		}
		else {
			LOG(logERROR) << "cannot find directory " << dir;
		}
	}

	// -------------------------------------------------------
	// Checks if the file has changed
	// -------------------------------------------------------
	bool compareFileTime(const char* fileName,const FILETIME& time) {
		FILETIME now;
		getFileTime(fileName,now);
		int t = CompareFileTime(&time,&now);		
		if ( t == -1 ) {
			return true;
		}
		return false;
	}

}

void FileUtils::listDirectory(const std::string& dir,std::vector<std::string>& files,bool appendWildcard) {
	std::string directory = dir;
	WIN32_FIND_DATAA ffd;
	//LARGE_INTEGER filesize;
	//TCHAR szDir[MAX_PATH];
	//size_t length_of_arg;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	//DWORD dwError = 0;
	if ( directory.find_last_of("\\") != directory.length() ) {
		directory.append("\\");
	}
	if ( appendWildcard ) {
		hFind = FindFirstFileA((directory+"*").c_str(), &ffd);
	}
	else {
		hFind = FindFirstFileA(directory.c_str(), &ffd);
	}
	if ( INVALID_HANDLE_VALUE != hFind ) {      
		do {
			if ( !(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ) {         
				files.push_back(ffd.cFileName);				
			}
		}
		while (FindNextFileA(hFind, &ffd) != 0);
		FindClose(hFind);
	}
	else {
		LOG(logERROR) << "cannot find directory " << dir.c_str();
	}
}

std::vector<std::string> FileUtils::listDirectory(const std::string& dir) {
	WIN32_FIND_DATAA ffd;
	LARGE_INTEGER filesize;
	TCHAR szDir[MAX_PATH];
	size_t length_of_arg;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD dwError=0;
	std::vector<std::string> files;
	hFind = FindFirstFileA(dir.c_str(), &ffd);
	if ( INVALID_HANDLE_VALUE != hFind ) {      
		do {
			if ( !(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ) {         
				files.push_back(ffd.cFileName);				
			}
		}
		while (FindNextFileA(hFind, &ffd) != 0);
		FindClose(hFind);
	}
	else {
		LOG(logERROR) << "cannot find directory " << dir.c_str();
	}
	return files;
}

FILETIME* FileUtils::getFileTime(const std::string& fileName) {
	WORD ret = -1;
	HANDLE hFile = CreateFile(fileName.c_str(), GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,OPEN_EXISTING, 0, NULL);
    if( hFile != INVALID_HANDLE_VALUE) {
		FILETIME* ftWrite = new FILETIME();	
		// Retrieve the file times for the file.
		if (GetFileTime(hFile, NULL, NULL, ftWrite)) {
			return ftWrite;
		}
		CloseHandle(hFile);    
	}            
	return 0;
}

};
