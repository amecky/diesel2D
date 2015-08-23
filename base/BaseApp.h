#pragma once
#include "..\dxstdafx.h"
#include <vector>
#include "..\utils\mtrand.h"
#include "..\dialogs\DialogManager.h"
#include "..\audio\AudioManager.h"
#include "GameObject.h"
#include "..\compiler\AssetCompiler.h"
#include "..\renderer\graphics.h"

namespace ds {

class GameStateManager;
class StateMachine;

struct GameTime {
	float elapsed;
	uint32 elapsedMillis;
	uint32 totalTime;
};

class BaseApp {

typedef std::vector<GameObject*> GameObjects;

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
	virtual void handleCollisions() {}
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
	const Vector2f& getMousePos() const {
		return m_MousePos;
	}
	const Vector2f& getTwistedMousePos() const {
		return m_TwistedMousePos;
	}
	void setMousePos(int x,int y) {		
		m_MousePos.x = static_cast<float>(x);
		m_MousePos.y = static_cast<float>(y);
		m_TwistedMousePos.x = static_cast<float>(x);
		m_TwistedMousePos.y = renderer::getScreenHeight() - static_cast<float>(y);
		renderer::setMousePosition(m_TwistedMousePos.x, m_TwistedMousePos.y);
	}
	float random(float min,float max) {
		return min + (max - min)* (float)rand();
	}	
	template<class S>
	void createGameObject(S* obj);
	//void createParticleSystem(const char* fileName,int textureID,ParticleSystem* entity,int maxParticles,int blendState = -1);
	StateMachine* getStateMachine() const {
		return stateMachine;
	}
	DialogManager* getGUI() {
		return &gui;
	}
	//Renderer* getRenderer() {
		//return renderer;
	//}
	//AssetCompiler* getAssets() {
		//return &assets;
	//}
	AudioManager* getAudio() {
		return audio;
	}
protected:
	void loadSprites();
	void initializeGUI();
	virtual const char* getTitle() = 0;
	virtual void OnButtonDown(int button,int x,int y) {}
	virtual void OnButtonUp(int button,int x,int y) {}
	virtual void OnKeyDown( WPARAM wParam ) {}
	virtual void OnKeyUp( WPARAM wParam ) {}
	virtual void onGUIButton(DialogID dlgID,int button) {}
	virtual void OnChar(char ascii,unsigned int keyState) {}
	//Renderer* renderer;
	DialogManager gui;
	AudioManager* audio;
	//AssetCompiler assets;
	HINSTANCE hInstance;
	HWND m_hWnd;
	bool m_Active;
	WINDOWPLACEMENT m_wp;	
	int m_Width;
	int m_Height;
	bool m_Fullscreen;
	GameTime m_GameTime;
	Color m_ClearColor;
	StateMachine* stateMachine;
private:
	Vector2f m_MousePos;
	Vector2f m_TwistedMousePos;
	DWORD m_CurTime;
	float g_fElapsedTime;
	DWORD m_LastTime;
	bool m_Loading;
	KeyStates m_KeyStates;
	DebugInfo m_DebugInfo;
	ButtonState m_ButtonState;
	MTRand_open rand;		
	GameObjects m_GameObjects;
	bool m_Running;
	
}; 

template<class S>
void BaseApp::createGameObject(S* obj) {
	//obj->setParticleManager(particles);
	//obj->setAssetCompiler(&assets);
	obj->setAudioManager(audio);
	obj->init();
	obj->GameObject::setActive(true);
	m_GameObjects.push_back(obj);
}

};