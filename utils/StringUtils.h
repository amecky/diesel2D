#pragma once
#include <vector>
#include <string>
#include "..\dxstdafx.h"

namespace ds {

namespace string {

std::vector<std::string> split(const std::string& str,const char delimiter = ',');

void split(const std::string& str,std::vector<std::string>& list,const char delimiter = ',');

void trim(std::string& str);

void trim(std::string& str,const std::string& ws);

void formatInt(int value,std::string& str,int length);

void formatInt(int value,std::string& str);

void formatTime(int minutes,int seconds,std::string& str);

IdString murmur_hash(const char* text);

IdString murmur_hash(const void * key, int len, unsigned int seed);

void formatDateTime(std::string& str);

bool isDigit(const char c);

bool isWhitespace(const char c);

float strtof(const char* p,char** endPtr);

bool isCharacter(const char p);


template <class T>
inline std::string to_string (const T& t) {
	std::stringstream ss;
	ss << t;
	return ss.str();
}

template<class T> 
T inline from_string(const std::string& s) {
	std::istringstream stream (s);
	T t;
	stream >> t;
	return t;
}


}

}

