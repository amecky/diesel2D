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

		virtual bool exportData(JSONWriter& writer) = 0;
		virtual bool importData(JSONReader& reader) = 0;
		virtual bool saveData(BinaryWriter& writer) = 0;
		virtual bool loadData(BinaryLoader& loader) = 0;
		virtual const char* getJSONFileName() const = 0;
		virtual const char* getFileName() const = 0;

		// -----------------------------------------------
		// load
		// -----------------------------------------------
		bool load() {
			BinaryLoader loader;
			char buffer[64];
			sprintf(buffer, "assets\\%u", string::murmur_hash(getFileName()));
			LOGC("DataFile") << "loading file: " << buffer;
			int signature[] = { 0, 8, 15 };
			if (loader.open(buffer, signature, 3) == IO_OK) {
				return loadData(loader);
			}
			else {
				LOGC("DataFile") << "Cannot find file";
				return false;
			}
		}

		// -----------------------------------------------
		// save
		// -----------------------------------------------
		bool save() {
			char buffer[64];
			sprintf(buffer, "assets\\%u", string::murmur_hash(getFileName()));
			LOGC("DataFile") << "saving file: " << buffer;
			BinaryWriter writer;
			int signature[] = { 0, 8, 15 };
			if (writer.open(buffer, signature, 3) == IO_OK) {
				bool ret = saveData(writer);
				writer.close();
				return ret;
			}
			else {
				LOGC("DataFile") << "Cannot open file";
				return false;
			}
		}

		// -----------------------------------------------
		// export JSON
		// -----------------------------------------------
		bool exportJSON() {
			char buffer[64];
			sprintf(buffer, "content\\%s", getJSONFileName());
			LOGC("DataFile") << "exporting json file: " << buffer;
			// FIXME: check if directory exists
			JSONWriter jw;
			if (jw.open(buffer)) {
				return exportData(jw);
			}
			else {
				LOGC("DataFile") << "Cannot open file";
				return false;
			}
		}

		// -----------------------------------------------
		// import JSON
		// -----------------------------------------------
		bool importJSON() {
			char buffer[64];
			// check length
			sprintf_s(buffer, 64, "content\\%s", getJSONFileName());
			LOGC("DataFile") << "Reading json file: " << buffer;
			JSONReader reader;
			if (reader.parse(buffer)) {
				return importData(reader);
			}
			else {
				LOGC("DataFile") << "Error: Cannot parse file: " << buffer;
				return false;
			}
		}

	};

}