#pragma once

#include "..\dxstdafx.h"

namespace ds {

class Input	{

public:
	Input(HWND hwnd);
	virtual ~Input();

	bool initialize() { return true; }
	void shutdown() { }
	void update(float elapsed);
	
	bool GetMouseButton( int button );
	bool isClicked(int button);
	char GetKeyState(int key) { return keyState[key]; }
	bool KeyDown(char key);// { return (keyState[key] & 0x80) != 0; }
	long GetPosX() { return position.x; }
	long GetPosY() { return position.y; }
	long GetDeltaX() { return mouseState.lX; }
	long GetDeltaY() { return mouseState.lY; }
	long GetDeltaWheel() { return mouseState.lZ; }

private:
	HWND window; 
	IDirectInput8 *di; 
	IDirectInputDevice8 *keyboard;
	char keyState[256]; 
	IDirectInputDevice8 *mouse; 
	DIMOUSESTATE mouseState; 
	POINT position; 
};

};
