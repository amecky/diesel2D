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

std::ostringstream& Log::get(LogLevel level) {
    os << NowTime();
    os << " [" << toString(level) << "] : ";
    os << std::string(level > logDEBUG ? level - logDEBUG : 0, '\t');
    return os;
}

std::ostringstream& Log::get(const std::string& category,LogLevel level) {
	os << NowTime();
	os << " [" << toString(level) << "] -";
	os << " [" << category << "] : ";
	os << std::string(level > logDEBUG ? level - logDEBUG : 0, '\t');
	return os;
}

Log::~Log() {
    os << std::endl;
	//OutputDebugStringA(os.str().c_str());
	handler().write(os.str());
}

LogLevel& Log::reportingLevel() {
    static LogLevel reportingLevel = logDEBUG;
    return reportingLevel;
}

LogOutputHandler& Log::handler() {
	static FileOutputHandler coh;
	return coh;	
}

std::string Log::toString(LogLevel level) {
    static const char* const buffer[] = {"ERROR", "WARNING", "INFO", "DEBUG"};
    return buffer[level];
}

LogLevel Log::fromString(const std::string& level) {
    if (level == "DEBUG")
        return logDEBUG;
    if (level == "INFO")
        return logINFO;
    if (level == "WARNING")
        return logWARNING;
    if (level == "ERROR")
        return logERROR;
    Log().get(logWARNING) << "Unknown logging level '" << level << "'. Using INFO level as default.";
    return logINFO;
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
