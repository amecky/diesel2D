#include "BaseApp.h"
#include "..\utils\Log.h"
#include <windows.h>
#include "..\utils\Profiler.h"
#include "..\utils\PlainTextReader.h"
#include "..\memory\DataBlockAllocator.h"
#include "..\io\FileWatcher.h"
#include "..\utils\FileUtils.h"
#include "..\particles\ParticleSystem.h"
#include "..\sprites\SpriteBatch.h"
#include "..\renderer\graphics.h"
#include "GameStateMachine.h"
#include "..\ui\IMGUI.h"
#include "..\DialogResources.h"
#include "..\particles\ParticleManager.h"
#include "..\editor\DialogsEditor.h"
#include "..\editor\SpriteTemplatesEditor.h"

namespace ds {

// -------------------------------------------------------
// Constructing new BaseApp
// -------------------------------------------------------	
BaseApp::BaseApp() {
	gBlockMemory = new DataBlockAllocator();
	gFileWatcher = new FileWatcher();
	m_Active = true;
	m_GameTime.elapsed = 0.0f;
	m_GameTime.elapsedMillis = 0;
	m_GameTime.totalTime = 0;
	m_CurTime = 0;
	m_LastTime = 0;
	m_Loading = true;
	//gProfiler = new NewProfiler();
	m_DebugInfo.showProfiler = false;
	m_DebugInfo.printProfiler = false;
	m_DebugInfo.showDrawCounter = false;
	m_DebugInfo.debugRenderer = false;
	m_DebugInfo.performanceOverlay = false;
	m_DebugInfo.showEditor = false;
	m_DebugInfo.profilerTicks = 0;
	m_ButtonState.processed = true;
	m_MousePos = Vector2f(0,0);
	rand.seed(GetTickCount());
	audio = new AudioManager;
	_totalTime = 0.0f;
	stateMachine = new GameStateMachine;
	_editor.dialogPos = v2(750, 750);
	_editor.position = v2(750, 750);
	_editor.dialogIndex = -1;
	_editor.state = 1;
	particles = new ParticleManager;

	_dialogsEditor = 0;
	_templatesEditor = 0;
	_perfHUDPos = v2(750, 750);
	
}

// -------------------------------------------------------
// Destructor
// -------------------------------------------------------
BaseApp::~BaseApp() {
	LOGC("BaseApp") << "Destructing all elements";
	sprites::shutdown();
	delete _dialogsEditor;
	delete particles;
	delete stateMachine;
	//delete gProfiler;
	delete audio;
	renderer::shutdown();	
	gui::shutdown();
	delete gFileWatcher;	
	delete gBlockMemory;

	//delete renderContext;
}

// -------------------------------------------------------
// Init
// -------------------------------------------------------
void BaseApp::prepare() {
	LOGC("BaseApp") << "---> Init <---";
	//settings.mode = 1;		
	renderer::initialize(m_hWnd,_settings);   	
	audio->initialize(m_hWnd);

	profiler::init();
	sprites::init();
	LOGC("BaseApp") << "---> Start loading content <---";
	loadContent();	
	LOGC("BaseApp") << "---> End loading content   <---";
	stateMachine->initializeStates();
	m_Loading = false;	
	init();
	if (_settings.initializeEditor) {
		_bmfDialog.init();
		_dialogsEditor = new DialogsEditor(&gui);
		_templatesEditor = new SpriteTemplatesEditor(renderer::getSpriteTemplates());
		_templatesEditor->init();
	}
	logKeyBindings();
	m_Running = true;
}

void BaseApp::logKeyBindings() {
	LOGC("BaseApp") << "-----------> Key Bindings   <-----------";
	LOGC("BaseApp") << "F1 -> print profiler";
	LOGC("BaseApp") << "F2 -> show draw counter";
	LOGC("BaseApp") << "F3 -> show profiler";
	LOGC("BaseApp") << "F4 -> toggle update flag";
	LOGC("BaseApp") << "F5 -> toggle performance overlay";
	LOGC("BaseApp") << "F6 -> toggle editor";
	LOGC("BaseApp") << "F7 -> debug renderer";
}

// -------------------------------------------------------
// Load sprites
// -------------------------------------------------------
void BaseApp::loadSprites() {
}

void BaseApp::initializeGUI(BitmapFont* font) {
	//ds::assets::load("gui", &gui, CVT_GUI);
	gui.init(font);
	gui.load();
}
// -------------------------------------------------------
// Creates the window
// -------------------------------------------------------
void BaseApp::createWindow() {
	RECT DesktopSize;
	GetClientRect(GetDesktopWindow(),&DesktopSize);
	// Create the application's window
	m_hWnd = CreateWindow( "Diesel", "Diesel",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE, 
		(DesktopSize.right - _settings.screenWidth) / 2,(DesktopSize.bottom - _settings.screenHeight) / 2,
		_settings.screenWidth, _settings.screenHeight,
		NULL, NULL, hInstance, NULL );

	if (!m_hWnd) 	{
		DWORD dw = GetLastError(); 
		LOGC("BaseApp") << "Failed to created window";
		//ErrorExit(TEXT("CreateWindow"));
		ExitProcess(dw); 
	}

	// Adjust to desired size
	RECT rect = { 0, 0, _settings.screenWidth, _settings.screenHeight };
	AdjustWindowRect( &rect, GetWindowLong( m_hWnd, GWL_STYLE ), FALSE );
	SetWindowPos( m_hWnd, HWND_TOP, 0, 0, rect.right - rect.left, rect.bottom - rect.top, 
		SWP_NOZORDER | SWP_NOMOVE  );

	LOG << "window rect " << rect.top << " " << rect.left << " " << rect.bottom << " " << rect.right;
	ShowWindow( m_hWnd, SW_SHOW );
	UpdateWindow( m_hWnd );

	SetWindowText(m_hWnd,getTitle());

	// Save current location/size
	ZeroMemory( &m_wp, sizeof( WINDOWPLACEMENT ) );
	m_wp.length = sizeof( WINDOWPLACEMENT );
	GetWindowPlacement( m_hWnd, &m_wp );	

	LOGC("Framework") << "window created";
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
	g_fElapsedTime = 0.016f;
	m_LastTime = m_CurTime;
	m_GameTime.elapsed = g_fElapsedTime;
	m_GameTime.elapsedMillis = static_cast<uint32>(g_fElapsedTime * 1000.0f);
	m_GameTime.totalTime += m_GameTime.elapsed;
}

// -------------------------------------------------------
// Build frame
// -------------------------------------------------------
void BaseApp::buildFrame() {	
	//gProfiler->reset();	
	profiler::reset();
	//PR_START("MAIN")
	PR_START("FILEWATCHER")
#ifdef DEBUG
	assets::update();
#endif
	PR_END("FILEWATCHER")
	renderer::drawCounter().reset();
	debug::reset();
	// handle key states
	PR_START("INPUT")
	if ( m_KeyStates.keyDown ) {
		m_KeyStates.keyDown = false;
		stateMachine->onKeyDown(m_KeyStates.keyPressed);
		OnKeyDown(m_KeyStates.keyPressed);
	}
	if ( m_KeyStates.keyUp ) {
		m_KeyStates.keyUp = false;
		stateMachine->onKeyUp(m_KeyStates.keyReleased);
		OnKeyUp(m_KeyStates.keyReleased);
	}
	if ( m_KeyStates.onChar ) {
		m_KeyStates.onChar = false;
		if (m_KeyStates.ascii >= 0 && m_KeyStates.ascii < 256) {
			stateMachine->onChar(m_KeyStates.ascii);
			OnChar(m_KeyStates.ascii, 0);
		}
	}
	PR_END("INPUT")
	if ( !m_ButtonState.processed ) {
		m_ButtonState.processed = true;
		if ( m_ButtonState.down ) {
			stateMachine->onButtonDown(m_ButtonState.button, m_ButtonState.x, m_ButtonState.y);
			OnButtonDown(m_ButtonState.button,m_ButtonState.x,m_ButtonState.y);			
		}
		else {
			DialogID did;
			int selected;
			if (gui.onButtonUp(m_ButtonState.button, m_ButtonState.x, m_ButtonState.y, &did, &selected)) {
				onGUIButton(did, selected);
				stateMachine->onGUIButton(did, selected);
			}
			else {
				stateMachine->onButtonUp(m_ButtonState.button, m_ButtonState.x, m_ButtonState.y);
				OnButtonUp(m_ButtonState.button, m_ButtonState.x, m_ButtonState.y);
			}
		}
	}
	sprites::begin();
	if ( m_Running ) {
		_totalTime += m_GameTime.elapsed;
		PR_START("UPDATE")
		gui.updateMousePos(getMousePos());
		gui.tick(m_GameTime.elapsed);
		PR_END("GameObjects::update")
		PR_START("Game::update")
		update(m_GameTime.elapsed);
		stateMachine->update(m_GameTime.elapsed);
		PR_END("Game::update")
		PR_END("UPDATE")
	}
	PR_START("RENDER")
	renderer::beginRendering(_settings.clearColor);	
	renderer::setupMatrices();
	PR_START("RENDER_GAME")
	ds::sprites::begin();		
	stateMachine->render();
	draw();
	ds::sprites::flush();
	PR_END("RENDER_GAME")
	//m_World.draw();
	PRS("RENDER_GUI")
	gui.render();
	PRE("RENDER_GUI")
	PRS("RENDER_EDITOR")
	if (m_DebugInfo.showEditor) {
		showEditor();
	}
	PRE("RENDER_EDITOR")
	debug::drawDebugMessages();
	if (m_DebugInfo.performanceOverlay) {
		showPerformceOverlay(&_perfHUDPos);
	}
	PRS("RENDER_FLUSH")
	renderer::flush();
	PRE("RENDER_FLUSH")
	PR_START("DEBUG_RENDER")
#ifdef DEBUG		
	if ( m_DebugInfo.showDrawCounter ) {
		int y = ds::renderer::getSelectedViewport().getHeight() - 80;
		debug::showDrawCounter(&_perfHUDPos);
	}
	if ( m_DebugInfo.showProfiler ) {
		++m_DebugInfo.profilerTicks;
		if (m_DebugInfo.profilerTicks > 10) {
			m_DebugInfo.profilerTicks = 0;
			m_DebugInfo.snapshotCount = profiler::get_snapshot(_snapshots, 64);
		}
		showProfilerSnapshot(&_perfHUDPos);
	}		
#endif
	
	PR_END("DEBUG_RENDER")
	PR_END("RENDER")
	gui::endFrame();
	profiler::finalize();
	renderer::endRendering();
	//profiler::finalize();
	//PR_END("MAIN")
	if ( m_DebugInfo.printProfiler ) {
		m_DebugInfo.printProfiler = false;
		debug::printDrawCounter();
		LOG << "--------------------------------";
		profiler::print();
	}	
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
	m_KeyStates.ascii = ascii;
	m_KeyStates.onChar = true;
	gui::sendKey(ascii);
}

void BaseApp::sendKeyDown(WPARAM virtualKey) {
	m_KeyStates.keyDown = true;
	m_KeyStates.keyPressed = virtualKey;
#ifdef DEBUG
	
	if ( virtualKey == VK_F1) {
		m_DebugInfo.printProfiler = !m_DebugInfo.printProfiler;
	}
	else if (virtualKey == VK_F2) {
		m_DebugInfo.showDrawCounter = !m_DebugInfo.showDrawCounter;
	}
	else if (virtualKey == VK_F3) {
		m_DebugInfo.showProfiler = !m_DebugInfo.showProfiler;
		m_DebugInfo.profilerTicks = 0;
		m_DebugInfo.snapshotCount = profiler::get_snapshot(_snapshots, 64);
	}
	else if ( virtualKey == VK_F4 ) {
		m_Running = !m_Running;
	}	
	else if (virtualKey == VK_F5) {
		m_DebugInfo.performanceOverlay = !m_DebugInfo.performanceOverlay;
	}
	else if (virtualKey == VK_F6) {
		m_DebugInfo.showEditor = !m_DebugInfo.showEditor;
	}
	else if (virtualKey == VK_F7 && !m_DebugInfo.debugRenderer) {
		m_DebugInfo.debugRenderer = true;
	}
#endif
}

void BaseApp::sendKeyUp(WPARAM virtualKey) {
	m_KeyStates.keyUp = true;
	m_KeyStates.keyReleased = virtualKey;
	gui::sendSpecialKey(virtualKey);
}

void BaseApp::showPerformceOverlay(v2* position) {
	gui::start(EDITOR_ID, position);
	int state = 1;
	if (gui::begin("Performance", &state)) {
		float val[16];// = { 0.2f, 0.4f, 0.3f, 0.1f, 0.15f, 0.6f, 1.0f, 0.1f, 0.9f, 0.75f };
		int count = profiler::get_total_times(val, 16);
		gui::Histogram(17, val, count, 0.0f, 16.0f, 1.0f);
	}
	gui::end();
}

void BaseApp::showEditor() {
	gui::start(EDITOR_ID, &_editor.position);
	if (gui::begin("Game", &_editor.state)) {
		gui::beginGroup();
		if (gui::Button(EDITOR_ID + 3, "GSM")) {
			_editor.dialogIndex = 2;
		}
		if (_dialogsEditor != 0 && gui::Button(EDITOR_ID + 4, "DLG")) {
			_dialogsEditor->init();
			_editor.dialogIndex = 3;
		}
		if (_templatesEditor != 0 && gui::Button(EDITOR_ID + 5, "SPT")) {
			_templatesEditor->init();
			_editor.dialogIndex = 4;
		}
		if (gui::Button(EDITOR_ID + 6, "SCG")) {
			_editor.dialogIndex = 5;
		}
		if (gui::Button(EDITOR_ID + 7, "BMF")) {
			_editor.dialogIndex = 6;
		}
		gui::endGroup();
		gui::beginGroup();
		if (gui::Button(EDITOR_ID + 8, "PS")) {
			_editor.dialogIndex = 7;
		}
		if (gui::Button(EDITOR_ID + 9, "PF")) {
			_editor.dialogIndex = 8;
		}
		gui::endGroup();
	}
	gui::end();

	if (_editor.dialogIndex == 2)  {
		stateMachine->showDialog();
	}
	if (_editor.dialogIndex == 3)  {
		//gui.showDialog();
		_dialogsEditor->showDialog();
	}
	if (_editor.dialogIndex == 4) {
		//renderer::getSpriteTemplates()->showDialog();
		_templatesEditor->showDialog();
	}
	if (_editor.dialogIndex == 5) {
		renderer::getSpriteGroupContainer()->showDialog();
	}
	if (_editor.dialogIndex == 6) {
		_bmfDialog.show();
	}
	if (_editor.dialogIndex == 7) {
		particles->showDialog();
	}
	if (_editor.dialogIndex == 8) {
		int state = 1;
		if (gui::begin("Performance", &state)) {
			float val[16];// = { 0.2f, 0.4f, 0.3f, 0.1f, 0.15f, 0.6f, 1.0f, 0.1f, 0.9f, 0.75f };
			int count = profiler::get_total_times(val, 16);
			gui::Histogram(17, val, count, 0.0f, 16.0f, 1.0f);
		}
		gui::end();
	}
}

void BaseApp::showProfilerSnapshot(v2* position) {
	gui::start(EDITOR_ID, position);
	int state = 1;
	if (gui::begin("Profiler", &state)) {
		char buffer[256];
		for (int i = 0; i < m_DebugInfo.snapshotCount; ++i) {
			ProfileSnapshot& pd = _snapshots[i];
			int ident = pd.level * 2;
			sprintf(buffer, "%3d - %3.8f - %.11s", pd.invokeCounter, pd.totalTime, pd.name);
			gui::Label(EDITOR_ID + 1 + i, buffer);
		}
	}
	gui::end();
}

}
