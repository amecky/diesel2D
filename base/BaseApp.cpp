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
#include "..\io\ReportWriter.h"
#include "..\ui\GameConsole.h"
#include "..\editor\AssetEditor.h"
#include "..\utils\GlobalStringBuffer.h"

namespace ds {

// -------------------------------------------------------
// Constructing new BaseApp
// -------------------------------------------------------	
BaseApp::BaseApp() {
	_bitmapFontEditor = 0;
	_spriteTemplatesEditor = 0;
	_particlesEditor = 0;
	_dialogEditor = 0;
	gDefaultMemory = new DefaultAllocator(256 * 1024 * 1024);
	gStringBuffer = new GlobalStringBuffer();
	perf::init();
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
	m_DebugInfo.showConsole = false;
	m_ButtonState.processed = true;
	m_MousePos = Vector2f(0,0);
	rand.seed(GetTickCount());
	audio = new AudioManager();
	_totalTime = 0.0f;
	_stateMachine = new GameStateMachine();
	particles = new ParticleManager();
	gui = new DialogManager();
	world = new World();
	console = new GameConsole();
	editor::init();
	//_templatesEditor = 0;
	_perfHUDPos = v2(10, 710);
	_prepared = false;
	JSONReader reader;
	bool ret = reader.parse("content\\engine_settings.json");
	assert(ret);
	int c = reader.find_category("basic_settings");
	assert(c != -1);
	reader.get_int(c, "screen_width", &_settings.screenWidth);
	reader.get_int(c, "screen_height", &_settings.screenHeight);
	reader.get_color(c, "clear_color", &_settings.clearColor);
	if (reader.contains_property(c, "reporting_directory")) {
		const char* dir = reader.get_string(c, "reporting_directory");
		sprintf_s(_settings.reportingDirectory, 64, "%s\\", dir);
	}
	else {
		sprintf_s(_settings.reportingDirectory, "");
	}
	_reload_counter = 0;
}

// -------------------------------------------------------
// Destructor
// -------------------------------------------------------
BaseApp::~BaseApp() {
	LOG << "Destructing all elements";
	sprites::shutdown();
	repository::shutdown();
	editor::shutdown();
	if (_bitmapFontEditor != 0) {
		delete _bitmapFontEditor;
	}
	if (_spriteTemplatesEditor != 0) {
		delete _spriteTemplatesEditor;
	}
	if (_particlesEditor != 0) {
		delete _particlesEditor;
	}
	if (_dialogEditor != 0) {
		delete _dialogEditor;
	}
	delete console;
	delete world;
	delete gui;
	delete particles;
	delete _stateMachine;
	delete audio;
	renderer::shutdown();	
	gui::shutdown();
	perf::shutdown();
	delete gStringBuffer;
	gDefaultMemory->printOpenAllocations();
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
			int texture = renderer::loadTexture(textureNames);
			assert(texture != -1);
		}
		else {
			Array<std::string> values;
			string::split(textureNames,values);
			for (size_t i = 0; i < values.size(); ++i) {
				int texture = renderer::loadTexture(values[i].c_str());
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
		editor::add(_stateMachine);
		_bitmapFontEditor = new BitmapFontsDialog();
		_spriteTemplatesEditor = new SpriteTemplatesEditor(renderer::getSpriteTemplates());
		_particlesEditor = new ParticlesEditState(particles);
		_dialogEditor = new DialogEditorState(gui);

		editor::add(_bitmapFontEditor);
		editor::add(_spriteTemplatesEditor);
		editor::add(_particlesEditor);
		editor::add(_dialogEditor);
	}

	loadShaders(reader);

	loadDescriptors(reader);
}
// -------------------------------------------------------
// Init
// -------------------------------------------------------
void BaseApp::prepare() {	
	assert(!_prepared);
	TIMER("Load Settings")
	LOG << "---> Init <---";
	LOG << "---- Loading settings ----";
	renderer::initialize(m_hWnd,_settings);   	
	audio->initialize(m_hWnd);

	profiler::init();
	sprites::init();
	world->setDefaultDescriptor(sprites::getDescriptorID());
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
	//_perfHUDPos = dp;
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
	perf::reset();
	//PR_START("MAIN")
	renderer::drawCounter().reset();
	++_reload_counter;
	if (_reload_counter >= 60) {
		_reload_counter -= 60;
		repository::reload();
	}
	// handle key states
	{
		ZoneTracker z("INPUT");
		if (m_KeyStates.keyDown) {
			m_KeyStates.keyDown = false;
			_stateMachine->onKeyDown(m_KeyStates.keyPressed);
			OnKeyDown(m_KeyStates.keyPressed);
		}
		if (m_KeyStates.keyUp) {
			m_KeyStates.keyUp = false;
			_stateMachine->onKeyUp(m_KeyStates.keyReleased);
			OnKeyUp(m_KeyStates.keyReleased);
		}
		if (m_KeyStates.onChar) {
			m_KeyStates.onChar = false;
			if (m_KeyStates.ascii >= 0 && m_KeyStates.ascii < 256) {
				_stateMachine->onChar(m_KeyStates.ascii);
				OnChar(m_KeyStates.ascii, 0);
			}
		}
	}
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
		ZoneTracker u1("UPDATE");
		{
			{
				ZoneTracker u2("UPDATE:gui");
				gui->updateMousePos(getMousePos());
				gui->tick(m_GameTime.elapsed);
			}
			{
				ZoneTracker u3("Game::update");
				update(m_GameTime.elapsed);
				_stateMachine->update(m_GameTime.elapsed);
			}
			world->tick(m_GameTime.elapsed);
		}
	}
	{
		ZoneTracker r1("RENDER");
		renderer::beginRendering(_settings.clearColor);
		renderer::setupMatrices();
		{
			ZoneTracker r("RENDER_GAME");
			sprites::begin();
			_stateMachine->render();
			draw();
			sprites::flush();
		}
		{
			ZoneTracker r("RENDER_GUI");
			gui->render();
		}
		{
			ZoneTracker r("RENDER_ASSET_EDITORS");
			editor::render();
		}
		if (m_DebugInfo.performanceOverlay) {
			showPerformceOverlay(&_perfHUDPos);
		}
		if (m_DebugInfo.showConsole) {
			console->show();
		}
		{
			ZoneTracker r("RENDER_FLUSH");
			sprites::flush();
			renderer::flush();
		}
		{
			ZoneTracker r("DEBUG_RENDER");
#ifdef DEBUG		
			if (m_DebugInfo.showDrawCounter) {
				int y = renderer::getSelectedViewport().getScreenSize().y - 80;
				renderer::showDrawCounter(&_perfHUDPos);
			}
			if (m_DebugInfo.showProfiler) {
				++m_DebugInfo.profilerTicks;
				if (m_DebugInfo.profilerTicks > 10) {
					m_DebugInfo.profilerTicks = 0;
					m_DebugInfo.snapshotCount = profiler::get_snapshot(_snapshots, 64);
				}
				showProfilerSnapshot(&_perfHUDPos);
			}
#endif

		}
		gui::endFrame();
	}	
	profiler::finalize();
	perf::finalize();
	if (m_DebugInfo.monitoring) {
		if (perf::get_current_total_time() > m_DebugInfo.treshold) {
			saveHTMLProfilingReport();
			LOG << "performance treshold exceeded: " << perf::get_current_total_time();
		}
	}
	renderer::endRendering();
	if ( m_DebugInfo.printProfiler ) {
		m_DebugInfo.printProfiler = false;
		renderer::printDrawCounter();
		LOG << "------------------------------------------------------------";
		gDefaultMemory->debug();
		profiler::print();
		saveHTMLProfilingReport(); 
	}	
}

