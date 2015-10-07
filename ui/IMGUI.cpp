#include "IMGUI.h"
#include "..\renderer\graphics.h"
#include "..\sprites\SpriteBatch.h"
#include "..\utils\font.h"

namespace gui {

	const float BUTTON_HEIGHT = 24.0f;
	const float TEXTFIELD_HEIGHT = 24.0f;
	const float TEXT_PADDING = 2.0f;
	const float LINE_HEIGHT = 26.0f;
	const float TEXT_OFFSET = 8.0f;
	const int CHAR_PADDING = 0;
	const float BOX_HEIGHT = 18.0f;
	const float WHITE_BOX_SIZE = 256.0f;
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

	// -------------------------------------------------------
	// GUIWindow
	// -------------------------------------------------------
	struct GUIWindow {

		DrawCall calls[64];
		int num;

		void reset() {
			num = 0;
		}

		void addBox(const v2& position,const v2& size, const ds::Color& color) {
			DrawCall& call = calls[num++];
			call.type = 1;
			call.color = color;
			call.size = size;
			call.position = position;
			call.padding = 2;
		}

		void addImage(const v2& position, const ds::Texture& texture) {
			DrawCall& call = calls[num++];
			call.type = 3;
			call.color = ds::Color::WHITE;
			call.size = v2(1,1);
			call.position = position;
			call.texture = texture;
			call.padding = 1;
		}

		void addText(const v2& position,const char* text, const v2& size) {
			DrawCall& call = calls[num++];
			call.type = 2;
			call.color = ds::Color::WHITE;
			call.size = size;
			sprintf_s(call.text,32,text);
			call.position = position;
			call.position.y -= TEXT_OFFSET;
			call.padding = CHAR_PADDING;
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
		ICN_ARROW_UP
	};

	// -------------------------------------------------------
	// GUI context
	// -------------------------------------------------------
	struct GUIContext {

		ds::BitmapFont* font;
		int textureID;
		GUIWindow windows[8];
		GUIWindow* currentWindow;
		int windowIndex;
		int windowCount;
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
		bool ready;

		GUIContext() {
			textureID = -1;
			ready = false;
		}

		void reset() {
			windowIndex = 0;
			windowCount = 0;	
			currentWindow = 0;
			cursorPosition = ds::renderer::getMousePosition();
			clicked = false;
			grouped = false;
			visible = false;
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

		void pushWindow() {
			currentWindow = &windows[windowIndex++];
			currentWindow->reset();
			++windowCount;
		}

		void popWindow() {
			if (windowIndex > 0) {
				--windowIndex;
			}
		}

		

		void addBox(const v2& position, const v2& size, const ds::Color& color) {
			currentWindow->addBox(position, size, color);
			calculateDimension(position, size);

		}

		void addImage(const v2& position, const ds::Texture& texture) {
			currentWindow->addImage(position, texture);
			calculateDimension(position, texture.dim);
		}

		void addText(const v2& position, const char* text) {
			v2 size = ds::font::calculateSize(*font, text, CHAR_PADDING);
			addText(position, text, size);
			calculateDimension(position, size);
		}

		void addText(const v2& position, const char* text, const v2& size) {
			currentWindow->addText(position, text, size);
			calculateDimension(position, size);
		}

		void nextPosition() {
			if (!grouped) {
				position.y -= LINE_HEIGHT;
			}
			else {
				if (currentWindow != 0 && currentWindow->num > 0) {
					position.x += currentWindow->calls[currentWindow->num - 1].size.x + 50.0f;
				}
			}
		}
	};

	static GUIContext* guiContext;

