#include "IMGUI.h"
#include "..\renderer\graphics.h"
#include "..\sprites\SpriteBatch.h"
#include "..\utils\font.h"

namespace gui {

	const float BUTTON_HEIGHT = 24.0f;
	const float TEXTFIELD_HEIGHT = 16.0f;
	const float TEXT_PADDING = 4.0f;
	const float BUTTON_PADDING = 20.0f;
	const float LINE_HEIGHT = 26.0f;
	const float TEXT_OFFSET = 7.0f;
	const int CHAR_PADDING = 0;
	const float BOX_HEIGHT = 16.0f;
	const float WHITE_BOX_SIZE = 256.0f;
	const float INPUT_BOX_WIDTH = 70.0f;
	// -------------------------------------------------------
	// DrawCall
	// -------------------------------------------------------
	struct DrawCall {

		int type;
		char text[32];
		ds::Color color;
		v2 size;
		v2 position;
		ds::Texture texture;
		int padding;
	};

	const int MAX_DRAW_CALLS = 256;
	// -------------------------------------------------------
	// GUIWindow
	// -------------------------------------------------------
	struct GUIWindow {

		DrawCall calls[MAX_DRAW_CALLS];
		int num;

		void reset() {
			num = 0;
		}

		void addBox(const v2& position,const v2& size, const ds::Color& color) {
			if (num < MAX_DRAW_CALLS) {
				DrawCall& call = calls[num++];
				call.type = 1;
				call.color = color;
				call.size = size;
				call.position = position;
				call.padding = 2;
			}
		}

		void addImage(const v2& position, const ds::Texture& texture) {
			if (num < MAX_DRAW_CALLS) {
				DrawCall& call = calls[num++];
				call.type = 3;
				call.color = ds::Color::WHITE;
				call.size = v2(1, 1);
				call.position = position;
				call.texture = texture;
				call.padding = 1;
			}
		}

		void addText(const v2& position,const char* text, const v2& size) {
			if (num < MAX_DRAW_CALLS) {
				DrawCall& call = calls[num++];
				call.type = 2;
				call.color = ds::Color::WHITE;
				call.size = size;
				sprintf_s(call.text, 32, text);
				call.position = position;
				call.position.y -= TEXT_OFFSET;
				call.padding = CHAR_PADDING;
			}
		}

		
	};

	// -------------------------------------------------------
	// KeyInput
	// -------------------------------------------------------
	struct KeyInput {
		unsigned char keys[256];
		int num;
	};

	enum GUIDataType {
		DT_INT,
		DT_FLOAT
	};

	enum GUIIcon {
		ICN_ARROW_LEFT,
		ICN_ARROW_DOWN,
		ICN_ARROW_RIGHT,
		ICN_ARROW_UP,
		ICN_PLUS,
		ICN_MINUS,
		ICN_CHECKBOX,
		ICN_CHECKBOX_SELECTED
	};

	enum GUIColor {
		CLR_PANEL_BACKGROUND,
		CLR_PANEL_HEADER,
		CLR_INPUT_EDIT,
		CLR_INPUT,
		CLR_BUTTON,
		CLR_BUTTON_HOVER
	};

	// -------------------------------------------------------
	// GUI context
	// -------------------------------------------------------
	struct GUIContext {

		ds::BitmapFont* font;
		int textureID;
		GUIWindow window;
		v2 cursorPosition;
		bool buttonPressed;
		bool clicked;
		bool released;
		int hot;
		int active;
		bool grouped;
		char inputText[32];
		KeyInput keyInput;
		bool visible;
		int caretPos;
		v2 position;
		const char* header;
		v2 dim;
		v2 startPosition;
		ds::Texture textures[16];
		ds::Color colors[16];
		bool ready;

		GUIContext() {
			textureID = -1;
			ready = false;
			keyInput.num = 0;
		}

		void reset() {
			cursorPosition = ds::renderer::getMousePosition();
			//clicked = false;
			grouped = false;
			visible = false;
			dim = v2(0, 0);
			window.reset();
			hot = -1;
		}

		void calculateDimension(const v2& position, const v2& size) {
			float endX = position.x + size.x - startPosition.x;
			float endY = startPosition.y - position.y - size.y;
			if (endX > dim.x) {
				dim.x = endX;
			}
			if (endY > dim.y) {
				dim.y = endY;
			}
		}

		void addBox(const v2& position, const v2& size, const ds::Color& color) {
			v2 p = position;
			p.x += size.x * 0.5f;
			window.addBox(p, size, color);
			calculateDimension(p, size);

		}

