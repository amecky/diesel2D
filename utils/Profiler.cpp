#include "Profiler.h"
#include <winbase.h>
#include "Log.h"
#include "StringUtils.h"

NewProfiler* gProfiler;

NewProfiler::NewProfiler() : m_Ticks(0) {
	LOGC("Profiler") << "creating new profiler";
	QueryPerformanceFrequency(&m_Frequency);
	//m_Frames.assureSize(20);
}


NewProfiler::~NewProfiler(void) {
}

void NewProfiler::start(const char* name) {
	bool found = false;
	IdString hash = ds::string::murmur_hash(name,strlen(name),0);
	//for ( size_t i = 0; i < m_Frames.num(); ++i ) {
	for ( size_t i = 0; i < m_Frames.size(); ++i ) {
		ProfileFrame* frame = &m_Frames[i];
		if ( hash == frame->hash ) {
			// found frame
			found = true;
			++frame->openCalls;
			++frame->beginCalls;
			QueryPerformanceCounter(&frame->started);
			frame->valid = true;
		}
	}
	if ( !found ) {
		ProfileFrame frame;
		frame.accumulator = 0.0f;
		frame.beginCalls = 1;
		frame.childAccu = 0.0f;
		frame.level = 0;
		frame.hash = hash;
		strncpy(frame.name,name,64);
		frame.openCalls = 1;
		QueryPerformanceCounter(&frame.started);
		frame.valid = true;
		//m_Frames.append(frame);
		m_Frames.push_back(frame);
	}
}

void NewProfiler::stop(const char* name) {
	bool found = false;
	//for ( size_t i = 0; i < m_Frames.num(); ++i ) {
	for ( size_t i = 0; i < m_Frames.size(); ++i ) {
		ProfileFrame* frame = &m_Frames[i];
		if ( strcmp(frame->name,name) == 0 ) {
			LARGE_INTEGER stop;
			QueryPerformanceCounter(&stop);
			// found frame
			found = true;
			--frame->openCalls;
			int parent = -1;
			int numParents = 0;
			//for ( size_t j = 0; j < m_Frames.num(); ++j ) {
			for ( size_t j = 0; j < m_Frames.size(); ++j ) {
				ProfileFrame* pf = &m_Frames[j];
				if ( pf->valid ) {
					if ( pf->openCalls > 0 ) {
						++numParents;
					}
					if ( parent < 0 ) {
						parent = j;
					}
					if ( m_Frames[j].started.QuadPart >= m_Frames[parent].started.QuadPart  ) {
						parent = j;
					}
				}
			}
			frame->level = numParents;
			if ( parent >= 0 ) {
				LARGE_INTEGER time;
				time.QuadPart = stop.QuadPart - frame->started.QuadPart;
				m_Frames[parent].childAccu += LIToSecs(time);//getTime() - frame->started;
			}
			
			LARGE_INTEGER time;
			time.QuadPart = stop.QuadPart - frame->started.QuadPart;    
			frame->accumulator +=  LIToSecs(time);//getTime() - frame->started;
			updateHistory(frame);
		}
	}
}

void NewProfiler::updateHistory(ProfileFrame* frame) {
	bool found = false;
	for ( uint32 i = 0; i < m_History.num(); ++i ) {
		ProfileHistory* history = &m_History[i];
		if ( history->hash == frame->hash ) {
			found = true;
			if ( m_Ticks == 0 ) {
				if ( frame->accumulator < history->min ) {
					history->min = frame->accumulator;
				}
				if ( frame->accumulator > history->max ) {
					history->max = frame->accumulator;
				}
				history->average = (history->average + frame->accumulator) * 0.5f;
			}
		}
	}
	if ( !found ) {
		ProfileHistory ph;
		ph.average = frame->accumulator;
		ph.calls = 0;
		ph.hash = frame->hash;
		strncpy(ph.name,frame->name,64);
		ph.max = frame->accumulator;
		ph.min = frame->accumulator;
		ph.level = frame->level;
		m_History.append(ph);
	}
}

double NewProfiler::LIToSecs( LARGE_INTEGER & L) {
	return ((double)L.QuadPart * 1000.0 /(double)m_Frequency.QuadPart) ;
	//return ((double)L.QuadPart /(double)m_Frequency.QuadPart) ;
}

void NewProfiler::reset() {
	//for ( size_t i = 0; i < m_Frames.num(); ++i ) {
	/*
	for ( size_t i = 0; i < m_Frames.size(); ++i ) {
		ProfileFrame* frame = &m_Frames[i];
		frame->valid = false;
		frame->beginCalls = 0;
		frame->openCalls = 0;
		frame->accumulator = 0.0f;
	}
	*/
	++m_Ticks;
	if ( m_Ticks > HISTORY_TICKS ) {
		m_Ticks = 0;
	}
	m_Frames.clear();
}

