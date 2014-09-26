#include "Profiler.h"
#include <windows.h>
#include "Log.h"
#include "StringUtils.h"

namespace profiler {

	const int MAX_PROFLING_ENTRIES = 256;
	// -------------------------------------------------------
	// Profile data
	// -------------------------------------------------------
	struct ProfileData {

		const char* name;
		int hash;
		int level;
		int invokeCounter;
		int parent;
		float totalTime;
		float subTime;
		LARGE_INTEGER startingTime;
	};

	// -------------------------------------------------------
	// Profile context
	// -------------------------------------------------------
	struct ProfileContext {

		ProfileData data[MAX_PROFLING_ENTRIES];
		int index;
		int current;
		LARGE_INTEGER frequency;
		float totalTime;
	};

	static ProfileContext ctx;

	// -------------------------------------------------------
	// initialize profiling system
	// -------------------------------------------------------
	void init() {
		ProfileData& pd = ctx.data[0];
		pd.name = "root";
		pd.level = 0;
		pd.parent = -1;
		pd.invokeCounter = 0;
		pd.totalTime = 0.0f;
		pd.subTime = 0.0f;
		QueryPerformanceFrequency(&ctx.frequency);
	}

	// -------------------------------------------------------
	// reset frame
	// -------------------------------------------------------
	void reset() {
		ctx.index = 1;
		ctx.current = 0;
		ctx.totalTime = 0;
		ctx.data[0].totalTime = 0;
		QueryPerformanceCounter(&ctx.data[0].startingTime);
	}

	// -------------------------------------------------------
	// build hash
	// -------------------------------------------------------
	int hash(const char *key) {
		int hash = 0;
		int len = strlen(key);
		for (int i = 0; i < len; ++i) {
			hash += key[i];
			hash += (hash << 10);
			hash ^= (hash >> 6);
		}
		hash += (hash << 3);
		hash ^= (hash >> 11);
		hash += (hash << 15);
		return hash;
	}

	// -------------------------------------------------------
	// find entry
	// -------------------------------------------------------
	int find(int hash) {
		for (int i = 0; i < ctx.index; ++i) {
			if (ctx.data[i].hash == hash) {
				return i;
			}
		}
		return -1;
	}

	// -------------------------------------------------------
	// start profiling sector
	// -------------------------------------------------------
	void start(const char* name) {
		int hashName = hash(name);
		int index = find(hashName);
		if (index == -1 && ctx.index < MAX_PROFLING_ENTRIES) {
			int idx = ctx.index++;
			ProfileData& pd = ctx.data[idx];
			pd.invokeCounter = 0;
			pd.totalTime = 0.0f;
			pd.subTime = 0.0f;
			pd.level = ctx.data[ctx.current].level + 1;
			pd.parent = ctx.current;
			pd.name = name;
			pd.hash = hashName;
			ctx.current = idx;
			QueryPerformanceCounter(&pd.startingTime);
		}
		else {
			ProfileData& pd = ctx.data[index];
			QueryPerformanceCounter(&pd.startingTime);
			++pd.invokeCounter;
		}
	}

	double LIToSecs(LARGE_INTEGER & L) {
		return ((double)L.QuadPart * 1000.0 / (double)ctx.frequency.QuadPart);
	}

	// -------------------------------------------------------
	// stop profiling sector
	// -------------------------------------------------------
	void end(const char* name) {
		int hashName = hash(name);
		int index = find(hashName);
		if (index != -1) {
			LARGE_INTEGER EndingTime;
			QueryPerformanceCounter(&EndingTime);
			LARGE_INTEGER time;
			time.QuadPart = EndingTime.QuadPart - ctx.data[index].startingTime.QuadPart;
			float elapsed = LIToSecs(time);
			ctx.totalTime += elapsed;
			ctx.data[index].totalTime += elapsed;
			ctx.data[index].invokeCounter += 1;
			ctx.current = ctx.data[index].parent;
		}
	}

	// -------------------------------------------------------
	// finalize profiling frame
	// -------------------------------------------------------
	void finalize() {
		LARGE_INTEGER EndingTime;
		QueryPerformanceCounter(&EndingTime);
		LARGE_INTEGER time;
		time.QuadPart = EndingTime.QuadPart - ctx.data[0].startingTime.QuadPart;
		float elapsed = LIToSecs(time);
		ctx.totalTime = elapsed;
		ctx.data[0].totalTime = elapsed;
	}

	// -------------------------------------------------------
	// format percentage
	// -------------------------------------------------------
	std::string formatPercentage(float percentage) {
		char buffer[20];
		sprintf(buffer, "%3.2f", percentage);
		std::string ret;
		if (percentage < 100.0f) {
			ret.append(" ");
		}
		if (percentage < 10.0f) {
			ret.append(" ");
		}
		ret.append(buffer);
		return ret;
	}

	// -------------------------------------------------------
	// Logs profiling data
	// -------------------------------------------------------
	void print() {
		LOG << "------------------------------------------------------------";
		LOG << " C  | Percent | Accu       | Name";
		LOG << "------------------------------------------------------------";
		float norm = ctx.data[0].totalTime;
		char buffer[256];
		std::string line;
		for (int i = 0; i < ctx.index; ++i) {
			ProfileData& pd = ctx.data[i];
			int ident = pd.level * 2;
			float per = pd.totalTime / norm * 100.0f;
			sprintf(buffer,"%3d | %s  | %3.8f | ", pd.invokeCounter, formatPercentage(per).c_str(), pd.totalTime);
			line = buffer;
			for (int j = 0; j < ident; ++j) {
				line += " ";
			}
			LOG << line << " " << pd.name;
		}
		LOG << "------------------------------------------------------------";
	}
}