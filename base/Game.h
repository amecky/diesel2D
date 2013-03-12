#pragma once
#include "..\audio\AudioManager.h"
#include "..\utils\mtrand.h"

namespace ds {

class Game {

public:
	Game(void);
	~Game(void);
	void init(HWND hWnd);
	AudioManager& audio() const {
		return *m_AudioManager;
	}
	float random(float min,float max) {
		return min + (max - min)* (float)rand();
	}	
private:
	AudioManager* m_AudioManager;
	MTRand_open rand;	
};

}
