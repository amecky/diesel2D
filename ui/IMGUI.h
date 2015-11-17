#pragma once
#include <Vector.h>
#include <vector>
#include "..\renderer\BitmapFont.h"
#include "..\renderer\render_types.h"

namespace gui {
	
	class AbstractComponentModel {

	public:
		AbstractComponentModel() : _selected(-1) {}
		bool hasSelection() const {
			return _selected != -1;
		}
		void select(int idx) {
			_selected = idx;
		}
		bool isSelected(int idx) const {
			return idx == _selected;
		}
		int getSelection() const {
			return _selected;
		}
		virtual size_t size() const = 0;
		virtual const char* getLabel(int idx) const = 0;
		virtual void clear() = 0;
		virtual void remove(int idx) = 0;
	protected:
		int _selected;
	};

	template<class T>
	class ComponentModel : public AbstractComponentModel {

		struct ModelEntry {
			char name[32];
			T value;
		};

		typedef std::vector<ModelEntry> Entries;

	public:
		ComponentModel() : AbstractComponentModel() {}
		void add(const char* label, const T& t) {
			ModelEntry entry;
			strcpy_s(entry.name, 32, label);
			entry.value = t;
			_entries.push_back(entry);
		}
		size_t size() const {
			return _entries.size();
		}
		const char* getLabel(int idx) const {
			return _entries[idx].name;
		}
		const T& getValue(int idx) const {
			return _entries[idx].value;
		}
		const T& getSelectedValue() const {
			assert(hasSelection());
			return _entries[_selected].value;
		}
		void clear() {
			_entries.clear();
			_selected = -1;
		}
		void remove(int idx) {
			_entries.erase(_entries.begin() + idx);
			_selected = -1;
		}
		void swap(int currentIndex, int newIndex) {
			if (currentIndex >= 0 && currentIndex < size() && newIndex >= 0 && newIndex < size()) {
				ModelEntry current = _entries[currentIndex];
				ModelEntry next = _entries[newIndex];
				_entries[newIndex] = current;
				_entries[currentIndex] = next;
			}
		}
	private:
		Entries _entries;
		
	};

	enum GUIDisplayMode {
		GDM_EDITOR,
		GDM_FLOATING
	};

	void initialize(bool editorMode = false);

	void switchEditorMode(bool editorMode);

	void start(int id,v2* startPos);

	bool begin(const char* header, int* state);

	void beginModal(const char* header);

	void Label(int id,const char* text);

	void beginGroup();

	void endGroup();

	bool Button(int id,const char* label);

	void BoxTest();

	void CheckBox(int id, const char* label,bool* selected);

	void InputFloat(int id,const char* label, float* v);

	void Input(int id, const char* label, char* str,int maxLength);

	void InputFloat(int id, const char* label, float* v, float minValue, float maxValue, float step);

	void InputInt(int id, const char* label, int* v);

	void InputInt(int id, const char* label, uint32* v);

	void InputInt(int id, const char* label, int* v,int minValue, int maxValue, int step);

	void InputVec2(int id, const char* label, v2* v);

	void InputVec3(int id, const char* label, v3* v);

	void InputRect(int id, const char* label, ds::Rect* v);

	void InputColor(int id, const char* label, ds::Color* v);

	void ComboBox(int id,const std::vector<std::string>& entries, int* selected,int *offset,int max = 5);

	void ComboBox(int id, AbstractComponentModel* model,int *offset,int max = 5);

	void DropDownBox(int id, const std::vector<std::string>& entries, int* selected,int* state);

	void Histogram(int id, float* values, int num, float minValue, float maxValue, float step);

	void sendKey(unsigned char c);

	void sendSpecialKey(WPARAM wParam);

	void debugWindow();

	void end();

	void shutdown();

	void endFrame();

	//int InputDialog(const char* message, const char* label, char* text);

	class InputDialog {

	public:
		InputDialog();
		~InputDialog() {}
		int show(const char* header,const char* label);
		const char* getText() const;
	private:
		bool _active;
		char _text[32];
		int _button;
	};

}
