#include "CrashReporter.h" 
#include "..\memory\CallStackTracer.h"

namespace ds {

	CrashDump* gCrashDump;

	void CrashDump::attach(CrashReporter* reporter) {
		_reporter = reporter;
	}

	void CrashDump::dump() {
		_reporter->saveReport();
		CallStackTracer tracer;
		stacktrace::logStackTrace();
		
	}

	void CrashDump::exit() {
		_reporter->shutdown();
	}

}