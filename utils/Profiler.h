#pragma once
#define PROFILING TRUE
#include <Vector.h>

// -------------------------------------------------------
// Profile data
// -------------------------------------------------------
struct ProfileSnapshot {

	char name[32];
	int level;
	int invokeCounter;
	float totalTime;

};
// -------------------------------------------------------
// profiler
// -------------------------------------------------------
namespace profiler {

	void init();

	void reset();

	void start(const char* name);

	void end(const char* name);

	void finalize();

	void print();

	int get_total_times(float* values, int max);

	void showDialog(v2* position);

	int get_snapshot(ProfileSnapshot* items, int max);

	float get_current_total_time();
}

#define PRS(name) do { profiler::start(name); } while(0)
#define PRE(name) do { profiler::end(name); } while(0)

#ifdef PROFILING
#define PR_START(a) do { profiler::start(a); } while(0)
#define PR_END(a) do { profiler::end(a); } while(0)
#else
#define PR_START(a) 
#define PR_END(a) 
#endif

