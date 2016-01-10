#pragma once
#include "..\utils\JSONWriter.h"
#include "..\utils\PlainTextReader.h"
#include "..\utils\Profiler.h"
#include "..\utils\SimpleJSONReader.h"

namespace ds {

	class DataFile {

	public:
		DataFile() {}
		virtual ~DataFile() {}
		virtual bool saveData(JSONWriter& writer) = 0;
		virtual bool saveData(SimpleJSONWriter& writer) {
			return true;
		}
		virtual bool loadData(JSONReader& loader) = 0;
		virtual bool loadData(SimpleJSONReader& loader) {
			return true;
		}
		virtual const char* getFileName() const = 0;
		virtual bool useSimplified() {
			return false;
		}
		// -----------------------------------------------
		// export JSON
		// -----------------------------------------------
		bool save() {
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
		bool save_simplified() {
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
		bool load() {
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
		bool load_simplified() {
			bool ret = false;
			char buffer[64];
			// check length
			StopWatch s;
			s.start();
			sprintf_s(buffer, 64, "content\\%s", getFileName());
			LOG << "Reading simplified json file: " << buffer;
			SimpleJSONReader reader;
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

	};

}