		void addImage(const v2& position, const ds::Texture& texture,float offsetX = 0.0f,float offsetY = 0.0f) {
			v2 p = position;
			p.x += offsetX;
			p.y += offsetY;
			window.addImage(p, texture);
			calculateDimension(p, texture.dim);
		}

		void addText(const v2& position, const char* text) {
			v2 size = ds::font::calculateSize(*font, text, CHAR_PADDING);
			v2 p = position;
			p.x += TEXT_PADDING;
			addText(p, text, size);
		}

		void addText(const v2& position, const char* text, const v2& size) {
			window.addText(position, text, size);
			calculateDimension(position, size);
		}

		void nextPosition() {
			if (!grouped) {
				position.y -= LINE_HEIGHT;
				position.x = startPosition.x;
			}
			else {
				position.x += window.calls[window.num - 1].size.x + 50.0f;
				
			}
		}
	};

	static GUIContext* guiContext;

	// -------------------------------------------------------
	// send key
	// -------------------------------------------------------
	void sendKey(unsigned char c) {
		if (guiContext->ready) {
			if (guiContext->keyInput.num < 256) {
				guiContext->keyInput.keys[guiContext->keyInput.num++] = c;
			}
			else {
				guiContext->keyInput.num = 0;
			}
		}
	}
	
	// -------------------------------------------------------
	// send special key
	// -------------------------------------------------------
	void sendSpecialKey(WPARAM wParam) {
		if (wParam == VK_BACK) {
			gui::sendKey(128);
		}
		if (wParam == VK_LEFT) {
			gui::sendKey(129);
		}
		if (wParam == VK_RIGHT) {
			gui::sendKey(130);
		}
		if (wParam == VK_HOME) {
			gui::sendKey(131);
		}
		if (wParam == VK_END) {
			gui::sendKey(132);
		}
		if (wParam == VK_RETURN) {
			gui::sendKey(133);
		}
	}
	// -------------------------------------------------------
	// check if mouse cursor is inside box
	// -------------------------------------------------------
	bool isCursorInside(const v2& p, const v2& dim) {
		v2 mp = guiContext->cursorPosition;
		if (mp.x < (p.x - dim.x * 0.5f)) {
			return false;
		}
		if (mp.x > (p.x + dim.x * 0.5f)) {
			return false;
		}
		if (mp.y < (p.y - dim.y * 0.5f)) {
			return false;
		}
		if (mp.y > (p.y + dim.y * 0.5f)) {
			return false;
		}
		return true;
	}

	ds::Texture buildBoxTexture(const v2& size) {
		return ds::math::buildTexture(200.0f, 0.0f, size.x, size.y, 512.0f, 512.0f);
	}

	ds::Texture buildBoxTexture(float dimX,float dimY) {
		return ds::math::buildTexture(200.0f, 0.0f, dimX, dimY, 512.0f, 512.0f);
	}

	// -------------------------------------------------------
	// begin group
	// -------------------------------------------------------
	void beginGroup() {
		guiContext->grouped = true;
	}

	// -------------------------------------------------------
	// end group
	// -------------------------------------------------------
	void endGroup() {
		guiContext->grouped = false;
		guiContext->nextPosition();
	}

	// -------------------------------------------------------
	// check if widget is hot
	// -------------------------------------------------------
	bool isHot(int id, const v2& pos, const v2& size,float offsetX = 0.0f,float offsetY = 0.0f) {
		v2 p = pos;
		p.x += offsetX;
		p.y += offsetY;		
		if (isCursorInside(p, size)) {
			guiContext->hot = id;
			return true;
		}
		return false;
	}

	// -------------------------------------------------------
	// handle mouse interaction
	// -------------------------------------------------------
	bool isSelected(int id, const v2& pos, const v2& size,bool setActive = true) {
		if (guiContext->clicked && isCursorInside(pos, size)) {
			if (setActive) {
				guiContext->active = id;
			}
			return true;
		}		
		return false;
	}

	// -------------------------------------------------------
	// is box selected
	// -------------------------------------------------------
	bool isBoxSelected(int id, const v2& pos, const v2& size,bool setActive = true) {
		if (guiContext->clicked) {
			v2 p = pos;
			p.x += size.x * 0.5f;
			if (guiContext->clicked && isCursorInside(p, size)) {
				if (setActive) {
					guiContext->active = id;
					guiContext->clicked = false;
				}
				return true;
			}
		}
		return false;
	}

