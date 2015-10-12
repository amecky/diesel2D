#pragma once
#include <Vector.h>
#include <vector>
#include "..\renderer\BitmapFont.h"

namespace gui {

	void initialize();

	void start(v2* startPos);

	bool begin(const char* header, int* state);

	void Label(int id,const char* text);

	void beginGroup();

	void endGroup();

	bool Button(int id,const char* label);

	void CheckBox(int id, const char* label,bool* selected);

	void InputFloat(int id,const char* label, float* v);

	void Input(int id, const char* label, char* str,int maxLength);

	void InputFloat(int id, const char* label, float* v, float minValue, float maxValue, float step);

	void InputInt(int id, const char* label, int* v);

	void InputInt(int id, const char* label, int* v,int minValue, int maxValue, int step);

	void InputVec2(int id, const char* label, v2* v);

	void InputVec3(int id, const char* label, v3* v);

	void InputRect(int id, const char* label, ds::Rect* v);

	void InputColor(int id, const char* label, ds::Color* v);

	void ComboBox(int id,const std::vector<std::string>& entries, int* selected);

	void DropDownBox(int id, const std::vector<std::string>& entries, int* selected,int* state);

	void sendKey(unsigned char c);

	void sendSpecialKey(WPARAM wParam);

	void debugWindow();

	void end();
}
