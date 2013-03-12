#include "StringUtils.h"
#include <iostream>
#include <sstream>
#include <locale>
#include <ctime>

namespace ds {

namespace string {

// -------------------------------------------------------
// Formats current date and time to string
// -------------------------------------------------------
void formatDateTime(std::string& str) {
	std::stringstream datetime;
	time_t t = time(0);   // get time now
	struct tm * now = localtime( & t );
	datetime << now->tm_mday << "." << (now->tm_mon + 1) << "." << (now->tm_year + 1900) << " " << now->tm_hour << ":" << now->tm_min;
	str = datetime.str();	
}

std::vector<std::string> split(const std::string& str,const char delimiter) {	
	std::vector<std::string> words;
	std::string word;
	std::stringstream stream(str);
	while( getline(stream, word, delimiter) ) {
		words.push_back(word);
	}
	return words;
}

void split(const std::string& str,std::vector<std::string>& words,const char delimiter) {	
	std::string word;
	std::stringstream stream(str);
	while( getline(stream, word, delimiter) ) {
		words.push_back(word);
	}
}

void trim(std::string& str) {
	trim(str," ");
	trim(str,"\t");	
}

void trim(std::string& str,const std::string& ws) {
	std::string::size_type pos = str.find_last_not_of(ws);
	if (pos != std::string::npos) {
		str.erase(pos + 1);
		pos = str.find_first_not_of(ws);
		if (pos != std::string::npos) {
			str.erase(0, pos);
		}
	} else {
		str.erase(str.begin(), str.end());
	}
}

// -------------------------------------------------------
// Formats time in minutes and seconds to string
// -------------------------------------------------------
void formatTime(int minutes,int seconds,std::string& str) {
	std::string secStr;
	formatInt(seconds,secStr,2);
	std::string minStr;
	formatInt(minutes,minStr,2);
	str = minStr+":"+secStr;
}

// -------------------------------------------------------
// Formats an int to a string using fixed length
// -------------------------------------------------------
void formatInt(int value,std::string& str,int length) {
	str = "";

	if (value < 0) {
		str += '-';
		value = -value;
	}
	int tmp = value;
	int div = 1;
	for ( int i = 0; i < length; ++i ) {
		if ( i > 0 ) {
			div *= 10;
		}
	}
	int r = 0;
	for ( int i = 0; i < length ; ++i ) {
		r = tmp / div;
		str += ('0' + r );
		tmp = tmp - r * div;
		div /= 10;        
	}    
}

// -------------------------------------------------------
// Formats an int to a string
// -------------------------------------------------------
void formatInt(int value,std::string& str) {
	char buffer[20];
	sprintf(buffer,"%d",value);
	int length = strlen(buffer);
	return formatInt(value,str,length);
}

IdString murmur_hash(const char* text) {
	return murmur_hash(text,strlen(text),0);
}
//-----------------------------------------------------------------------------
// MurmurHash2, by Austin Appleby
//-----------------------------------------------------------------------------
IdString murmur_hash(const void * key, int len, unsigned int seed) {
	// 'm' and 'r' are mixing constants generated offline.
	// They're not really 'magic', they just happen to work well.

	const unsigned int m = 0x5bd1e995;
	const int r = 24;

	// Initialize the hash to a 'random' value

	IdString h = seed ^ len;

	// Mix 4 bytes at a time into the hash

	const unsigned char * data = (const unsigned char *)key;

	while(len >= 4)
	{
		unsigned int k = *(unsigned int *)data;

		k *= m; 
		k ^= k >> r; 
		k *= m; 

		h *= m; 
		h ^= k;

		data += 4;
		len -= 4;
	}

	// Handle the last few bytes of the input array

	switch(len)
	{
	case 3: h ^= data[2] << 16;
	case 2: h ^= data[1] << 8;
	case 1: h ^= data[0];
		h *= m;
	};

	// Do a few final mixes of the hash to ensure the last few
	// bytes are well-incorporated.

	h ^= h >> 13;
	h *= m;
	h ^= h >> 15;

	return h;
}

}

}