	// -------------------------------------------------------
	// start
	// -------------------------------------------------------
	void start(const v2& startPos) {
		assert(guiContext->ready);		
		if (guiContext->hot == -1 && guiContext->clicked) {
			guiContext->active = -1;
		}
		if (guiContext->clicked) {
			guiContext->clicked = false;
		}
		guiContext->hot = -1;
		guiContext->startPosition = startPos;
		guiContext->position = startPos;		
		if ((GetKeyState(VK_LBUTTON) & 0x80) != 0) {
			guiContext->buttonPressed = true;
		}
		else {
			if (guiContext->buttonPressed) {
				LOG << "clicked on ";
				guiContext->clicked = true;
			}
			guiContext->buttonPressed = false;
		}
	}
	// -------------------------------------------------------
	// begin panel
	// -------------------------------------------------------
	bool begin(const char* header,int* state) {
		guiContext->reset();
		guiContext->header = header;
		
		// build panel header
		float width = 200.0f;
		v2 p = guiContext->position;
		guiContext->startPosition = p;
		//p.x += width / 2.0f;
		//bool active = handleMouse(0, p, v2(width, BOX_HEIGHT));
		bool active = isBoxSelected(0, p, v2(width, BOX_HEIGHT));
		if (active) {
			if (*state == 0) {
				*state = 1;
			}
			else {
				*state = 0;
			}
		}		
		if (*state == 1) {
			guiContext->visible = true;
		}
		else {
			guiContext->visible = false;
		}
		guiContext->nextPosition();
		return *state == 1;
	}

	v2 getTextSize(const char* text) {
		return ds::font::calculateSize(*guiContext->font, text,CHAR_PADDING);
	}
	// -------------------------------------------------------
	// Label
	// -------------------------------------------------------
	void Label(int id,const char* text) {
		v2 p = guiContext->position;
		bool hot = isHot(id,p, getTextSize(text));
		guiContext->addText(p, text);
		guiContext->nextPosition();
	}

	// -------------------------------------------------------
	// handle text input
	// -------------------------------------------------------
	void handleTextInput() {
		int len = strlen(guiContext->inputText);
		if (guiContext->keyInput.num > 0) {
			for (int i = 0; i < guiContext->keyInput.num; ++i) {
				if (guiContext->keyInput.keys[i] == 128) {
					if (guiContext->caretPos > 0) {
						if (guiContext->caretPos < len) {
							memmove(guiContext->inputText + guiContext->caretPos - 1, guiContext->inputText + guiContext->caretPos, len - guiContext->caretPos);
						}
						--guiContext->caretPos;
						--len;
						guiContext->inputText[len] = '\0';
						
					}
				}				
				else if (guiContext->keyInput.keys[i] == 129) {
					if (guiContext->caretPos > 0) {
						--guiContext->caretPos;
					}
				}
				else if (guiContext->keyInput.keys[i] == 130) {
					if (guiContext->caretPos < strlen(guiContext->inputText)) {
						++guiContext->caretPos;
					}
				}
				else if (guiContext->keyInput.keys[i] == 131) {
					guiContext->caretPos = 0;
				}
				else if (guiContext->keyInput.keys[i] == 132) {
					guiContext->caretPos = strlen(guiContext->inputText);
				}
				else if (guiContext->keyInput.keys[i] == 133) {
					guiContext->active = -1;
				}
				else if (guiContext->keyInput.keys[i] > 31 && guiContext->keyInput.keys[i] < 128) {
					if (len < 32) {
						if (guiContext->caretPos < len) {
							memmove(guiContext->inputText + guiContext->caretPos + 1, guiContext->inputText + guiContext->caretPos, len - guiContext->caretPos);
						}
						guiContext->inputText[guiContext->caretPos] = guiContext->keyInput.keys[i];
						++len;
						++guiContext->caretPos;
					}
				}
			}
			++len;
			guiContext->inputText[len] = '\0';
			guiContext->keyInput.num = 0;
		}
	}

	// -------------------------------------------------------
	// input scalar
	// -------------------------------------------------------
	void InputScalar(int id, int index, int* v, float width = INPUT_BOX_WIDTH) {
		int new_id = id + 1024 * index;
		v2 p = guiContext->position;
		p.x += (width + 10.0f) * index;
		bool hot = isHot(new_id, p, v2(width, BOX_HEIGHT),width * 0.5f);
		bool selected = isBoxSelected(new_id, p, v2(width, BOX_HEIGHT));
		if (selected) {
			sprintf_s(guiContext->inputText, 32, "%d", *v);
			guiContext->caretPos = strlen(guiContext->inputText);
			guiContext->active = new_id;
		}
		if (guiContext->active == new_id) {
			guiContext->addBox(p, v2(width, BOX_HEIGHT), guiContext->colors[CLR_INPUT_EDIT]);
			handleTextInput();
			*v = atoi(guiContext->inputText);
			v2 cp = p;
			v2 cursorPos = ds::font::calculateLimitedSize(*guiContext->font, guiContext->inputText,guiContext->caretPos,CHAR_PADDING);
			cp.x = guiContext->position.x + TEXT_PADDING + (width + 10.0f)  * index + cursorPos.x;
			cp.y -= 2.0f;
			guiContext->addBox(cp, v2(2, 18.0f), ds::Color(192, 0, 0, 255));
			guiContext->addText(p, guiContext->inputText);
		}
		else {
			char buffer[32];
			sprintf_s(buffer, 32, "%d", *v);
			guiContext->addBox(p, v2(width, BOX_HEIGHT), guiContext->colors[CLR_INPUT]);
			guiContext->addText(p, buffer);
		}		
	}

