#pragma once
#define PROFILING TRUE
//#include <windows.h>
//#include "..\dxstdafx.h"

namespace profiler {

	void init();

	void reset();

	void start(const char* name);

	void end(const char* name);

	void finalize();

	void print();

	int get_total_times(float* values, int max);
}

#define PRS(name) profiler::start(name);
#define PRE(name) profiler::end(name);

#ifdef PROFILING
#define PR_START(a) profiler::start(a);
#define PR_END(a) profiler::end(a);
#else
#define PR_START(a) 
#define PR_END(a) 
#endif

