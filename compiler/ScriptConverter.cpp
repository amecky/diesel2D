#include "ScriptConverter.h"
#include "..\utils\PlainTextReader.h"
#include "..\io\BinaryWriter.h"
#include "..\utils\FileUtils.h"

namespace ds {

ScriptConverter::ScriptConverter() : Converter("ScriptConverter") {
}


ScriptConverter::~ScriptConverter(void) {
}

void ScriptConverter::convert(JSONReader& reader,BinaryWriter& writer) {
}

}