	// -------------------------------------------------------
	// input scalar
	// -------------------------------------------------------
	void InputScalar(int id, int index, float* v, float width = INPUT_BOX_WIDTH) {
		int new_id = id + 1024 * index;
		v2 p = guiContext->position;
		p.x += (width + 10.0f) * index;
		bool hot = isHot(new_id, p, v2(width, BOX_HEIGHT), width * 0.5f);
		bool selected = isBoxSelected(new_id, p, v2(width, BOX_HEIGHT));
		if ( selected) {
			sprintf_s(guiContext->inputText, 32, "%.2f", *v);
			guiContext->caretPos = strlen(guiContext->inputText);
			guiContext->active = new_id;
		}
		if (guiContext->active == new_id) {
			guiContext->addBox(p, v2(width, BOX_HEIGHT), guiContext->colors[CLR_INPUT_EDIT]);
			handleTextInput();
			*v = atoi(guiContext->inputText);
			v2 cp = p;
			v2 cursorPos = ds::font::calculateLimitedSize(*guiContext->font, guiContext->inputText, guiContext->caretPos, CHAR_PADDING);
			cp.x = guiContext->position.x + TEXT_PADDING + 110.0f * index + cursorPos.x;
			cp.y -= 2.0f;
			guiContext->addBox(cp, v2(2, BOX_HEIGHT), ds::Color(192, 0, 0, 255));
			guiContext->addText(p, guiContext->inputText);
		}
		else {
			char buffer[32];
			sprintf_s(buffer, 32, "%.2f", *v);
			guiContext->addBox(p, v2(width, BOX_HEIGHT), guiContext->colors[CLR_INPUT]);
			guiContext->addText(p, buffer);
		}
	}

	// -------------------------------------------------------
	// input scalar
	// -------------------------------------------------------
	void InputScalar(int id, int index, char* v,int maxLength,float width = INPUT_BOX_WIDTH) {
		int new_id = id + 1024 * index;
		v2 p = guiContext->position;
		p.x += (width + 10.0f) * index;
		bool hot = isHot(new_id, p, v2(width, BOX_HEIGHT), width * 0.5f);
		bool selected = isBoxSelected(new_id, p, v2(width, BOX_HEIGHT));
		if (selected) {
			sprintf_s(guiContext->inputText, maxLength, "%s", v);
			guiContext->caretPos = strlen(guiContext->inputText);
			guiContext->active = new_id;
		}
		if (guiContext->active == new_id) {
			guiContext->addBox(p, v2(width, BOX_HEIGHT), guiContext->colors[CLR_INPUT_EDIT]);
			handleTextInput();
			*v = atoi(guiContext->inputText);
			v2 cp = p;
			v2 cursorPos = ds::font::calculateLimitedSize(*guiContext->font, guiContext->inputText, guiContext->caretPos, CHAR_PADDING);
			cp.x = guiContext->position.x + TEXT_PADDING + (width + 10.0f) * index + cursorPos.x;
			cp.y -= 2.0f;
			guiContext->addBox(cp, v2(2, BOX_HEIGHT), ds::Color(192, 0, 0, 255));
			guiContext->addText(p, guiContext->inputText);
			sprintf_s(v, maxLength, "%s", guiContext->inputText);
		}
		else {
			char buffer[32];
			sprintf_s(buffer, 32, "%s", v);
			guiContext->addBox(p, v2(width, BOX_HEIGHT), guiContext->colors[CLR_INPUT]);
			guiContext->addText(p, buffer);
		}
	}

	void Input(int id, const char* label, char* str, int maxLength) {
		InputScalar(id, 0, str, maxLength,400.0f);
		v2 p = guiContext->position;
		p.x += 410.0f;
		guiContext->addText(p, label);
		guiContext->nextPosition();
	}