	void sendKey(unsigned char c) {
		if (guiContext->keyInput.num < 256) {
			guiContext->keyInput.keys[guiContext->keyInput.num++] = c;
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
		if (mp.x >(p.x + dim.x * 0.5f)) {
			return false;
		}
		if (mp.y < (p.y - dim.y * 0.5f)) {
			return false;
		}
		if (mp.y >(p.y + dim.y * 0.5f)) {
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
	}

	// -------------------------------------------------------
	// check if widget is hot
	// -------------------------------------------------------
	bool isHot(int id, const v2& pos, const v2& size) {
		if (isCursorInside(pos, size)) {
			guiContext->hot = id;
			return true;
		}
		return false;
	}

	// -------------------------------------------------------
	// handle mouse interaction
	// -------------------------------------------------------
	bool handleMouse(int id, const v2& pos, const v2& size) {
		if (guiContext->active == id) {
			if (guiContext->clicked && guiContext->hot == id) {
				if (isCursorInside(pos, size)) {
					return true;
				}
				else {
					guiContext->active = -1;
				}
			}
		}
		else if (guiContext->hot == id) {
			//if (guiContext->buttonPressed) {
			if (guiContext->clicked) {
				guiContext->keyInput.num = 0;
				guiContext->active = id;
				return true;
			}
		}
		if (isCursorInside(pos, size)) {
			guiContext->hot = id;
		}
		return false;
	}

	// -------------------------------------------------------
	// handle mouse interaction
	// -------------------------------------------------------
	bool isSelected(int id, const v2& pos, const v2& size) {
		if (guiContext->clicked && isCursorInside(pos, size)) {
			return true;
		}		
		return false;
	}
	// -------------------------------------------------------
	// begin panel
	// -------------------------------------------------------
	bool begin(const char* header,int* state,const v2& startPos) {
		assert(guiContext->ready);
		ds::sprites::setTexture(guiContext->textureID);
		guiContext->reset();
		guiContext->startPosition = startPos;
		guiContext->position = startPos;
		guiContext->pushWindow();
		guiContext->header = header;
		if ((GetKeyState(VK_LBUTTON) & 0x80) != 0) {
			guiContext->buttonPressed = true;
		}
		else {
			if (guiContext->buttonPressed) {
				guiContext->clicked = true;
			}
			guiContext->buttonPressed = false;
		}
		// build panel header
		float width = 200.0f;
		v2 p = guiContext->position;
		p.x += width / 2.0f;
		bool active = handleMouse(0, p, v2(width, BOX_HEIGHT));
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
		guiContext->nextPosition();
		return *state == 1;
	}

	v2 getTextSize(const char* text) {
		return ds::font::calculateSize(*guiContext->font, text,CHAR_PADDING);
	}
	// -------------------------------------------------------
	// Label
	// -------------------------------------------------------
	void Label(const char* text) {
		v2 p = guiContext->position;
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
				else if (guiContext->keyInput.keys[i] == 133) {
					guiContext->caretPos = 0;
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
				else if (guiContext->keyInput.keys[i] == 132) {
					guiContext->caretPos = strlen(guiContext->inputText);
				}
				else if (guiContext->keyInput.keys[i] == 131) {
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
	void InputScalar(int id,int index, int* v) {
		int new_id = id + 1024 * index;
		float width = 100.0f;
		v2 p = guiContext->position;
		p.x += width / 2.0f + 110.0f * index;
		int prev = guiContext->active;
		bool active = handleMouse(new_id, p, v2(width, BOX_HEIGHT));
		if (prev != guiContext->active) {
			sprintf_s(guiContext->inputText, 32, "%d", *v);
			guiContext->caretPos = strlen(guiContext->inputText);
		}
		if (guiContext->active == new_id) {
			guiContext->pushWindow();
			guiContext->addBox(p, v2(width, BOX_HEIGHT), ds::Color(64, 64, 64, 255));
			handleTextInput();
			*v = atoi(guiContext->inputText);
			v2 cp = p;
			v2 cursorPos = ds::font::calculateLimitedSize(*guiContext->font, guiContext->inputText,guiContext->caretPos,CHAR_PADDING);
			cp.x = guiContext->position.x + 8.0f + 110.0f * index + cursorPos.x;
			cp.y -= 2.0f;
			guiContext->addBox(cp, v2(2, 18.0f), ds::Color(192, 0, 0, 255));
			p.x = guiContext->position.x + 8.0f + 110.0f * index;
			guiContext->addText(p, guiContext->inputText);
			guiContext->popWindow();
		}
		else {
			char buffer[32];
			sprintf_s(buffer, 32, "%d", *v);
			guiContext->addBox(p, v2(width, BOX_HEIGHT), ds::Color(128, 128, 128, 255));
			p.x = guiContext->position.x + 8.0f + 110.0f * index;
			guiContext->addText(p, buffer);
		}		
	}

	// -------------------------------------------------------
	// input scalar
	// -------------------------------------------------------
	void InputScalar(int id, int index, float* v) {
		int new_id = id + 1024 * index;
		float width = 100.0f;
		v2 p = guiContext->position;
		p.x += width / 2.0f + 110.0f * index;
		int prev = guiContext->active;
		bool active = handleMouse(new_id, p, v2(width, BOX_HEIGHT));
		if (prev != guiContext->active) {
			sprintf_s(guiContext->inputText, 32, "%.2f", *v);
			guiContext->caretPos = strlen(guiContext->inputText);
		}
		if (guiContext->active == new_id) {
			guiContext->pushWindow();
			guiContext->addBox(p, v2(width, BOX_HEIGHT), ds::Color(64, 64, 64, 255));
			handleTextInput();
			*v = atoi(guiContext->inputText);
			v2 cp = p;
			v2 cursorPos = ds::font::calculateLimitedSize(*guiContext->font, guiContext->inputText, guiContext->caretPos, CHAR_PADDING);
			cp.x = guiContext->position.x + 8.0f + 110.0f * index + cursorPos.x;
			cp.y -= 2.0f;
			guiContext->addBox(cp, v2(2, 18.0f), ds::Color(192, 0, 0, 255));
			p.x = guiContext->position.x + 8.0f + 110.0f * index;
			guiContext->addText(p, guiContext->inputText);
			guiContext->popWindow();
		}
		else {
			char buffer[32];
			sprintf_s(buffer, 32, "%.2f", *v);
			guiContext->addBox(p, v2(width, BOX_HEIGHT), ds::Color(128, 128, 128, 255));
			p.x = guiContext->position.x + 8.0f + 110.0f * index;
			guiContext->addText(p, buffer);
		}
	}

	// -------------------------------------------------------
	// input float
	// -------------------------------------------------------
	void InputFloat(int id, const char* label, float* v) {
		InputScalar(id, 0, v);
		v2 p = guiContext->position;
		p.x += 110.0f;
		guiContext->addText(p, label);
		guiContext->nextPosition();
	}

	// -------------------------------------------------------
	// input int
	// -------------------------------------------------------
	void InputInt(int id, const char* label, int* v) {
		InputScalar(id, 0, v);
		v2 p = guiContext->position;
		p.x += 110.0f;
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
		p.x += 220.0f;		
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
		p.x += 330.0f;
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
		p.x += 440.0f;
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
		p.x += width / 2.0f;		
		float height = max * BOX_HEIGHT;
		p.y -= height / 2.0f - BOX_HEIGHT / 2.0f;
		bool hot = isHot(id, p, v2(width, height));
		if (hot) {
			guiContext->addBox(p, v2(width, height), ds::Color(64, 64, 0, 255));
		}
		else {
			guiContext->addBox(p, v2(width, height), ds::Color(32, 32, 0, 255));
		}
		bool inside = isCursorInside(p, v2(width, height));
		p = guiContext->position;
		for (size_t i = 0; i < max; ++i) {
			p.x = guiContext->position.x + 10.0f;
			if (*selected == i) {
				v2 selp = p;
				selp.x += width / 2.0f;
				selp.x -= 10.0f;
				guiContext->addBox(selp, v2(width, BOX_HEIGHT), ds::Color(128, 0, 128, 255));
			}				
			v2 textDim = ds::font::calculateSize(*guiContext->font, entries[i].c_str());
			guiContext->addText(p, entries[i].c_str(), textDim);
			if (guiContext->clicked && isCursorInside(p, v2(width, 24.0f))) {
				*selected = i;
			}
			p.y -= BOX_HEIGHT;
		}
		guiContext->position.y -= height;
		guiContext->position.y -= 8.0f;
		//for (int i = 0; i < max ; ++i) {
			//guiContext->nextPosition();
		//}
	}

	// -------------------------------------------------------
	// combo box
	// -------------------------------------------------------	
	void DropDownBox(int id, const std::vector<std::string>& entries, int* selected,int* state) {
		int max = entries.size();
		float width = 200.0f;
		v2 p = guiContext->position;
		p.x += width / 2.0f;
		if (isSelected(id, p, v2(width, BOX_HEIGHT))) {
			if (*state == 0) {
				*state = 1;
			}
			else {
				*state = 0;
			}
		}
		float height = max * BOX_HEIGHT;
		if (*state == 0) {
			height = BOX_HEIGHT;
		}
		p.y -= height / 2.0f - BOX_HEIGHT / 2.0f;
		bool hot = isHot(id, p, v2(width, height));
		if (hot) {		
			guiContext->addBox(p, v2(width, height), ds::Color(64, 64, 0, 255));
		}
		else {
			guiContext->addBox(p, v2(width, height), ds::Color(32, 32, 0, 255));
		}
		int prev = guiContext->active;
		bool active = handleMouse(id, p, v2(width, BOX_HEIGHT));
		bool inside = isCursorInside(p, v2(width, height));
		p = guiContext->position;
		if (*state == 1) {
			v2 iconPos = p;
			iconPos.x += width - 10.0f;
			guiContext->addImage(iconPos, guiContext->textures[ICN_ARROW_DOWN]);
			for (size_t i = 0; i < max; ++i) {
				if (isSelected(id, p, v2(width, BOX_HEIGHT))) {
					*selected = i;
					guiContext->active = -1;
				}
				p.x = guiContext->position.x + 10.0f;
				if (*selected == i) {
					v2 selp = p;
					selp.x += width / 2.0f;
					selp.x -= 10.0f;
					guiContext->addBox(selp, v2(width, BOX_HEIGHT), ds::Color(128, 0, 128, 255));
				}
				v2 textDim = ds::font::calculateSize(*guiContext->font, entries[i].c_str());
				guiContext->addText(p, entries[i].c_str(), textDim);
				if (guiContext->clicked && isCursorInside(p, v2(width, BOX_HEIGHT))) {
					*selected = i;
				}
				p.y -= BOX_HEIGHT;
			}
			
		}
		else {
			v2 iconPos = p;
			iconPos.x += width - 10.0f;
			guiContext->addImage(iconPos, guiContext->textures[ICN_ARROW_LEFT]);
			p.x = guiContext->position.x + 10.0f;
			int idx = *selected;
			if (*selected < 0 || *selected >= entries.size()) {
				idx = 0;
				*selected = 0;
			}
			guiContext->addText(p, entries[idx].c_str());
		}
		guiContext->position.y -= height;
		guiContext->position.y -= 8.0f;
	}
	
	// -------------------------------------------------------
	// button
	// -------------------------------------------------------	
	bool Button(int id,const char* label) {
		// get text size
		v2 textDim = getTextSize(label);
		float width = textDim.x + 40.0f;
		v2 p = guiContext->position;
		p.x += width / 2.0f;
		bool hot = isHot(id, p, v2(width, BUTTON_HEIGHT));
		if (hot) {
			guiContext->addBox(p, v2(width, BUTTON_HEIGHT), ds::Color(0, 64, 64, 255));
		}
		else {
			guiContext->addBox(p, v2(width, BUTTON_HEIGHT), ds::Color(0, 64, 0, 255));
		}
		bool inside = isCursorInside(p, v2(width, BUTTON_HEIGHT));
		p.x = guiContext->position.x + (width - textDim.x) / 2.0f;
		guiContext->addText(p,label, textDim);
		guiContext->nextPosition();
		return handleMouse(id, p, v2(width, BUTTON_HEIGHT));
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
		guiContext->font->intialize(fdf);
		ds::renderer::initializeBitmapFont(*guiContext->font, guiContext->textureID);
		guiContext->clicked = false;
		guiContext->released = false;
		guiContext->buttonPressed = false;
		guiContext->active = -1;
		guiContext->hot = -1;
		// some pre build icons / textures
		guiContext->textures[ICN_ARROW_RIGHT] = ds::math::buildTexture(0.0f, 256.0f, BOX_HEIGHT, BOX_HEIGHT, 512.0f, 512.0f);
		guiContext->textures[ICN_ARROW_DOWN]  = ds::math::buildTexture(0.0f, 272.0f, BOX_HEIGHT, BOX_HEIGHT, 512.0f, 512.0f);
		guiContext->textures[ICN_ARROW_LEFT]  = ds::math::buildTexture(0.0f, 292.0f, BOX_HEIGHT, BOX_HEIGHT, 512.0f, 512.0f);
		guiContext->textures[ICN_ARROW_UP]    = ds::math::buildTexture(0.0f, 310.0f, BOX_HEIGHT, BOX_HEIGHT, 512.0f, 512.0f);
		guiContext->ready = true;
	}

	// -------------------------------------------------------
	// end panel
	// -------------------------------------------------------	
	void end() {
		
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
		ds::sprites::draw(p, buildBoxTexture(dim.x, BOX_HEIGHT), 0.0f, sx, 1.0f, ds::Color(192, 0, 0, 255));
		p.x = guiContext->startPosition.x + 2.0f;
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
			ds::sprites::draw(center, buildBoxTexture(dim.x, dim.y), 0.0f, sx, sy, ds::Color(16, 16, 16, 255));
		}
		for (int w = 0; w < guiContext->windowCount; ++w) {
			GUIWindow& win = guiContext->windows[w];
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
		}
		if (guiContext->hot == -1 && guiContext->clicked) {
			guiContext->active = -1;
		}
		if (guiContext->clicked) {
			guiContext->clicked = false;
		}
	}

}
