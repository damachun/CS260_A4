/* Start Header
*****************************************************************/
/*!
\file windows.cpp
\author Chloe Lim Jia-Han, 440003018
		Edwin Khoo Bing Shen, 390001918
		Javier Foo, 440002318

\par j.lim\@digipen.edu	b.khoo\@digipen.edu javier.foo\@digipen.edu
\date 18/7/2021
\brief 	the window class definition

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

#include "windows.h"

#pragma once
#include <stdlib.h>
#include <stdio.h>
#include "wtypes.h"

bool run = true;

std::wstring strtowstr(const std::string& word)
{
	/*std::wstring wword;
	wword.resize(word.length() + 1);
	size_t retval = 0;

	mbstowcs_s(&retval, &wword[0], wword.size(), word.c_str(), word.length());

	return wword;*/

	return std::wstring(word.begin(), word.end());
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static PAINTSTRUCT ps;

	switch (uMsg)
	{
	case WM_PAINT:
		BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		return 0;
	case WM_SIZE:
		PostMessage(hWnd, WM_PAINT, 0, 0);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_KEYDOWN:
		//F11
		if (wParam == VK_F11)
		{
			if (GetWindowLongPtr(hWnd, GWL_STYLE) & WS_POPUP)
			{
				SetWindowLongPtr(hWnd, GWL_STYLE, WS_VISIBLE | WS_OVERLAPPEDWINDOW);
				SetWindowPos(hWnd, NULL, 0, 0, 1024, 720, SWP_FRAMECHANGED);
			}
			else
			{ // set full screen
				SetWindowLongPtr(hWnd, GWL_STYLE, WS_VISIBLE | WS_POPUP);
				SetWindowPos(hWnd, HWND_TOP, 0, 0, GetSystemMetrics(SM_CXSCREEN),
					GetSystemMetrics(SM_CYSCREEN), SWP_FRAMECHANGED);
			}
		}
		return 0;
	case WM_CLOSE:
		//Window::Singleton().SetIsRunning(false);
		run = false;
		return 0;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
{
	switch (fdwCtrlType)
	{
		// Handle the CTRL-C signal. 
	case CTRL_C_EVENT:
		printf("Ctrl-C event\n\n");
		return TRUE;

		// CTRL-CLOSE: confirm that the user wants to exit. 
	case CTRL_CLOSE_EVENT:
		printf("Ctrl-Close event\n\n");
		return FALSE;

		// Pass other signals to the next handler. 
	case CTRL_BREAK_EVENT:
		printf("Ctrl-Break event\n\n");
		return FALSE;

	case CTRL_LOGOFF_EVENT:
		printf("Ctrl-Logoff event\n\n");
		return FALSE;

	case CTRL_SHUTDOWN_EVENT:
		printf("Ctrl-Shutdown event\n\n");
		return FALSE;

	default:
		return FALSE;
	}
}
HWND MakeWindow(char* title, int x, int y, int width, int height, BYTE type, DWORD flags)
{
	int         pf;
	HDC         hDC;
	HWND        hWnd;
	WNDCLASS    wc;
	PIXELFORMATDESCRIPTOR pfd{ 0 };
	static HINSTANCE hInstance = 0;

	/* only register the window class once - use hInstance as a flag. */
	if (!hInstance)
	{
		hInstance = GetModuleHandle(NULL);
		wc.style = CS_OWNDC;
		wc.lpfnWndProc = (WNDPROC)WndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = hInstance;
		wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = NULL;
		wc.lpszMenuName = NULL;
		wc.lpszClassName = LPCWSTR(L"CS260 A4");

		if (!RegisterClass(&wc))
		{
			MessageBox(NULL, 
				LPCWSTR(L"RegisterClass() failed:  Cannot register window class."),
				LPCWSTR(L"Error"), MB_OK);
			return NULL;
		}
	}

	hWnd = CreateWindow(strtowstr("CS260 A4").c_str(), strtowstr(title).c_str(),
		WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		x, y, width, height, NULL, NULL, hInstance, NULL);

	if (hWnd == NULL)
	{
		MessageBox(NULL, LPCWSTR(L"CreateWindow() failed:  Cannot create a window."),
			LPCWSTR(L"Error"), MB_OK);
		return NULL;
	}

	hDC = GetDC(hWnd);

	memset(&pfd, 0, sizeof(pfd));
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | flags | PFD_GENERIC_ACCELERATED | PFD_DOUBLEBUFFER;
	pfd.iPixelType = type;
	pfd.cColorBits = 32;

	pf = ChoosePixelFormat(hDC, &pfd);
	if (pf == 0)
	{
		MessageBox(NULL, LPCWSTR(L"ChoosePixelFormat() failed:  Cannot find a suitable pixel format."),
			LPCWSTR(L"Error"), MB_OK);
		return 0;
	}

	if (SetPixelFormat(hDC, pf, &pfd) == FALSE)
	{
		MessageBox(NULL, LPCWSTR(L"SetPixelFormat() failed:  Cannot set format specified."),
			LPCWSTR(L"Error"), MB_OK);
		return 0;
	}

	DescribePixelFormat(hDC, pf, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

	return hWnd;
}

window::window()
{
	_hWnd = MakeWindow(const_cast<char*>("CS260 A4"), 0, 0, 800, 800, 0, 0);
	if (_hWnd == NULL) exit(1);

	// Create the OpenGL context
	_hDC = GetDC(_hWnd);
	_hRC = wglCreateContext(_hDC);
	wglMakeCurrent(_hDC, _hRC);

	ShowWindow(_hWnd, SW_SHOW);
}

window::~window()
{
	wglMakeCurrent(NULL, NULL);
	ReleaseDC(_hWnd, _hDC);
	wglDeleteContext(_hRC);
	DestroyWindow(_hWnd);
}

const bool& window::updatecheck()
{
	return run;
}
void window::updateend()
{
	SwapBuffers(_hDC);
}
void window::update()
{
	while (PeekMessage(&_msg, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&_msg);
		DispatchMessage(&_msg);
	}
}

int window::getwindowwidth()
{
	RECT rect;
	GetWindowRect(_hWnd, &rect);
	return rect.right - rect.left;
}
int window::getwindowheight()
{
	RECT rect;
	GetWindowRect(_hWnd, &rect);
	return rect.bottom - rect.top;
}