	// -------------------------------------------------------
	// input float
	// -------------------------------------------------------
	void InputFloat(int id, const char* label, float* v) {
		InputScalar(id, 0, v);
		v2 p = guiContext->position;
		p.x += INPUT_BOX_WIDTH + 10.0f;
		guiContext->addText(p, label);
		guiContext->nextPosition();
	}

	// -------------------------------------------------------
	// input int
	// -------------------------------------------------------
	void InputInt(int id, const char* label, int* v) {		
		InputScalar(id, 0, v);
		v2 p = guiContext->position;
		p.x += INPUT_BOX_WIDTH + 10.0f;
		guiContext->addText(p, label);
		guiContext->nextPosition();
	}

	// -------------------------------------------------------
	// input int using steps
	// -------------------------------------------------------
	void InputFloat(int id, const char* label, float* v, float minValue, float maxValue, float step) {
		v2 p = guiContext->position;
		guiContext->addImage(p, guiContext->textures[ICN_MINUS], BOX_HEIGHT*0.5f);
		if (isBoxSelected(id, p, v2(BOX_HEIGHT, BOX_HEIGHT))) {
			*v -= step;			
		}
		float width = INPUT_BOX_WIDTH;
		p.x += BOX_HEIGHT;
		guiContext->addBox(p, v2(width, BOX_HEIGHT), guiContext->colors[CLR_INPUT]);
		p.x += INPUT_BOX_WIDTH;
		guiContext->addImage(p, guiContext->textures[ICN_PLUS], BOX_HEIGHT*0.5f);
		if (isBoxSelected(id, p, v2(BOX_HEIGHT, BOX_HEIGHT))) {
			*v += step;			
		}
		if (*v < minValue) {
			*v = minValue;
		}
		if (*v > maxValue) {
			*v = maxValue;
		}
		char buffer[16];
		sprintf_s(buffer, 16, "%.2f", *v);
		v2 dim = getTextSize(buffer);
		p = guiContext->position;
		p.x += BOX_HEIGHT + (INPUT_BOX_WIDTH - dim.x) * 0.5f;
		guiContext->addText(p, buffer);
		p = guiContext->position;
		p.x += INPUT_BOX_WIDTH + BOX_HEIGHT * 2.0f + 10.0f;
		guiContext->addText(p, label);
		guiContext->nextPosition();
	}

	// -------------------------------------------------------
	// input int using steps
	// -------------------------------------------------------
	void InputInt(int id, const char* label, int* v,int minValue,int maxValue,int step) {
		v2 p = guiContext->position;
		guiContext->addImage(p, guiContext->textures[ICN_MINUS], BOX_HEIGHT*0.5f);
		if (isBoxSelected(id, p, v2(BOX_HEIGHT, BOX_HEIGHT))) {
			*v -= step;			
		}
		float width = INPUT_BOX_WIDTH;
		p.x += BOX_HEIGHT;
		guiContext->addBox(p, v2(width, BOX_HEIGHT), guiContext->colors[CLR_INPUT]);		
		p.x += INPUT_BOX_WIDTH;
		guiContext->addImage(p, guiContext->textures[ICN_PLUS], BOX_HEIGHT*0.5f);
		if (isBoxSelected(id, p, v2(BOX_HEIGHT, BOX_HEIGHT))) {
			*v += step;			
		}
		if (*v < minValue) {
			*v = minValue;
		}
		if (*v > maxValue) {
			*v = maxValue;
		}
		char buffer[16];
		sprintf_s(buffer, 16, "%d", *v);
		v2 dim = getTextSize(buffer);
		p = guiContext->position;
		p.x += BOX_HEIGHT + (INPUT_BOX_WIDTH - dim.x) * 0.5f;
		guiContext->addText(p, buffer);
		p = guiContext->position;
		p.x += INPUT_BOX_WIDTH + BOX_HEIGHT * 2.0f + 10.0f;
		guiContext->addText(p, label);
		guiContext->nextPosition();
	}

	// -------------------------------------------------------
	// input v2
	// -------------------------------------------------------
	void InputVec2(int id, const char* label, v2* v) {
		InputScalar(id, 0, &v->x);
		InputScalar(id, 1, &v->y);	
		v2 p = guiContext->position;
		p.x += (INPUT_BOX_WIDTH + 10.0f) * 2.0f;
		guiContext->addText(p, label);
		guiContext->nextPosition();
	}