// -------------------------------------------------------
// Save profiling report
// -------------------------------------------------------
void BaseApp::saveHTMLProfilingReport() {
	char timeFormat[255];
	time_t now;
	time(&now);
	tm *now_tm = localtime(&now);
	strftime(timeFormat, 255, "%Y%m%d_%H%M%S", now_tm);
	char filename[255];
	sprintf_s(filename, "%s%s.html", _settings.reportingDirectory, timeFormat);
	ReportWriter rw(filename);
	renderer::saveDrawCounter(rw);
	perf::save(rw);
	world->save(rw);
	gDefaultMemory->save(rw);
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
	if (!m_Running && !down) {
		int ny = renderer::getScreenHeight() - y;
		SID sid = world->pick(v2(x, ny));
		if (sid != INVALID_SID) {
			LOG << "------------------------------------------";
			world->debug(sid);
			LOG << "------------------------------------------";
		}
	}
}

// -------------------------------------------------------
// Key message handling
// -------------------------------------------------------
void BaseApp::sendOnChar(char ascii,unsigned int state) {
	m_KeyStates.ascii = ascii;
	m_KeyStates.onChar = true;
	gui::sendKey(ascii);
}

// -------------------------------------------------------
// send key down
// -------------------------------------------------------
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
		editor::toggle();
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
	else if (virtualKey == VK_F12) {
		m_DebugInfo.showConsole = !m_DebugInfo.showConsole;
	}
