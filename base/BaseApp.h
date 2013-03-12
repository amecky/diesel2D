#pragma once
#include "..\dxstdafx.h"
#include <vector>
//#include "..\content\ResManager.h"
#include "..\renderer\Renderer.h"
#include "..\utils\mtrand.h"
#include "Game.h"
#include "..\dialogs\DialogManager.h"

namespace ds {
	
struct GameTime {
	float elapsed;
	uint32 elapsedMillis;
	uint32 totalTime;
};

class BaseApp {

struct DebugInfo {
	bool showProfiler;
	bool showDrawCounter;
	bool printProfiler;
	bool debugRenderer;
};
	
struct KeyStates {
	int ascii;
	bool onChar;
	WPARAM keyPressed;
	bool keyDown;
	WPARAM keyReleased;
	bool keyUp;
};

struct ButtonState {
	int button;
	int x;
	int y;
	bool down;
	bool processed;
};

public:
	BaseApp();
	virtual ~BaseApp();
	void init();
	bool virtual initialize() {
		return true;
	}
	virtual bool loadContent() {
		return true;
	}
	virtual void update(const GameTime& gameTime) {}
	virtual void draw(const GameTime& gameTime) {}
	void setInstance(const HINSTANCE hInst){ 
		hInstance = hInst; 
	}
	void createWindow();
	HWND getWindow() {
		return m_hWnd;
	}
	const bool isDone() const {
		return !m_Active;
	}
	void shutdown();
	bool isLoading() {
		return m_Loading;
	}
	void initTimer();
	void updateTime();
	void buildFrame();

	void sendOnChar(char ascii,unsigned int state);
	void sendKeyDown(WPARAM virtualKey);
	void sendKeyUp(WPARAM virtualKey);
	void sendButton(int button,int x,int y,bool down);
	const Vec2& getMousePos() const {
		return m_MousePos;
	}
	void setMousePos(int x,int y) {		
		m_MousePos.x = x;
		m_MousePos.y = y;
	}
	float random(float min,float max) {
		return min + (max - min)* (float)rand();
	}	
protected:
	virtual const char* getTitle() = 0;
	virtual void OnButtonDown(int button,int x,int y) {}
	virtual void OnButtonUp(int button,int x,int y) {}
	virtual void OnKeyDown( WPARAM wParam ) {}
	virtual void OnKeyUp( WPARAM wParam ) {}
	virtual void onGUIButton(DialogID dlgID,int button) {}
	virtual void OnChar(char ascii,unsigned int keyState) {}
	void loadSkyNode(const std::string& envMapFile,float radius);	
	Renderer* renderer;
	DialogManager* gui;
	Game* game;
	HINSTANCE hInstance;
	HWND m_hWnd;
	bool m_Active;
	WINDOWPLACEMENT m_wp;	
	int m_Width;
	int m_Height;
	GameTime m_GameTime;
	Color m_ClearColor;
private:
	void loadTVarFile();
	Vec2 m_MousePos;
	DWORD m_CurTime;
	float g_fElapsedTime;
	DWORD m_LastTime;
	bool m_Loading;
	KeyStates m_KeyStates;
	DebugInfo m_DebugInfo;
	ButtonState m_ButtonState;
	MTRand_open rand;		
}; 

};