	// -------------------------------------------------------
	// input color
	// -------------------------------------------------------
	void InputColor(int id, const char* label, ds::Color* v) {
		int r = v->r * 255.0f;
		int g = v->g * 255.0f;
		int b = v->b * 255.0f;
		int a = v->a * 255.0f;
		InputScalar(id, 0, &r, 40.0f);
		InputScalar(id, 1, &g, 40.0f);
		InputScalar(id, 2, &b, 40.0f);
		InputScalar(id, 3, &a, 40.0f);
		*v = ds::Color(r, g, b, a);
		v2 p = guiContext->position;
		p.x += 200.0f;
		guiContext->addBox(p, v2(BOX_HEIGHT, BOX_HEIGHT), *v);
		p.x += BOX_HEIGHT;
		guiContext->addText(p, label);
		guiContext->nextPosition();
	}

	// -------------------------------------------------------
	// input vec3
	// -------------------------------------------------------
	void InputVec3(int id, const char* label, v3* v) {
		InputScalar(id, 0, &v->x);
		InputScalar(id, 1, &v->y);
		InputScalar(id, 2, &v->z);
		v2 p = guiContext->position;
		p.x += (INPUT_BOX_WIDTH + 10.0f) * 3.0f;
		guiContext->addText(p, label);
		guiContext->nextPosition();
	}

	// -------------------------------------------------------
	// input rect
	// -------------------------------------------------------
	void InputRect(int id, const char* label, ds::Rect* v) {
		int top = v->top;
		int left = v->left;
		int width = v->width();
		int height = v->height();
		InputScalar(id, 0, &top);
		InputScalar(id, 1, &left);
		InputScalar(id, 2, &width);
		InputScalar(id, 3, &height);
		*v = ds::Rect(top, left, width, height);
		v2 p = guiContext->position;
		p.x += (INPUT_BOX_WIDTH + 10.0f) * 4.0f;
		guiContext->addText(p, label);
		guiContext->nextPosition();
	}

	// -------------------------------------------------------
	// combo box
	// -------------------------------------------------------	
	void ComboBox(int id, const std::vector<std::string>& entries, int* selected) {
		int max = entries.size();
		float width = 200.0f;
		v2 p = guiContext->position;
		float height = max * BOX_HEIGHT;
		p.y -= height / 2.0f - BOX_HEIGHT / 2.0f;
		guiContext->addBox(p, v2(width, height), guiContext->colors[CLR_INPUT]);
		p = guiContext->position;
		bool hot = isHot(id, p, v2(width, height));
		for (size_t i = 0; i < max; ++i) {
			if (isBoxSelected(id, p, v2(width, BOX_HEIGHT))) {
				*selected = i;
			}
			if (*selected == i) {
				guiContext->addBox(p, v2(width, BOX_HEIGHT), ds::Color(128, 0, 128, 255));
			}				
			guiContext->addText(p, entries[i].c_str());			
			p.y -= BOX_HEIGHT;
		}
		guiContext->position.y -= height;
		guiContext->position.y -= 8.0f;
	}

	// -------------------------------------------------------
	// combo box
	// -------------------------------------------------------	
	void DropDownBox(int id, const std::vector<std::string>& entries, int* selected,int* state) {
		int max = entries.size();
		float width = 200.0f;
		v2 p = guiContext->position;
		bool hot = isHot(id, p, v2(width, BOX_HEIGHT),width * 0.5f);
		if (isBoxSelected(id, p, v2(width, BOX_HEIGHT))) {
			if (*state == 0) {
				*state = 1;
			}
			else {
				guiContext->active = -1;
				*state = 0;
			}
		}				
		guiContext->addBox(p, v2(width, BOX_HEIGHT), guiContext->colors[CLR_INPUT]);
		if (*state == 0) {					
			guiContext->addImage(p, guiContext->textures[ICN_ARROW_LEFT],width - 10.0f);
		}
		else {
			guiContext->addImage(p, guiContext->textures[ICN_ARROW_DOWN],width - 10.0f);
		}
		int idx = *selected;
		if (*selected < 0 || *selected >= entries.size()) {
			guiContext->addText(p, "Please select one");
		}
		else {
			guiContext->addText(p, entries[idx].c_str());
		}
		
		if (*state == 1) {		
			float height = max * BOX_HEIGHT;
			p.y -= height / 2.0f + BOX_HEIGHT * 0.5f;
			guiContext->addBox(p, v2(width, height), guiContext->colors[CLR_INPUT_EDIT]);
			p = guiContext->position;
			p.y -= BOX_HEIGHT;
			for (size_t i = 0; i < max; ++i) {
				if (isBoxSelected(id, p, v2(width, BOX_HEIGHT),false)) {
					*selected = i;
				}
				if (*selected == i) {
					guiContext->addBox(p, v2(width, BOX_HEIGHT), ds::Color(128, 0, 128, 255));
				}
				guiContext->addText(p, entries[i].c_str());
				p.y -= BOX_HEIGHT;
			}
			guiContext->position.y -= height;
			guiContext->position.y -= BOX_HEIGHT * 0.5f;			
		}
		guiContext->nextPosition();
	}

