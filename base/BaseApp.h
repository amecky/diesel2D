#pragma once
#include "..\dxstdafx.h"
#include "..\utils\mtrand.h"
#include "..\dialogs\DialogManager.h"
#include "..\audio\AudioManager.h"
#include "..\renderer\graphics.h"
#include "Settings.h"
#include "..\editor\BitmapFontsDialog.h"
#include "..\utils\Profiler.h"

namespace ds {

class GameStateMachine;
class GameState;
struct BitmapFont;
class ParticleManager;
class SpriteTemplatesEditor;
class World;
class GameConsole;
class BitmapFontsDialog;
class ParticlesEditState;
class DialogEditorState;

struct GameTime {
	float elapsed;
	uint32 elapsedMillis;
	float totalTime;
};

class BaseApp {

struct DebugInfo {
	bool showProfiler;
	bool showDrawCounter;
	bool printProfiler;
	bool debugRenderer;
	bool performanceOverlay;
	bool showEditor;
	int profilerTicks;
	int snapshotCount;
	bool monitoring;
	float treshold;
	bool showConsole;
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
	virtual void init() {}
	void prepare();
	virtual bool loadContent() {
		return true;
	}
	virtual void update(float dt) {}
	virtual void draw() {}
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

	void addGameState(GameState* gameState);
	void activate(const char* name);
	void connectGameStates(const char* firstStateName, int outcome, const char* secondStateName);

protected:
	void activateMonitoring(float threshold);
	void loadSprites();
	void initializeGUI(BitmapFont* font);
	virtual const char* getTitle() = 0;
	virtual void OnButtonDown(int button,int x,int y) {}
	virtual void OnButtonUp(int button,int x,int y) {}
	virtual void OnKeyDown( WPARAM wParam ) {}
	virtual void OnKeyUp( WPARAM wParam ) {}
	virtual void onGUIButton(DialogID dlgID,int button) {}
	virtual void OnChar(char ascii,unsigned int keyState) {}
	void logKeyBindings();
	void showPerformceOverlay(v2* position);	
	DialogManager* gui;
	AudioManager* audio;	
	ParticleManager* particles;
	World* world;	
	GameConsole* console;
	//AssetEditorManager* assetEditors;
private:
	void saveHTMLProfilingReport();
	void loadSettings();
	void showProfilerSnapshot(v2* position);
	//void showEditor();
	void loadShaders(const JSONReader& reader);
	void loadDescriptors(const JSONReader& reader);
	void loadViewports(const JSONReader& reader);
	HINSTANCE hInstance;
	HWND m_hWnd;
	bool m_Active;
	WINDOWPLACEMENT m_wp;
	GameTime m_GameTime;
	float _totalTime;
	Settings _settings;
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
	bool m_Running;
	GameStateMachine* _stateMachine;
	v2 _perfHUDPos;
	ProfileSnapshot _snapshots[64];
	bool _prepared;

	BitmapFontsDialog* _bitmapFontEditor;
	SpriteTemplatesEditor* _spriteTemplatesEditor;
	ParticlesEditState* _particlesEditor;
	DialogEditorState* _dialogEditor;
}; 

}