/* Start Header
*****************************************************************/
/*!
\file assignment.h
\author Chloe Lim Jia-Han, 440003018
		Edwin Khoo Bing Shen, 390001918
		Javier Foo, 440002318

\par j.lim\@digipen.edu	b.khoo\@digipen.edu javier.foo\@digipen.edu
\date 18/7/2021
\brief 	overarching assignment organizational class declaration

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

#pragma once
#include "game.h"

class assignment
{
	// the following excludes the main thread
	// mainthread will be console
	enum class extrathreadsorganization : int
	{
		GAMELOOP = 0,
		CONSOLE,
		SOCKRECVFROM
	};

	// basic applications
	window   _window;
	renderer _renderer;
	console  _console;

	// send and recv
	wrapWSADATA    _wsa; // figure out where to move this
	ClientsHandler _clienthandler;

	// game system
	bool       _updategamedata;
	game       _game;

	// threads and thread functions
	std::vector <std::thread> _threads;
	void thread_gameloop();
	void thread_console();
	void thread_sockrecvfrom();

public:
	assignment(int argc, char** argv);
	~assignment();

	void update();
};