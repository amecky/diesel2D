/* 
 * File:   Log.h
 * Author: mecky
 *
 * Created on 3. November 2010, 08:05
 */

#ifndef LOG_H
#define	LOG_H

#include <sstream>
#include <string>
#include <stdio.h>

enum LogLevel {logERROR, logWARNING, logINFO, logDEBUG};

class LogOutputHandler {

public:
	virtual void write(const std::string& message) = 0;
};

class ConsoleOutputHandler : public LogOutputHandler {

public:
	void write(const std::string& message);
};

class FileOutputHandler : public LogOutputHandler {

public:
   static FILE*& Stream();
   void write(const std::string& msg);

};

class Log {
    
public:
    Log();    
    virtual ~Log();
    std::ostringstream& get(LogLevel level = logINFO);
	std::ostringstream& get(const std::string& category,LogLevel level = logINFO);
    static LogLevel& reportingLevel();
	static LogOutputHandler& handler();
    static std::string toString(LogLevel level);
    static LogLevel fromString(const std::string& level);
protected:
    std::ostringstream os;		
private:
    std::string NowTime();
    Log(const Log&);
    Log& operator =(const Log&);	
};

#ifndef FILELOG_MAX_LEVEL
#define FILELOG_MAX_LEVEL logDEBUG
#endif

#define LG \
	Log().get()

#define LOG(level) \
    if (level > FILELOG_MAX_LEVEL) ;\
    else if (level > Log::reportingLevel() ) ; \
    else Log().get(level)

#define LOGC(level,category) \
	if (level > FILELOG_MAX_LEVEL) ;\
	else if (level > Log::reportingLevel() ) ; \
	else Log().get(category,level)

#endif	/* LOG_H */

