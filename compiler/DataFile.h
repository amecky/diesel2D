#pragma once
#include "..\utils\JSONWriter.h"
#include "..\utils\PlainTextReader.h"
#include "..\io\BinaryWriter.h"
#include "..\io\BinaryLoader.h"

namespace ds {

	class DataFile {

	public:
		DataFile() {}
		virtual ~DataFile() {}
		virtual bool saveData(JSONWriter& writer) = 0;
		virtual bool loadData(JSONReader& loader) = 0;
		virtual const char* getFileName() const = 0;

		// -----------------------------------------------
		// export JSON
		// -----------------------------------------------
		bool save() {
			char buffer[64];
			sprintf_s(buffer, 64,"content\\%s", getFileName());
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

		// -----------------------------------------------
		// import JSON
		// -----------------------------------------------
		bool load() {
			char buffer[64];
			// check length
			sprintf_s(buffer, 64, "content\\%s", getFileName());
			LOG << "Reading json file: " << buffer;
			JSONReader reader;
			if (reader.parse(buffer)) {
				return loadData(reader);
			}
			else {
				LOG << "Error: Cannot parse file: " << buffer;
				return false;
			}
		}

	};

}