#include "..\dxstdafx.h"
#include "BaseApp.h"
#include "..\utils\Log.h"
#include <strsafe.h>
/*
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
*/
extern ds::BaseApp *app;

#define GETX(l) (int(l & 0xFFFF))
#define GETY(l) (int(l) >> 16)

bool active = true;

void ErrorExit(LPTSTR lpszFunction) 
{ 
	// Retrieve the system error message for the last-error code

	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError(); 

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &lpMsgBuf,
		0, NULL );

	// Display the error message and exit the process

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
		(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR)); 
	StringCchPrintf((LPTSTR)lpDisplayBuf, 
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s failed with error %d: %s"), 
		lpszFunction, dw, lpMsgBuf); 
	MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK); 

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
	ExitProcess(dw); 
}

LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){	
	switch (message){
		case WM_PAINT:
			if ( app->isLoading()) {
				HDC hDC = GetDC( hwnd );
				TCHAR strMsg[MAX_PATH];
				wsprintf( strMsg, TEXT("Loading data - Please wait") );
				RECT rct;
				GetClientRect( hwnd, &rct );
				SetTextColor(hDC, RGB(255,255,255));
				SetBkMode(hDC,TRANSPARENT);
				DrawText( hDC, strMsg, -1, &rct, DT_CENTER|DT_VCENTER|DT_SINGLELINE );
				ReleaseDC( hwnd, hDC );
			}
			break;
		case WM_MOUSEMOVE:
			int x, y;
			x = GETX(lParam);
			y = GETY(lParam);
			app->setMousePos(x, y);
			break;
		case WM_CHAR: {
				char ascii = wParam;
				unsigned int keyState = lParam;
				app->sendOnChar(ascii,keyState);
			}
			return 0;
		case WM_KEYDOWN:        			
			app->sendKeyDown( wParam );
			return 0;
		case WM_KEYUP:        
			app->sendKeyUp( wParam );
			return 0;		
		case WM_LBUTTONDOWN:
			app->sendButton(0,GETX(lParam), GETY(lParam),true);
 			break;
		case WM_LBUTTONUP:
			app->sendButton(0,GETX(lParam), GETY(lParam),false);
			break;
		case WM_RBUTTONDOWN:
			app->sendButton(1,GETX(lParam), GETY(lParam),true);
 			break;
		case WM_RBUTTONUP:
			app->sendButton(1,GETX(lParam), GETY(lParam),false);
			break;
		case WM_MOUSEWHEEL:
			static int scroll;
			int s;

			scroll += GET_WHEEL_DELTA_WPARAM(wParam);
			s = scroll / WHEEL_DELTA;
			scroll %= WHEEL_DELTA;

			POINT point;
			point.x = GETX(lParam);
			point.y = GETY(lParam);
			ScreenToClient(hwnd, &point);

			//if (s != 0) app->onMouseWheel(point.x, point.y, s);
			break;
		case WM_SIZE:
			//app->onSize(LOWORD(lParam), HIWORD(lParam));
			break;
		case WM_WINDOWPOSCHANGED:
			if ((((LPWINDOWPOS) lParam)->flags & SWP_NOSIZE) == 0){
				RECT rect;
				GetClientRect(hwnd, &rect);
				int w = rect.right - rect.left;
				int h = rect.bottom - rect.top;
				//if (w > 0 && h > 0) app->onSize(w, h);
			}
			break;
		case WM_SYSKEYDOWN:
			/*
			if ((lParam & (1 << 29)) && wParam == KEY_ENTER){
				app->toggleFullscreen();
			} else {
				app->onKey((unsigned int) wParam, true);
			}
			*/
			break;
		case WM_SYSKEYUP:
			//app->onKey((unsigned int) wParam, false);
			break;
		case WM_CREATE:
			ShowWindow(hwnd, SW_SHOW);
			break;
		case WM_CLOSE:
			LOG << "WM_CLOSE";
			app->shutdown();
			break;
		case WM_DESTROY:
			LOG << "WM_DESTROY";
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hwnd, message, wParam, lParam);
		}
	return DefWindowProc(hwnd, message, wParam, lParam);
}
/*
#ifdef _DEBUG
#include <crtdbg.h>
#endif
*/
#include <stdio.h>

int WINAPI WinMain(HINSTANCE hThisInst, HINSTANCE hLastInst, LPSTR lpszCmdLine, int nCmdShow){
	
#ifdef _DEBUG
	int flag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG); // Get current flag
	flag |= _CRTDBG_LEAK_CHECK_DF; // Turn on leak-checking bit
	flag |= _CRTDBG_CHECK_ALWAYS_DF; // Turn on CrtCheckMemory
	//flag |= _CRTDBG_DELAY_FREE_MEM_DF;
	_CrtSetDbgFlag(flag); // Set flag to the new value
#endif

	WNDCLASS wincl;
	wincl.hInstance = hThisInst;
	wincl.lpszClassName = "Diesel";
	wincl.lpfnWndProc = WinProc;
	wincl.style = 0;
	wincl.hIcon = LoadIcon( NULL, IDI_APPLICATION);
	wincl.hCursor = LoadCursor( NULL, IDC_ARROW );
	wincl.lpszMenuName = NULL;
	wincl.cbClsExtra = 0;
	wincl.cbWndExtra = 0;
	wincl.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	if (!RegisterClass(&wincl)) {
		ErrorExit("RegisterClass");
		return 0;
	}
	app->setInstance(hThisInst);
	app->createWindow();
	app->prepare();
	// Initialize timer
	app->initTimer();
	/*
		Force the main thread to always run on CPU 0.
		This is done because on some systems QueryPerformanceCounter returns a bit different counter values
		on the different CPUs (contrary to what it's supposed to do), which can cause negative frame times
		if the thread is scheduled on the other CPU in the next frame. This can cause very jerky behavior and
		appear as if frames return out of order.
	*/
	SetThreadAffinityMask(GetCurrentThread(), 1);
	MSG msg;
    while ( !app->isDone() ) {
        // Did we receive a message, or are we idling ?
        if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )  {
            if ( msg.message == WM_QUIT) {
                break;
            }
            TranslateMessage( &msg );
            DispatchMessage ( &msg );
        } 
        else  {						
			app->updateTime();            
			app->buildFrame();        
        } 
    }
	
	delete app;
	//_CrtDumpMemoryLeaks();
	return (int) msg.wParam;
}