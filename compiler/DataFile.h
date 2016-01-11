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
		bool save();		
		bool load();
	private:
		bool save_simplified();
		bool load_simplified();

	};

}