#include "BaseApp.h"
#include "..\utils\Log.h"
#include <windows.h>
#include "..\utils\Profiler.h"
#include "..\memory\DataBlockAllocator.h"
#include "..\utils\FileUtils.h"
#include "..\particles\ParticleSystem.h"
#include "..\sprites\SpriteBatch.h"
#include "..\renderer\graphics.h"
#include "GameStateMachine.h"
#include "..\ui\IMGUI.h"
#include "..\DialogResources.h"
#include "..\particles\ParticleManager.h"
#include "..\editor\DialogEditorState.h"
#include "..\editor\SpriteTemplatesEditor.h"
#include "..\editor\SpriteTemplatesState.h"
#include "..\editor\ParticlesEditState.h"
#include "..\io\FileRepository.h"
#include "..\memory\DefaultAllocator.h"
#include "..\world\World.h"
#include <time.h>

namespace ds {

// -------------------------------------------------------
// Constructing new BaseApp
// -------------------------------------------------------	
BaseApp::BaseApp() {
	gDefaultMemory = new DefaultAllocator();
	repository::initialize(repository::RM_DEBUG);
	//gBlockMemory = new DataBlockAllocator();
	
	//gFileWatcher = new FileWatcher();
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
	m_DebugInfo.monitoring = false;
	m_ButtonState.processed = true;
	m_MousePos = Vector2f(0,0);
	rand.seed(GetTickCount());
	audio = new AudioManager;
	_totalTime = 0.0f;
	_stateMachine = new GameStateMachine;
	_editor.dialogPos = v2(10, 710);
	_editor.position = v2(10, 710);
	_editor.dialogIndex = -1;
	_editor.state = 1;
	particles = new ParticleManager;
	gui = new DialogManager;
	world = new World;
	_templatesEditor = 0;
	_perfHUDPos = v2(750, 710);
	_prepared = false;
	JSONReader reader;
	bool ret = reader.parse("content\\engine_settings.json");
	assert(ret);
	int c = reader.find_category("basic_settings");
	assert(c != -1);
	reader.get_int(c, "screen_width", &_settings.screenWidth);
	reader.get_int(c, "screen_height", &_settings.screenHeight);
	reader.get_color(c, "clear_color", &_settings.clearColor);
	
}

// -------------------------------------------------------
// Destructor
// -------------------------------------------------------
BaseApp::~BaseApp() {
	LOG << "Destructing all elements";
	sprites::shutdown();
	repository::shutdown();
	delete _bmfDialog;
	delete _templatesEditor;	
	delete world;
	delete gui;
	delete particles;
	delete _stateMachine;
	//delete gProfiler;
	delete audio;
	renderer::shutdown();	
	gui::shutdown();
	//delete gBlockMemory;
	// FIXME: should be enabled
	delete gDefaultMemory;
}

void BaseApp::loadSettings() {
	LOG << "---- Loading settings ----";
	JSONReader reader;
	bool ret = reader.parse("content\\engine_settings.json");
	assert(ret);
	int root = reader.find_category("init_actions");
	assert(root != -1);

	// load textures
	if (reader.contains_property(root,"textures")) {
		const char* textureNames = reader.get_string(root,"textures");
		if (strchr(textureNames,',') == 0) {
			int texture = ds::renderer::loadTexture(textureNames);
			assert(texture != -1);
		}
		else {
			Array<std::string> values;
			string::split(textureNames,values);
			for (size_t i = 0; i < values.size(); ++i) {
				int texture = ds::renderer::loadTexture(values[i].c_str());
				assert(texture != -1);
			}
		}
	}
	else {
		LOG << "No textures defined";
	}
	// load fonts
	int children[32];
	int fonts_id = reader.find_category("fonts", root);
	if ( fonts_id != -1 ) {
		int num = reader.get_categories(children, 32, fonts_id);
		for (int i = 0; i < num; ++i) {
			const char* texName = reader.get_string(children[i],"texture");
			int texture = renderer::getTextureId(texName);
			assert(texture != -1);
			const char* file = reader.get_string(children[i],"file");
			BitmapFont* font = renderer::loadBitmapFont(file, texture);
		}
	}
	else {
		LOG << "No fonts defined";
	}
	// initialize text system
	int text_id = reader.find_category("use_text_system",root);
	//Category* textSystem = init->getChild("text_system");
	//if (textSystem != 0) {
	if ( text_id != -1 ) {
		const char* fontName = reader.get_string(text_id,"font");
		BitmapFont* font = renderer::getBitmapFont(fontName);
		assert(font != 0);
		sprites::initializeTextSystem(font);
	}
	else {
		LOG << "No textsystem defined";
	}
	int imgui_id = reader.find_category("initialize_imgui", root);
	if (imgui_id != -1) {
		const char* fontName = reader.get_string(imgui_id,"font");
		BitmapFont* font = renderer::getBitmapFont(fontName);
		assert(font != 0);
		initializeGUI(font);
	}
	else {
		LOG << "Not initializing IMGUI";
	}
	// load particles
	bool load_particles = false;
	reader.get_bool(root, "load_particles", &load_particles);
	//int particles_id = reader.find_category("")
	//if (init->getBool("load_particles", false)) {
	if ( load_particles) {
		LOG << "loading particle systems";
		// prepare particle system
		Descriptor desc;
		desc.shader = shader::createParticleShader();
		assert(desc.shader != 0);
		desc.texture = 0;
		desc.blendState = renderer::getDefaultBlendState();
		particles->init(desc);
		particles->load();
	}
	// load dialogs
	bool load_dialogs = false;
	reader.get_bool(root, "load_dialogs", &load_dialogs);
	if ( load_dialogs) {
		gui::initialize();
	}
	// load sprite templates
	bool load_templates = false;
	reader.get_bool(root, "load_sprite_templates", &load_templates);
	if (load_templates) {
		renderer::loadSpriteTemplates();
	}
	else {
		LOG << "Not loading sprite templates";
	}
	bool initialize_editor = false;
	reader.get_bool(root, "initialize_editor", &initialize_editor);
	if (initialize_editor) {
		_bmfDialog = new BitmapFontsDialog;
		_bmfDialog->init();
		_templatesEditor = new SpriteTemplatesEditor(renderer::getSpriteTemplates());
		_templatesEditor->init();
		_stateMachine->add(new SpriteTemplatesState());
		_stateMachine->add(new ParticlesEditState(particles));
		_stateMachine->add(new DialogEditorState(gui));
	}
}
// -------------------------------------------------------
// Init
// -------------------------------------------------------
void BaseApp::prepare() {	
	assert(!_prepared);
	TIMER("Load Settings")
	LOG << "---> Init <---";
	LOG << "---- Loading settings ----";
	//Category root("root");
	//bool loaded = json::read_simplified_json("content\\engine_settings.json", &root);
	/*
	//settings.mode = 1;		
	if (loaded) {
		Category* basic = root.getChild("basic_settings");
		if (basic != 0) {
			//full_screen = false
			//synched = true
			basic->getInt("screen_width", &_settings.screenWidth);
			basic->getInt("screen_height", &_settings.screenHeight);
			basic->getColor("clear_color", &_settings.clearColor);
		}
	}
	*/
	renderer::initialize(m_hWnd,_settings);   	
	audio->initialize(m_hWnd);

	profiler::init();
	sprites::init();
	loadSettings();
	LOG << "---> Start loading content <---";
	loadContent();	
	LOG << "---> End loading content   <---";
	_stateMachine->initializeStates();
	m_Loading = false;	
	init();
	logKeyBindings();
	v2 dp;
	dp.x = _settings.screenWidth - 350.0f;
	dp.y = _settings.screenHeight - 10.0f;
	_editor.dialogPos.y = _settings.screenHeight - 10.0f;
	_editor.position.y = _settings.screenHeight - 10.0f;
	_perfHUDPos = dp;
	m_Running = true;
	_prepared = true;
}

void BaseApp::logKeyBindings() {
	LOG << "-----------> Key Bindings   <-----------";
	LOG << "F1  -> print profiler";
	LOG << "F2  -> show draw counter";
	LOG << "F3  -> show profiler";
	LOG << "F4  -> toggle update flag";
	LOG << "F5  -> toggle performance overlay";
	LOG << "F6  -> toggle editor";
	LOG << "F7  -> debug renderer";
	LOG << "F8  -> particle editor";
	LOG << "F9  -> sprite template editor";
	LOG << "F10 -> dialog editor";
}

void BaseApp::activateMonitoring(float threshold) {
	m_DebugInfo.monitoring = true;
	m_DebugInfo.treshold = threshold;
}
// -------------------------------------------------------
// Load sprites
// -------------------------------------------------------
void BaseApp::loadSprites() {
}

void BaseApp::initializeGUI(BitmapFont* font) {
	gui->init(font);
	gui->load();
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
		LOG << "Failed to created window";
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

	LOG << "window created";
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
	renderer::drawCounter().reset();
	// handle key states
	PR_START("INPUT");
	if ( m_KeyStates.keyDown ) {
		m_KeyStates.keyDown = false;
		_stateMachine->onKeyDown(m_KeyStates.keyPressed);
		OnKeyDown(m_KeyStates.keyPressed);
	}
	if ( m_KeyStates.keyUp ) {
		m_KeyStates.keyUp = false;
		_stateMachine->onKeyUp(m_KeyStates.keyReleased);
		OnKeyUp(m_KeyStates.keyReleased);
	}
	if ( m_KeyStates.onChar ) {
		m_KeyStates.onChar = false;
		if (m_KeyStates.ascii >= 0 && m_KeyStates.ascii < 256) {
			_stateMachine->onChar(m_KeyStates.ascii);
			OnChar(m_KeyStates.ascii, 0);
		}
	}
	PR_END("INPUT");
	if ( !m_ButtonState.processed ) {
		m_ButtonState.processed = true;
		if ( m_ButtonState.down ) {
			_stateMachine->onButtonDown(m_ButtonState.button, m_ButtonState.x, m_ButtonState.y);
			OnButtonDown(m_ButtonState.button,m_ButtonState.x,m_ButtonState.y);			
		}
		else {
			DialogID did;
			int selected;
			if (gui->onButtonUp(m_ButtonState.button, m_ButtonState.x, m_ButtonState.y, &did, &selected)) {
				onGUIButton(did, selected);
				_stateMachine->onGUIButton(did, selected);
			}
			else {
				_stateMachine->onButtonUp(m_ButtonState.button, m_ButtonState.x, m_ButtonState.y);
				OnButtonUp(m_ButtonState.button, m_ButtonState.x, m_ButtonState.y);
			}
		}
	}
	sprites::begin();
	if ( m_Running ) {
		_totalTime += m_GameTime.elapsed;
		PR_START("UPDATE");
		PR_START("UPDATE:gui");
		gui->updateMousePos(getMousePos());
		gui->tick(m_GameTime.elapsed);
		PR_END("UPDATE:gui");
		PR_START("Game::update");
		update(m_GameTime.elapsed);
		_stateMachine->update(m_GameTime.elapsed);
		PR_END("Game::update");
		PR_END("UPDATE");
	}
	PR_START("RENDER");
	renderer::beginRendering(_settings.clearColor);	
	renderer::setupMatrices();
	PR_START("RENDER_GAME");
	ds::sprites::begin();		
	_stateMachine->render();
	draw();
	ds::sprites::flush();
	PR_END("RENDER_GAME");
	//m_World.draw();
	PRS("RENDER_GUI");
	gui->render();
	PRE("RENDER_GUI");
	PRS("RENDER_EDITOR");
	if (m_DebugInfo.showEditor) {
		showEditor();
	}
	PRE("RENDER_EDITOR");
	if (m_DebugInfo.performanceOverlay) {
		showPerformceOverlay(&_perfHUDPos);
	}
	PRS("RENDER_FLUSH");
	renderer::flush();
	PRE("RENDER_FLUSH");
	PR_START("DEBUG_RENDER");
#ifdef DEBUG		
	if ( m_DebugInfo.showDrawCounter ) {
		int y = ds::renderer::getSelectedViewport().getHeight() - 80;
		//debug::showDrawCounter(&_perfHUDPos);
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
	
	PR_END("DEBUG_RENDER");
	PR_END("RENDER");
	gui::endFrame();
	profiler::finalize();
	if (m_DebugInfo.monitoring) {
		if (profiler::get_current_total_time() > m_DebugInfo.treshold) {
			char timeFormat[255];
			time_t now;
			time(&now);
			tm *now_tm = localtime(&now);
			strftime(timeFormat, 255, "%Y%m%d_%H%M%S", now_tm);
			char filename[255];
			sprintf_s(filename, "%s.prof", timeFormat);
			FILE* f = fopen(filename, "w");
			renderer::saveDrawCounter(f);
			profiler::save(f);
			world->save(f);
			fclose(f);
			//renderer::printDrawCounter();
			LOG << "performance treshold exceeded: " << profiler::get_current_total_time();
			//LOG << "--------------------------------";
			//profiler::print();			
		}
	}
	renderer::endRendering();
	//profiler::finalize();
	//PR_END("MAIN")
	if ( m_DebugInfo.printProfiler ) {
		m_DebugInfo.printProfiler = false;
		renderer::printDrawCounter();
		LOG << "------------------------------------------------------------";
		gDefaultMemory->debug();
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
		if (m_DebugInfo.showEditor) {
			if (m_Running)  {
				m_Running = false;
			}
		}
		if (!m_DebugInfo.showEditor) {
			if (!m_Running)  {
				m_Running = true;
			}
		}
	}
	else if (virtualKey == VK_F7 && !m_DebugInfo.debugRenderer) {
		m_DebugInfo.debugRenderer = true;
	}
	else if (virtualKey == VK_F8) {
		_stateMachine->activate("ParticlesEditState");
	}
	else if (virtualKey == VK_F9) {
		_stateMachine->activate("SpriteTemplatesState");
	}	
	else if (virtualKey == VK_F11) {
		_stateMachine->activate("DialogEditorState");
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
		float val[16];
		int count = profiler::get_total_times(val, 16);
		float max = 0.0f;
		for (int i = 0; i < 16; ++i) {
			if (val[i] > max) {
				max = val[i];
			}
		}
		max = ceil(max);
		gui::Histogram(val, count, 0.0f, max, 1.0f);
	}
	gui::end();
}

void BaseApp::showEditor() {
	gui::start(EDITOR_ID, &_editor.position);
	if (gui::begin("Game", &_editor.state)) {
		gui::beginGroup();
		if (gui::Button("GSM")) {
			_editor.dialogIndex = 2;
		}
		if (gui::Button("DLG")) {
			m_DebugInfo.showEditor = false;
			_stateMachine->activate("DialogEditorState");
		}
		if (_templatesEditor != 0 && gui::Button("SPT")) {
			//_templatesEditor->init();
			//_editor.dialogIndex = 4;
			m_DebugInfo.showEditor = false;
			_stateMachine->activate("SpriteTemplatesState");
		}
		if (gui::Button("SCG")) {
			_editor.dialogIndex = 5;
		}
		if (gui::Button("BMF")) {
			_editor.dialogIndex = 6;
		}
		if (gui::Button("PS")) {
			m_DebugInfo.showEditor = false;
			_stateMachine->activate("ParticlesEditState");
		}
		gui::endGroup();
	}
	gui::end();

	if (_editor.dialogIndex == 2)  {
		_stateMachine->showDialog();
	}
	if (_editor.dialogIndex == 5) {
		renderer::getSpriteGroupContainer()->showDialog();
	}
	if (_editor.dialogIndex == 6) {
		_bmfDialog->show();
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
			gui::Label(buffer);
		}
	}
	gui::end();
}

void BaseApp::addGameState(GameState* gameState) {
	_stateMachine->add(gameState);
}

void BaseApp::activate(const char* name) {
	_stateMachine->activate(name);
}

void BaseApp::connectGameStates(const char* firstStateName, int outcome, const char* secondStateName) {
	_stateMachine->connect(firstStateName, outcome, secondStateName);
}

}