#endif
}

// -------------------------------------------------------
// send key up
// -------------------------------------------------------
void BaseApp::sendKeyUp(WPARAM virtualKey) {
	m_KeyStates.keyUp = true;
	m_KeyStates.keyReleased = virtualKey;
	gui::sendSpecialKey(virtualKey);
}

// -------------------------------------------------------
// show performance overlay FIXME: broken
// -------------------------------------------------------
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

// -------------------------------------------------------
// show editor
// -------------------------------------------------------
/*
void BaseApp::showEditor() {
	gui::start(EDITOR_ID, &_editor.position);
	if (gui::begin("Game", &_editor.state)) {
		gui::beginGroup();
		if (gui::Button("GSM")) {
			_editor.dialogIndex = 2;
		}
		if (gui::Button("SCG")) {
			_editor.dialogIndex = 5;
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
}
*/
// -------------------------------------------------------
// show profiler snapshot FIXME: broken
// -------------------------------------------------------
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

// -------------------------------------------------------
// add game state
// -------------------------------------------------------
void BaseApp::addGameState(GameState* gameState) {
	_stateMachine->add(gameState);
}

// -------------------------------------------------------
// activate game state
// -------------------------------------------------------
void BaseApp::activate(const char* name) {
	_stateMachine->activate(name);
}

// -------------------------------------------------------
// connect game states
// -------------------------------------------------------
void BaseApp::connectGameStates(const char* firstStateName, int outcome, const char* secondStateName) {
	_stateMachine->connect(firstStateName, outcome, secondStateName);
}

// -------------------------------------------------------
// load shaders
// -------------------------------------------------------
void BaseApp::loadShaders(const JSONReader& reader) {
	int root = reader.find_category("shaders");
	if(root != -1) {
		int entries[128];
		int num = reader.get_categories(entries,128,root);
		for (int i = 0; i < num; ++i) {
			const char* file = reader.get_string(entries[i], "file");
			const char* tech = reader.get_string(entries[i], "tech");
			int sh = renderer::loadShader(file, tech);
			assert(sh >= 0);
			LOG << "shader '" << file << "' id: " << sh;
		}
	}
	else {
		LOG << "No shaders defined";
	}
}

// -------------------------------------------------------
// load descriptors
// -------------------------------------------------------
void BaseApp::loadDescriptors(const JSONReader& reader) {
	int root = reader.find_category("descriptors");
	if (root != -1) {
		int entries[128];
		int num = reader.get_categories(entries, 128, root);
		for (int i = 0; i < num; ++i) {
			const char* texture = reader.get_string(entries[i], "texture");
			const char* shader = reader.get_string(entries[i], "shader");
			const char* blend_state = reader.get_string(entries[i], "blend_state");
			Descriptor desc;
			desc.hash = string::murmur_hash(reader.get_category_name(entries[i]));
			desc.shader = renderer::getShaderID(string::murmur_hash(shader));
			assert(desc.shader != 0);
			desc.texture = renderer::getTextureId(texture);
			int did = renderer::addDescriptor(desc);
			LOG << "descriptor: '" << reader.get_category_name(entries[i]) << "' id: " << did;
		}
	}
	else {
		LOG << "No descriptors defined";
	}
}

// -------------------------------------------------------
// load viewports
// -------------------------------------------------------
/*
viewports {
default {
screen_size : 1280,720
world_size : 1280,720
}
main {
screen_size : 1280,720
world_size : 1600,900
}
}
*/
void BaseApp::loadViewports(const JSONReader& reader) {
	int root = reader.find_category("viewports");
	if (root != -1) {
		int entries[128];
		int num = reader.get_categories(entries, 128, root);
		for (int i = 0; i < num; ++i) {
			const char* texture = reader.get_string(entries[i], "texture");
			const char* shader = reader.get_string(entries[i], "shader");
			const char* blend_state = reader.get_string(entries[i], "blend_state");
			Descriptor desc;
			desc.hash = string::murmur_hash(reader.get_category_name(entries[i]));
			desc.shader = renderer::getShaderID(string::murmur_hash(shader));
			assert(desc.shader != 0);
			desc.texture = renderer::getTextureId(texture);
			int did = renderer::addDescriptor(desc);
		}
	}
	else {
		LOG << "No descriptors defined";
	}
}

}