	// -------------------------------------------------------
	// checkbox
	// -------------------------------------------------------	
	void CheckBox(int id, const char* label, bool* selected) {
		v2 p = guiContext->position;
		if (*selected) {
			guiContext->addImage(p, guiContext->textures[ICN_CHECKBOX],BOX_HEIGHT * 0.5f);
		}
		else {
			guiContext->addImage(p, guiContext->textures[ICN_CHECKBOX_SELECTED], BOX_HEIGHT * 0.5f);
		}
		if (isBoxSelected(id, p, v2(BOX_HEIGHT, BOX_HEIGHT))) {
			*selected = !*selected;
		}
		v2 textDim = getTextSize(label);
		textDim.x += BOX_HEIGHT;
		bool hot = isHot(id, p, textDim,textDim.x * 0.5f);
		p.x += BOX_HEIGHT;		
		guiContext->addText(p, label);
		guiContext->nextPosition();
	}
	
	// -------------------------------------------------------
	// button
	// -------------------------------------------------------	
	bool Button(int id,const char* label) {
		// get text size
		v2 textDim = getTextSize(label);
		float width = textDim.x + BUTTON_PADDING * 2.0f;
		v2 p = guiContext->position;
		p.y -= 4.0f;
		bool hot = isHot(id, p, v2(width, BUTTON_HEIGHT),width * 0.5f);
		if (hot) {
			guiContext->addBox(p, v2(width, BUTTON_HEIGHT), guiContext->colors[CLR_BUTTON_HOVER]);
		}
		else {
			guiContext->addBox(p, v2(width, BUTTON_HEIGHT), guiContext->colors[CLR_BUTTON]);
		}
		p.x = guiContext->position.x + (width - textDim.x) / 2.0f;
		guiContext->addText(p,label, textDim);
		guiContext->nextPosition();
		return isBoxSelected(id, p, v2(width, BUTTON_HEIGHT));
	}

	// -------------------------------------------------------
	// intialize gui
	// -------------------------------------------------------	
	void initialize() {
		guiContext = new GUIContext;
		guiContext->textureID = ds::renderer::loadTexture("gui");
		assert(guiContext->textureID != -1);
		guiContext->font = ds::renderer::createBitmapFont("gui_font");		
		ds::FontDefinition fdf;
		/*
		fdf.startChar = 32;
		fdf.endChar = 128;
		fdf.width = 222;
		fdf.height = 104;
		fdf.padding = 1;
		fdf.textureSize = 512.0f;
		fdf.charHeight = 14;
		fdf.startX = 0;
		fdf.startY = 0;
		fdf.gridHeight = 16;
		*/
		fdf.startChar = 32;
		fdf.endChar = 128;
		fdf.width = 209;
		fdf.height = 92;
		fdf.padding = 1;
		fdf.textureSize = 512.0f;
		fdf.charHeight = 14;
		fdf.startX = 0;
		fdf.startY = 0;
		fdf.gridHeight = 14;

		guiContext->font->intialize(fdf);
		ds::renderer::initializeBitmapFont(*guiContext->font, guiContext->textureID);
		guiContext->clicked = false;
		guiContext->released = false;
		guiContext->buttonPressed = false;
		guiContext->active = -1;
		guiContext->hot = -1;
		// some pre build icons / textures
		guiContext->textures[ICN_ARROW_RIGHT] = ds::math::buildTexture(16.0f, 256.0f, BOX_HEIGHT, BOX_HEIGHT, 512.0f, 512.0f);
		guiContext->textures[ICN_ARROW_DOWN]  = ds::math::buildTexture(16.0f, 272.0f, BOX_HEIGHT, BOX_HEIGHT, 512.0f, 512.0f);
		guiContext->textures[ICN_ARROW_LEFT]  = ds::math::buildTexture(16.0f, 288.0f, BOX_HEIGHT, BOX_HEIGHT, 512.0f, 512.0f);
		guiContext->textures[ICN_ARROW_UP]    = ds::math::buildTexture(16.0f, 304.0f, BOX_HEIGHT, BOX_HEIGHT, 512.0f, 512.0f);
		guiContext->textures[ICN_PLUS]        = ds::math::buildTexture(16.0f, 336.0f, BOX_HEIGHT, BOX_HEIGHT, 512.0f, 512.0f);
		guiContext->textures[ICN_MINUS]       = ds::math::buildTexture(16.0f, 320.0f, BOX_HEIGHT, BOX_HEIGHT, 512.0f, 512.0f);
		guiContext->textures[ICN_CHECKBOX]    = ds::math::buildTexture(16.0f, 352.0f, BOX_HEIGHT, BOX_HEIGHT, 512.0f, 512.0f);
		guiContext->textures[ICN_CHECKBOX_SELECTED] = ds::math::buildTexture(16.0f, 368.0f, BOX_HEIGHT, BOX_HEIGHT, 512.0f, 512.0f);
		
		guiContext->colors[CLR_PANEL_BACKGROUND] = ds::Color(32, 32, 32, 255);
		guiContext->colors[CLR_PANEL_HEADER] = ds::Color(0, 111, 204, 255);
		guiContext->colors[CLR_INPUT] = ds::Color(41, 46, 52, 255);
		guiContext->colors[CLR_INPUT_EDIT] = ds::Color(128, 128, 128, 255);
		guiContext->colors[CLR_BUTTON] = ds::Color(89, 101, 117, 255);
		guiContext->colors[CLR_BUTTON_HOVER] = ds::Color(66, 76, 88, 255);

		guiContext->ready = true;
	}

