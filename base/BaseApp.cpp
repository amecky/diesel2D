#include "BaseApp.h"
#include "..\utils\Log.h"
#include <windows.h>
#include "..\utils\Profiler.h"
#include "..\nodes\SkyNode.h"
#include "..\utils\PlainTextReader.h"
#include "..\cvar\TVar.h"

namespace ds {

// -------------------------------------------------------
// Constructing new BaseApp
// -------------------------------------------------------	
BaseApp::BaseApp() {
	m_Active = true;
	m_GameTime.elapsed = 0.0f;
	m_GameTime.elapsedMillis = 0;
	m_GameTime.totalTime = 0;
	m_CurTime = 0;
	m_LastTime = 0;
	m_Loading = true;
	gProfiler = new NewProfiler();
	if ( gTVarSystem == 0 ) {
		gTVarSystem = new TVarSystem();
	}
	m_ClearColor = Color(100,149,237,255);
	m_DebugInfo.showProfiler = false;
	m_DebugInfo.printProfiler = false;
	m_DebugInfo.showDrawCounter = false;
	m_DebugInfo.debugRenderer = false;
	m_ButtonState.processed = true;
	m_MousePos = Vec2(0,0);
	rand.seed(GetTickCount());
	game = new Game();
	gui = new DialogManager();
}

// -------------------------------------------------------
// Destructor
// -------------------------------------------------------
BaseApp::~BaseApp() {
	delete renderer;
	delete gui;
}

// -------------------------------------------------------
// Init
// -------------------------------------------------------
void BaseApp::init() {
	Settings settings;
	settings.fullscreen = false;
	settings.height = m_Height;
	settings.width = m_Width;
	settings.synched = true;
	settings.mode = 1;
	settings.postProcessing = false;
	renderer = new Renderer(m_hWnd,settings);   
	game->init(m_hWnd);
	loadTVarFile();
	initialize();	
	loadContent();
	m_Loading = false;
}

// -------------------------------------------------------
// Creates the window
// -------------------------------------------------------
void BaseApp::createWindow() {
	RECT DesktopSize;
	HBRUSH bgBrush = CreateSolidBrush(RGB(0,0,0));
	GetClientRect(GetDesktopWindow(),&DesktopSize);
	// Create the application's window
	m_hWnd = CreateWindow( "Diesel", "Diesel",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE, 
		(DesktopSize.right - m_Width) / 2,(DesktopSize.bottom - m_Height) / 2,
		m_Width, m_Height,
		NULL, NULL, hInstance, NULL );

	if (!m_hWnd) 	{
		DWORD dw = GetLastError(); 
		LOGC(logINFO,"BaseApp") << "Failed to created window";
		//ErrorExit(TEXT("CreateWindow"));
		ExitProcess(dw); 
	}

	// Adjust to desired size
	RECT rect = { 0, 0, m_Width, m_Height };
	AdjustWindowRect( &rect, GetWindowLong( m_hWnd, GWL_STYLE ), FALSE );
	SetWindowPos( m_hWnd, HWND_TOP, 0, 0, rect.right - rect.left, rect.bottom - rect.top, 
		SWP_NOZORDER | SWP_NOMOVE  );

	LOG(logINFO) << "window rect " << rect.top << " " << rect.left << " " << rect.bottom << " " << rect.right;
	ShowWindow( m_hWnd, SW_SHOW );
	UpdateWindow( m_hWnd );

	SetWindowText(m_hWnd,getTitle());

	// Save current location/size
	ZeroMemory( &m_wp, sizeof( WINDOWPLACEMENT ) );
	m_wp.length = sizeof( WINDOWPLACEMENT );
	GetWindowPlacement( m_hWnd, &m_wp );	

	LOGC(logINFO,"Framework") << "window created";
}

// -------------------------------------------------------
// Shutdown
// -------------------------------------------------------
void BaseApp::shutdown() {
	m_Active = false;
	DestroyWindow(m_hWnd);
}

// -------------------------------------------------------
// Initialize timer
// -------------------------------------------------------
void BaseApp::initTimer() {
	m_LastTime = GetTickCount();
}

// -------------------------------------------------------
// Update game time
// -------------------------------------------------------
void BaseApp::updateTime() {
	m_CurTime = GetTickCount();
	//g_fElapsedTime = (float)((m_CurTime - m_LastTime) * 0.001);
	g_fElapsedTime = 0.016;
	m_LastTime = m_CurTime;
	m_GameTime.elapsed = g_fElapsedTime;
	m_GameTime.elapsedMillis = g_fElapsedTime * 1000.0f;
	m_GameTime.totalTime += m_GameTime.elapsedMillis;
}

// -------------------------------------------------------
// Build frame
// -------------------------------------------------------
void BaseApp::buildFrame() {	
	gProfiler->reset();
	PR_START("MAIN")
	renderer->getDrawCounter().reset();
	renderer->getDebugRenderer().clear();	
	// handle key states
	PR_START("INPUT")
	if ( m_KeyStates.keyDown ) {
		m_KeyStates.keyDown = false;
		OnKeyDown(m_KeyStates.keyPressed);
	}
	if ( m_KeyStates.keyUp ) {
		m_KeyStates.keyUp = false;
		OnKeyUp(m_KeyStates.keyReleased);
	}
	if ( m_KeyStates.onChar ) {
		m_KeyStates.onChar = false;
		// if any dialog has processed the char then do not propagate it
		if ( !gui->OnChar(m_KeyStates.ascii,0) ) {
			OnChar(m_KeyStates.ascii,0);
		}
	}
	PR_END("INPUT")

	if ( !m_ButtonState.processed ) {
		m_ButtonState.processed = true;
		if ( m_ButtonState.down ) {
			OnButtonDown(m_ButtonState.button,m_ButtonState.x,m_ButtonState.y);
			DialogID did;
			int selected;
			if ( gui->onButtonDown(m_ButtonState.button,m_ButtonState.x,m_ButtonState.y,&did,&selected) ) {
				onGUIButton(did,selected);
			}
		}
		else {
			OnButtonUp(m_ButtonState.button,m_ButtonState.x,m_ButtonState.y);
		}
	}
	PR_START("UPDATE")
	gui->updateMousePos(getMousePos());
	update(m_GameTime);
	PR_END("UPDATE")
	PR_START("RENDER")
	renderer->beginRendering(m_ClearColor);	
	renderer->setupMatrices();
	PR_START("RENDER_GAME")
	draw(m_GameTime);
	gui->render();
	PR_END("RENDER_GAME")
	PR_START("DEBUG_RENDER")
#ifdef DEBUG		
	if ( m_DebugInfo.showDrawCounter ) {
		renderer->getDebugRenderer().showDrawCounter(Vec2(10,10));
	}
	if ( m_DebugInfo.showProfiler ) {
		renderer->getDebugRenderer().showProfile(Vec2(10,10));
	}		
#endif
	renderer->getDebugRenderer().draw();
	PR_END("DEBUG_RENDER")
	PR_END("RENDER")		
	renderer->endRendering();	
	PR_END("MAIN")
	if ( m_DebugInfo.printProfiler ) {
		m_DebugInfo.printProfiler = false;
		gProfiler->print();
	}	
}

void BaseApp::loadSkyNode(const std::string& envMapFile,float radius) {
	SkyNode* sn = new SkyNode(renderer,0,envMapFile,radius);
	renderer->setSkyNode(sn);
}

// -------------------------------------------------------
// Mouse button
// -------------------------------------------------------
void BaseApp::sendButton(int button,int x,int y,bool down) {
	m_ButtonState.processed = false;
	m_ButtonState.x = x;
	m_ButtonState.y = y;
	m_ButtonState.button = button;
	m_ButtonState.down = down;
}

// -------------------------------------------------------
// Key message handling
// -------------------------------------------------------
void BaseApp::sendOnChar(char ascii,unsigned int state) {
#ifdef DEBUG
	if ( ascii == 'p' ) {
		gProfiler->print();
	}
	else if ( ascii == 'r' ) {
		loadTVarFile();
	}
#endif
	m_KeyStates.ascii = ascii;
	m_KeyStates.onChar = true;
}

void BaseApp::sendKeyDown(WPARAM virtualKey) {
	m_KeyStates.keyDown = true;
	m_KeyStates.keyPressed = virtualKey;
#ifdef DEBUG
	
	if ( virtualKey == VK_F1 && !m_DebugInfo.printProfiler) {
		m_DebugInfo.printProfiler = true;
	}
	if ( virtualKey == VK_F2 && !m_DebugInfo.showDrawCounter) {
		m_DebugInfo.showDrawCounter = true;
	}
	else if ( virtualKey == VK_F2 && m_DebugInfo.showDrawCounter) {
		m_DebugInfo.showDrawCounter = false;
	}
	if ( virtualKey == VK_F3 && !m_DebugInfo.showProfiler) {
		m_DebugInfo.showProfiler = true;
	}
	else if ( virtualKey == VK_F3 && m_DebugInfo.showProfiler) {
		m_DebugInfo.showProfiler = false;
	}	
	if ( virtualKey == VK_F7 && !m_DebugInfo.debugRenderer) {
		m_DebugInfo.debugRenderer = true;
	}
#endif
}

void BaseApp::sendKeyUp(WPARAM virtualKey) {
	m_KeyStates.keyUp = true;
	m_KeyStates.keyReleased = virtualKey;
}

// -------------------------------------------------------
// Load TVAR file if found
// -------------------------------------------------------
void BaseApp::loadTVarFile() {
	SettingsReader settingsReader;
	if ( settingsReader.parse("content\\resources\\Settings.tvar") ) {	
		LOGC(logINFO,"BaseApp") << "Loading tvar file";
		//PAKWriter writer;
		//writer.open(buffer);
		//writer.writeHeader(DT_TVAR);		
		std::vector<std::string> names;
		settingsReader.getNames(names);
		for ( size_t i = 0; i < names.size(); ++i ) {
			gTVarSystem->update(names[i].c_str(),settingsReader.getValue(names[i]).c_str());
		}
		//gTVarSystem->saveBinaryFile(writer);				
		//writer.close();
#ifdef DEBUG
		gTVarSystem->debug();
#endif
	}
	else {
		LOGC(logINFO,"BaseApp") << "No tvar file found";
	}
}

}
