#include "BaseApp.h"
#include "..\utils\Log.h"
#include <windows.h>
#include "..\utils\Profiler.h"
#include "..\utils\PlainTextReader.h"
#include "..\memory\DataBlockAllocator.h"
#include "..\io\FileWatcher.h"
#include "..\utils\FileUtils.h"
#include "..\particles\ParticleSystem.h"
#include "..\sprites\SpriteObjectDescription.h"

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
	gProfiler = new NewProfiler();
	m_ClearColor = Color(100,149,237,255);
	m_DebugInfo.showProfiler = false;
	m_DebugInfo.printProfiler = false;
	m_DebugInfo.showDrawCounter = false;
	m_DebugInfo.debugRenderer = false;
	m_ButtonState.processed = true;
	m_MousePos = Vector2f(0,0);
	rand.seed(GetTickCount());
	audio = new AudioManager;
	m_Fullscreen = false;
	m_CollisionManager = new SpriteCollisionManager();
	particles = new ParticleManager;
	//m_DialogBatch = 0;
}

// -------------------------------------------------------
// Destructor
// -------------------------------------------------------
BaseApp::~BaseApp() {
	LOGC("BaseApp") << "Destructing all elements";
	delete m_SpriteRenderer;
	delete particles;
	delete m_CollisionManager;
	delete gProfiler;
	delete audio;
	delete renderer;	
	delete gFileWatcher;
	delete gBlockMemory;
	
}

// -------------------------------------------------------
// Init
// -------------------------------------------------------
void BaseApp::init() {
	LOGC("BaseApp") << "----------------- Init ----------------------";
	Settings settings;
	settings.fullscreen = m_Fullscreen;
	settings.height = m_Height;
	settings.width = m_Width;
	settings.synched = true;
	settings.mode = 1;
	settings.postProcessing = false;
	renderer = new Renderer(m_hWnd,settings);   
	m_SpriteRenderer = new SpriteRenderSystem(renderer);
	world.setSpriteRenderer(m_SpriteRenderer);
	audio->initialize(m_hWnd);
	particles->setRenderer(renderer);
	particles->setAssetCompiler(&assets);
	initialize();		
	/*
	LOGC("BaseApp") << "-> loading gui.json";
	if ( file::fileExists("content\\resources\\gui.json")) {
		LOGC("BaseApp") << "There is a gui file";
		initializeGUI();
	}
	*/
	LOGC("BaseApp") << "----------------- Init ----------------------";
	LOGC("BaseApp") << "------------ Loading content  ---------------";
	loadContent();
	LOGC("BaseApp") << "------------ Loading content  ---------------";
	m_Loading = false;
}

// -------------------------------------------------------
// Load sprites
// -------------------------------------------------------
void BaseApp::loadSprites() {
	SpriteDescriptionManager* sdm = new SpriteDescriptionManager;
	assets.load("sprites",sdm,CVT_SPRITES_DESCRIPTION);
	renderer->setDescriptionManager(sdm);
}

void BaseApp::initializeGUI() {
	gui.setRenderer(renderer);
	gui.setAssetCompiler(&assets);
	assets.load("content\\resources","gui",&gui,CVT_GUI);	
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
		LOGC("BaseApp") << "Failed to created window";
		//ErrorExit(TEXT("CreateWindow"));
		ExitProcess(dw); 
	}

	// Adjust to desired size
	RECT rect = { 0, 0, m_Width, m_Height };
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
	m_GameTime.totalTime += m_GameTime.elapsedMillis;
}

// -------------------------------------------------------
// Build frame
// -------------------------------------------------------
void BaseApp::buildFrame() {	
	gProfiler->reset();	
	m_CollisionManager->reset();
	PR_START("MAIN")
	PR_START("FILEWATCHER")
#ifdef DEBUG
	assets.update();
#endif
	PR_END("FILEWATCHER")
	renderer->getDrawCounter().reset();
	renderer->clearDebugMessages();
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
		if ( !gui.OnChar(m_KeyStates.ascii,0) ) {
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
			if ( gui.onButtonDown(m_ButtonState.button,m_ButtonState.x,m_ButtonState.y,&did,&selected) ) {
				onGUIButton(did,selected);
			}
		}
		else {
			OnButtonUp(m_ButtonState.button,m_ButtonState.x,m_ButtonState.y);
		}
	}
	PR_START("UPDATE")
	gui.updateMousePos(getMousePos());
	PR_START("GameObjects::update")
	//if ( !m_Paused ) {
		for ( size_t i = 0; i < m_GameObjects.size(); ++i ) {
			GameObject* obj = m_GameObjects[i];
			obj->resetEvents();
			if (obj->isActive()) {				
				obj->update(m_GameTime.elapsed);
			}
		}
	//}
	PR_END("GameObjects::update")
	PR_START("Game::update")
	update(m_GameTime);
	world.update(m_GameTime.elapsed);
	particles->update(m_GameTime.elapsed);
	PR_END("Game::update")
	int collisions = m_CollisionManager->checkIntersections();
	if ( collisions > 0 ) {
		handleCollisions();
	}
	PR_END("UPDATE")
	PR_START("RENDER")
	renderer->beginRendering(m_ClearColor);	
	renderer->setupMatrices();
	PR_START("RENDER_GAME")
	draw(m_GameTime);
	//m_World.draw();
	gui.render();
	PR_END("RENDER_GAME")
	PR_START("DEBUG_RENDER")
#ifdef DEBUG		
	if ( m_DebugInfo.showDrawCounter ) {
		int y = renderer->getViewport()->getHeight() - 80;
		renderer->showDrawCounter(10,y);
	}
	if ( m_DebugInfo.showProfiler ) {
		renderer->showProfiler(10,10);
	}		
#endif
	renderer->drawDebugMessages();
	PR_END("DEBUG_RENDER")
	PR_END("RENDER")		
	renderer->endRendering();	
	PR_END("MAIN")
	if ( m_DebugInfo.printProfiler ) {
		m_DebugInfo.printProfiler = false;
		renderer->printDrawCounter();
		gProfiler->print();
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
#ifdef DEBUG
	if ( ascii == 'p' ) {
		gProfiler->print();
	}
	if ( ascii == 'o' ) {
		gBlockMemory->debug();
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

void BaseApp::createParticleSystem(const char* fileName,int textureID,ParticleSystem* entity,int maxParticles,int blendState) {
	entity->setRenderer(renderer);
	entity->setMaxParticles(maxParticles);
	entity->setBlendState(blendState);
	entity->setTextureID(textureID);
	entity->init();
	assets.load("content\\resources",fileName,entity,CVT_PARTICLESYSTEM);
	//entity->load(fileName);	
	m_GameObjects.push_back(entity);
}

}
