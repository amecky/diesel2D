#pragma once
#include <Vector.h>
#include <vector>
#include "..\renderer\BitmapFont.h"

namespace gui {

	void initialize();

	bool begin(const char* header,int* state,const v2& startPos = v2(10,750));

	void Label(const char* text);

	void beginGroup();

	void endGroup();

	bool Button(int id,const char* label);

	void InputFloat(int id,const char* label, float* v);

	void InputInt(int id, const char* label, int* v);

	void InputVec2(int id, const char* label, v2* v);

	void InputVec3(int id, const char* label, v3* v);

	void InputRect(int id, const char* label, ds::Rect* v);

	void ComboBox(int id,const std::vector<std::string>& entries, int* selected);

	void DropDownBox(int id, const std::vector<std::string>& entries, int* selected,int* state);

	void sendKey(unsigned char c);

	void end();
}
