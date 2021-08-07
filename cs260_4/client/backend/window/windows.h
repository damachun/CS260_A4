/* Start Header
*****************************************************************/
/*!
\file windows.h
\author Chloe Lim Jia-Han, 440003018
		Edwin Khoo Bing Shen, 390001918

\par j.lim\@digipen.edu	b.kohh\@digipen.edu
\date 20/6/2021
\brief 	the window class declaration

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

#pragma once
#include "../globalhelpers.h"
#include <Windows.h>

extern bool run;
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL WINAPI CtrlHandler(DWORD fdwCtrlType);
HWND MakeWindow(char* title, int x, int y, int width, int height, BYTE type, DWORD flags);

class window
{
public:
	const bool& updatecheck();
	void update();
	void updateend();

	void setisrunning(const bool& isRunning);

	int getwindowwidth();
	int getwindowheight();

	window();
	~window();

private:

	HDC   hDC;				/* device context */
	HGLRC hRC;				/* opengl context */
	HWND  hWnd;				/* window */
	MSG   msg = { 0 };		/* message */

	int         pf;
	WNDCLASS    wc;
	PIXELFORMATDESCRIPTOR pfd{ 0 };
	HINSTANCE hInstance;

	bool m_running = true;
};
#define WINDOWS Window::Singleton()