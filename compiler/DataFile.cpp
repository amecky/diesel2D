#pragma once
#include "DataFile.h"
#include "..\utils\PlainTextReader.h"
#include "..\utils\Profiler.h"
#include "..\utils\SimpleJSONReader.h"

namespace ds {

	// -----------------------------------------------
	// export JSON
	// -----------------------------------------------
	bool DataFile::save() {
		if (useSimplified()) {
			return save_simplified();
		}
		else {
			char buffer[64];
			sprintf_s(buffer, 64, "content\\%s", getFileName());
			LOG << "exporting json file: " << buffer;
			// FIXME: check if directory exists
			JSONWriter jw;
			if (jw.open(buffer)) {
				return saveData(jw);
			}
			else {
				LOG << "Cannot open file";
				return false;
			}
		}
	}

	// -----------------------------------------------
	// export JSON
	// -----------------------------------------------
	bool DataFile::save_simplified() {
		char buffer[64];
		sprintf_s(buffer, 64, "content\\%s", getFileName());
		LOG << "exporting simplified json file: " << buffer;
		// FIXME: check if directory exists
		SimpleJSONWriter jw;
		if (jw.open(buffer)) {
			return saveData(jw);
		}
		else {
			LOG << "Cannot open file";
			return false;
		}
	}

	// -----------------------------------------------
	// import JSON
	// -----------------------------------------------
	bool DataFile::load() {
		if (useSimplified()) {
			return load_simplified();
		}
		else {
			bool ret = false;
			char buffer[64];
			// check length
			StopWatch s;
			s.start();
			sprintf_s(buffer, 64, "content\\%s", getFileName());
			LOG << "Reading json file: " << buffer;
			JSONReader reader;
			if (reader.parse(buffer)) {
				ret = loadData(reader);
			}
			else {
				LOG << "Error: Cannot parse file: " << buffer;
				ret = false;
			}
			s.end();
			LOG << "----> elapsed: " << s.elapsed();
			return ret;
		}
	}

	// -----------------------------------------------
	// import JSON
	// -----------------------------------------------
	bool DataFile::load_simplified() {
		bool BINARY = true;
		bool ret = false;
		char buffer[64];
		// check length
		StopWatch s;
		s.start();
		sprintf_s(buffer, 64, "content\\%s", getFileName());
		LOG << "Reading simplified json file: " << buffer;
		SimpleJSONReader reader;
		if (BINARY) {
			sprintf_s(buffer, 64, "assets\\%u", string::murmur_hash(getFileName()));
			if (reader.load_binary(buffer)) {
				ret = loadData(reader);
			}
		}
		else {
			if (reader.parse(buffer)) {
				ret = loadData(reader);
			}
			if (ret) {
				sprintf_s(buffer, 64, "assets\\%u", string::murmur_hash(getFileName()));
				reader.save_binary(buffer);
			}
		}
		if (!ret) {
			LOG << "Error: Cannot parse file: " << buffer;
			ret = false;
		}		
		s.end();
		LOG << "----> elapsed: " << s.elapsed();
		return ret;
	}


}