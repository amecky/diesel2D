/* 
 * File:   Log.cpp
 * Author: mecky
 * 
 * Created on 3. November 2010, 08:05
 */

#include "Log.h"
#include <windows.h>
#include <stdio.h>

void ConsoleOutputHandler::write(const std::string& message) {
	OutputDebugStringA(message.c_str());
}

FILE*& FileOutputHandler::Stream() {
    static FILE* pStream = stderr;
    return pStream;
}

void FileOutputHandler::write(const std::string& message) {
	OutputDebugStringA(message.c_str());
	FILE* pStream = Stream();
	if ( pStream ) {
		fprintf(pStream, "%s", message.c_str());
		fflush(pStream);
	}
}

Log::Log() {}

Log::Log(const Log& orig) {}

std::ostringstream& Log::get() {
    os << NowTime();
    os << " : ";
    return os;
}

std::ostringstream& Log::get(const char *file, const unsigned long line) {
	os << NowTime();
	os << " : [";
	os << file;
	os << " : ";
	os << line;
	os << "] ";
	return os;
}
std::ostringstream& Log::error() {
	os << NowTime();
	os << " [ERROR] : ";
	return os;
}

std::ostringstream& Log::error(const std::string& category) {
	os << NowTime();
	os << " [ERROR] : ";
	os << " [" << category << "] : ";
	return os;
}

std::ostringstream& Log::get(const std::string& category) {
	os << NowTime();
	os << " [" << category << "] : ";
	return os;
}

Log::~Log() {
    os << std::endl;
	OutputDebugStringA(os.str().c_str());
	//handler().write(os.str());
}

LogOutputHandler& Log::handler() {
	static ConsoleOutputHandler coh;
	return coh;	
}

std::string Log::NowTime() {
    const int MAX_LEN = 200;
    char buffer[MAX_LEN];
    if (GetTimeFormatA(LOCALE_USER_DEFAULT, 0, 0,
            "HH':'mm':'ss", buffer, MAX_LEN) == 0)
        return "Error in NowTime()";

    char result[100] = {0};
    static DWORD first = GetTickCount();
    std::sprintf(result, "%s.%03ld", buffer, (long)(GetTickCount() - first) % 1000);
    return result;
}
