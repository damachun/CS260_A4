/* Start Header
*****************************************************************/
/*!
\file assignment.cpp
\author Chloe Lim Jia-Han, 440003018
		Edwin Khoo Bing Shen, 390001918

\par j.lim\@digipen.edu	b.kohh\@digipen.edu
\date 18/7/2021
\brief 	overarching assignment organizational class definition

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

#include "assignment.h"

assignment::assignment(int argc, char** argv):
	_window{}, _renderer{ _window }, _console{ },
	_wsa{ }, _clienthandler{ argc, argv },
	_updategamedata{ false }, _game{ _clienthandler },
	_threads{ }
{
	_threads.push_back(std::thread(&assignment::thread_gameloop, this));
	_threads.push_back(std::thread(&assignment::thread_console, this));
	_threads.push_back(std::thread(&assignment::thread_sockrecvfrom, this));
}
assignment::~assignment()
{
	for (std::thread& thread : _threads)
		thread.join();
}

void assignment::update()
{
	while (run);
}

void assignment::thread_gameloop()
{
	_game.init();

	bool gameend = false;
	while (run && !gameend)
	{
		_window.update();

		{
			_updategamedata = _clienthandler.isupdated();
			gameend = _game.update(_updategamedata);
		}
		_game.render(_renderer);

		_renderer.update();
		_window.updateend();
	}
}
void assignment::thread_console()
{
	_console.update(run);
}
void assignment::thread_sockrecvfrom()
{
	//while (run)
	//{
	//	_updategamedata = _clienthandler.recvfrom();
	//}
}