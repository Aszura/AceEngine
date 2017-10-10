#include "Game.h"

#include <Windows.h>
#include <direct.h>
#include <gl/gl.h>
#include <string>
#include <atlstr.h>
#include <shellapi.h>

#include "InputCallback.h"
#include "Window.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int iCmdShow)
{
	LPWSTR* szArglist;
	int nArgs;
	szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
	if (NULL == szArglist)
	{
		wprintf(L"CommandLineToArgvW failed\n");
		return 0;
	}

	//Change current running path to executable's path
	std::wstring execPath(szArglist[0]);
	std::wstring execDirectory = execPath.substr(0, execPath.find_last_of(L"\\/"));
	int i = _wchdir(execDirectory.c_str());

	WNDCLASS wc;
	HWND     hWnd;
	HDC      hDC;
	HGLRC    hRC;
	MSG      msg;
	BOOL     quit = FALSE;

	// register window class
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = L"GLSample";
	RegisterClass(&wc);

	// create main window
	hWnd = CreateWindow(
		L"GLSample", L"OpenGL Sample",
		WS_CAPTION | WS_POPUPWINDOW | WS_VISIBLE,
		0, 0, 256, 256,
		NULL, NULL, hInstance, NULL);

	// create callbacks for windows input
	input::InputCallback inputCallback;
	inputCallback.keyCallback = GetAsyncKeyState;
	inputCallback.mousePosCallback = [](long& posX, long& posY)
	{
		POINT mousePos;

		GetCursorPos(&mousePos);
		posX = mousePos.x;
		posY = mousePos.y;
	};

	rendering::Window windowData (&hWnd, &hDC, &hRC);

	// create player for game
	game::Game* game = new game::Game(windowData, inputCallback);
	game->start();

	// program main loop
	while (!quit)
	{

		// check for messages
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{

			// handle or dispatch messages
			if (msg.message == WM_QUIT)
			{
				quit = TRUE;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

		}
		else
		{
			game->run();
		}

	}

	// shutdown OpenGL
	game->stop();
	delete game;

	// destroy the window explicitly
	DestroyWindow(hWnd);

	LocalFree(szArglist);

	return msg.wParam;
}

// Window Procedure

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch (message)
	{

	case WM_CREATE:
		return 0;

	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;

	case WM_DESTROY:
		return 0;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			return 0;

		}
		return 0;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);

	} // switch

}
