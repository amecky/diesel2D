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
    std::ostringstream& get();
	std::ostringstream& error();
	std::ostringstream& get(const std::string& category);    
	std::ostringstream& get(const char *file, const unsigned long line);    
	std::ostringstream& error(const std::string& category);   
	static LogOutputHandler& handler();        
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

#define LOG Log().get()
#define LOGE Log().error()

#define LOGC(category) Log().get(category)
#define LOGEC(category) Log().error(category)

#define ELOG Log().get(__FILE__,__LINE__)

#endif	/* LOG_H */