	void debugWindow() {
		GUIWindow& win = guiContext->window;
		if (win.num > 0) {
			for (int i = 0; i < win.num; ++i) {
				const DrawCall& call = win.calls[i];
				LOG << "type: " << call.type << " position: " << DBG_V2(call.position) << " size: " << DBG_V2(call.size);
			}

		}
	}

	// -------------------------------------------------------
	// end panel
	// -------------------------------------------------------	
	void end() {
		assert(guiContext->ready);
		int current = ds::sprites::getCurrentTextureID();
		ds::sprites::setTexture(guiContext->textureID);
		// get dimension of entire panel
		v2 dim = guiContext->dim;
		if (dim.x == 0.0f) {
			dim.x = 200.0f;
		}
		dim += v2(20, 20);
		// draw header
		v2 p = guiContext->startPosition;
		float sx = 1.0f;
		if (dim.x > WHITE_BOX_SIZE) {
			sx = dim.x / WHITE_BOX_SIZE;
			dim.x = WHITE_BOX_SIZE;
		}		
		p.x = guiContext->startPosition.x + dim.x / 2.0f * sx - 10.0f;
		ds::sprites::draw(p, buildBoxTexture(dim.x, BOX_HEIGHT), 0.0f, sx, 1.0f, guiContext->colors[CLR_PANEL_HEADER]);
		p.x = guiContext->startPosition.x - 2.0f;
		if (guiContext->visible) {
			ds::sprites::draw(p, guiContext->textures[ICN_ARROW_DOWN]);
		}
		else {
			ds::sprites::draw(p, guiContext->textures[ICN_ARROW_RIGHT]);
		}
		p.y -= 7.0f;
		p.x = guiContext->startPosition.x + 20.0f;
		ds::sprites::drawText(guiContext->font, p.x, p.y, guiContext->header, 2);
		if (guiContext->visible) {
			v2 p = guiContext->startPosition;
			float sy = 1.0f;
			if (dim.y > WHITE_BOX_SIZE) {
				sy = dim.y / WHITE_BOX_SIZE;
				dim.y = WHITE_BOX_SIZE;
			}
			v2 center;
			center.x = guiContext->startPosition.x + dim.x / 2.0f * sx - 10.0f;
			center.y = guiContext->startPosition.y - dim.y / 2.0f * sy - 10.0f;
			ds::sprites::draw(center, buildBoxTexture(dim.x, dim.y), 0.0f, sx, sy, guiContext->colors[CLR_PANEL_BACKGROUND]);
		}
		GUIWindow& win = guiContext->window;
		if (win.num > 0) {				
			for (int i = 0; i < win.num; ++i) {
				const DrawCall& call = win.calls[i];
				if (call.type == 1) {
					ds::sprites::draw(call.position, buildBoxTexture(call.size.x, call.size.y), 0.0f, 1.0f, 1.0f, call.color);
				}
				else if (call.type == 2) {
					ds::sprites::drawText(guiContext->font, call.position.x, call.position.y, call.text, call.padding);
				}
				else if (call.type == 3) {
					ds::sprites::draw(call.position, call.texture, 0.0f, 1.0f, 1.0f, call.color);
				}
			}
				
		}		
		
		ds::sprites::setTexture(current);
		guiContext->position.y -= 10.0f;
	}

}
