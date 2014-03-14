#pragma once
#define PROFILING TRUE
//#include <vector>
#include <windows.h>
#include "..\lib\container\List.h"
#include "..\dxstdafx.h"
#include <vector>
#include "..\renderer\Renderer.h"

const int HISTORY_TICKS = 60;
const int MAX_PROFILE_DATA = 100;

struct DataMeasurement {
	IdString hash;
	char name[32];
	LARGE_INTEGER started;
	float value;
};

class NewProfiler {

struct ProfileFrame {
	bool valid;
	int beginCalls;
	int openCalls;
	char name[64];
	IdString hash;
	LARGE_INTEGER started;
	float accumulator;
	float childAccu;
	int level;
};

struct ProfileHistory {
	IdString hash;
	char name[64];
	int level;
	float average;
	float min;
	float max;
	long calls;
};

struct ProfileData {
	IdString hash;
	const char* name;
	int counter;
	LARGE_INTEGER started;
	double data[MAX_PROFILE_DATA];
};

//typedef List<ProfileFrame> Frames;

public:
	NewProfiler();
	~NewProfiler();
	void start(const char* name);
	void stop(const char* name);
	void reset();
	void print();
	void printCollectedData(const char* name);
	void show(int x,int y,ds::Renderer* renderer);
	// profiling data
	void startCollectData(const char* name);
	void stopCollectData(const char* name);
private:
	double LIToSecs( LARGE_INTEGER & L);
	std::string formatPercentage(float percentage);
	std::string formatDuration(float percentage);
	std::string formatName(ProfileFrame* frame);
	void updateHistory(ProfileFrame* frame);
	//Frames m_Frames;
	//List<ProfileFrame> m_Frames;
	std::vector<ProfileFrame> m_Frames;
	ds::List<ProfileHistory> m_History;
	ds::List<ProfileData> m_ProfileData;
	LARGE_INTEGER m_Frequency;
	int m_Ticks;
};

extern NewProfiler* gProfiler;
#ifdef PROFILING
#define PR_START(a) gProfiler->start(a);
#define PR_END(a) gProfiler->stop(a);
#define PR_COLLECT_START(a) gProfiler->startCollectData(a);
#define PR_COLLECT_END(a) gProfiler->stopCollectData(a);
#else
#define PR_START(a) 
#define PR_END(a) 
#define PR_COLLECT_START(a)
#define PR_COLLECT_END(a)
#endif

