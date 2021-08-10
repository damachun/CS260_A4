/* Start Header
*****************************************************************/
/*!
\file windows.h
\author Chloe Lim Jia-Han, 440003018
		Edwin Khoo Bing Shen, 390001918
		Javier Foo, 440002318

\par j.lim\@digipen.edu	b.khoo\@digipen.edu javier.foo\@digipen.edu
\date 18/7/2021
\brief 	the window class declaration

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

#pragma once
#include "../baseincludes.h"

extern bool run;
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL WINAPI CtrlHandler(DWORD fdwCtrlType);
HWND MakeWindow(char* title, int x, int y, int width, int height, BYTE type, DWORD flags);

class window
{
	HDC   _hDC;				/* device context */
	HGLRC _hRC;				/* opengl context */
	HWND  _hWnd;			/* window */
	MSG   _msg = { 0 };		/* message */

	int         _pf;
	WNDCLASS    _wc;

	PIXELFORMATDESCRIPTOR _pfd{ 0 };
	HINSTANCE             _hInstance;

public:
	const bool& updatecheck();
	void update();
	void updateend();

	int getwindowwidth();
	int getwindowheight();

	window();
	~window();
};
#define WINDOWS Window::Singleton()