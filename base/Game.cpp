#include "Game.h"

namespace ds {

Game::Game(void) {
	m_AudioManager = new AudioManager();		
	rand.seed(GetTickCount());
}


Game::~Game(void) {
	
	delete m_AudioManager;
}

void Game::init(HWND hWnd) {
	m_AudioManager->initialize(hWnd);
}
}