void NewProfiler::print() {
	float total = 0.0f;
	LOG << "Profiler";
	LOG << "-------------------------------------------------";
	for ( size_t i = 0; i < m_Frames.size(); ++i ) {
		ProfileFrame* frame = &m_Frames[i];
		if ( frame->valid && frame->level == 0 ) {
			//total += frame->accumulator;
			total = frame->accumulator;
		}
	}
	LOG << "Percent | C | Accu      | Frame";		
	for ( size_t i = 0; i < m_Frames.size(); ++i ) {
		ProfileFrame* frame = &m_Frames[i];		
		if ( frame->beginCalls > 0 ) {
			float percentage = frame->accumulator / total * 100.0f;
			LOG << formatPercentage(percentage) << "% | " << frame->beginCalls << " | " << formatDuration(frame->accumulator) << " | " << formatName(frame);	
		}
	}
	/*
	LOGC("NewProfiler") << "---- history -------";
	for ( uint32 i = 0; i < m_History.num(); ++i ) {
		ProfileHistory* ph = &m_History[i];
		if ( ph->level == 0 ) {
			total = ph->average;
		}
	}
	LOGC("NewProfiler") << "Percentage | Average | Min | Max | Name";			
	for ( uint32 i = 0; i < m_History.num(); ++i ) {
		ProfileHistory* ph = &m_History[i];
		float percentage = ph->average / total * 100.0f;
		LOGC("NewProfiler") << formatPercentage(percentage) << "% | " << formatDuration(ph->average) << " | " << formatDuration(ph->min) << " | " << formatDuration(ph->max) << " | " << ph->name;			
	}
	*/
}

void NewProfiler::show(int x,int y,ds::Renderer* renderer) {
	float total = 1.0f;
	for ( uint32 i = 0; i < m_History.num(); ++i ) {
		ProfileHistory* ph = &m_History[i];
		if ( ph->level == 0 ) {
			total = ph->average;
		}
	}
	int tx = x;
	int ty = y;
	for ( uint32 i = 0; i < m_History.num(); ++i ) {
		ProfileHistory* ph = &m_History[i];
		float percentage = ph->average / total * 100.0f;	
		tx = x;		
		renderer->debug(tx,ty,ds::Color(1.0f,1.0f,1.0f,1.0f),"%s",formatDuration(ph->average).c_str());
		tx = tx +70.0f + ph->level * 5.0f;
		renderer->debug(tx,ty,ph->name);
		ty += 20.0f;
		//LOGC("NewProfiler") << formatPercentage(percentage) << "% | " << formatDuration(ph->average) << " | " << formatDuration(ph->min) << " | " << formatDuration(ph->max) << " | " << ph->name;			
	}
}

std::string NewProfiler::formatName(ProfileFrame* frame) {
	int ident = 0;
	std::string ret;
	if ( frame->level > 0 ) {
		ident = frame->level * 2;
	}
	for ( int i = 0; i < ident; ++i ) {
		ret.append(" ");
	}
	ret.append(frame->name);
	return ret;
}

std::string NewProfiler::formatDuration(float percentage) {
	char buffer[20];
	sprintf(buffer,"%2.6f",percentage);
	std::string ret;
	if ( percentage < 10.0f ) {
		ret.append(" ");
	}
	ret.append(buffer);
	return ret;
}


std::string NewProfiler::formatPercentage(float percentage) {
	char buffer[20];
	sprintf(buffer,"%3.2f",percentage);
	std::string ret;
	if ( percentage < 100.0f ) {
		ret.append(" ");
	}
	if ( percentage < 10.0f ) {
		ret.append(" ");
	}
	ret.append(buffer);
	return ret;
}

// -------------------------------------------------------
// Start collecting data
// -------------------------------------------------------
void NewProfiler::startCollectData(const char* name) {
	IdString hash = ds::string::murmur_hash(name);
	bool found = false;
	for ( uint32 i = 0; i < m_ProfileData.num(); ++i ) {
		ProfileData* data = &m_ProfileData[i];
		if ( data->hash == hash ) {
			found = true;
			QueryPerformanceCounter(&data->started);
		}
	}
	if ( !found ) {
		ProfileData data;
		data.counter = 0;
		data.hash = hash;
		QueryPerformanceCounter(&data.started);
		m_ProfileData.append(data);
	}
}

// -------------------------------------------------------
// Stops collection data
// -------------------------------------------------------
void NewProfiler::stopCollectData(const char* name) {
	IdString hash = ds::string::murmur_hash(name);	
	for ( uint32 i = 0; i < m_ProfileData.num(); ++i ) {
		ProfileData* data = &m_ProfileData[i];
		if ( data->hash == hash ) {
			LARGE_INTEGER stop;
			QueryPerformanceCounter(&stop);
			LARGE_INTEGER time;
			time.QuadPart = stop.QuadPart - data->started.QuadPart;
			data->data[data->counter] = LIToSecs(time);
			++data->counter;
			if ( data->counter > MAX_PROFILE_DATA ) {
				data->counter = 0;
			}
		}
	}
}
// -------------------------------------------------------
// Prints the collected data for given name
// -------------------------------------------------------
void NewProfiler::printCollectedData(const char* name) {
	IdString hash = ds::string::murmur_hash(name);	
	float total = 0.0f;
	for ( uint32 i = 0; i < m_ProfileData.num(); ++i ) {
		ProfileData* data = &m_ProfileData[i];
		if ( data->hash == hash ) {
			LOG << "---- Collected data for " << name << " - entries " << data->counter;
			int start = data->counter - 10;
			if ( start < 0 ) {
				start = 0;
			}
			for ( int i = start; i < data->counter; ++i ) {
				LOG << i << " : " << data->data[i] * 1000.0;
				total += data->data[i];
			}
		}
	}
	float average = total / 10.0f;
	LOG << "Average: " << average;
}