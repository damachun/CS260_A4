/* Start Header
*****************************************************************/
/*!
\file windows.h
\author Chloe Lim Jia-Han, 440003018
		Edwin Khoo Bing Shen, 390001918

\par j.lim\@digipen.edu	b.kohh\@digipen.edu
\date 20/6/2021
\brief 	the main function

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

#include "backendwrap.h"

int main()
{
	window   mainWindow;
	renderer mainRenderer(mainWindow);
	console  mainConsole;
	
	auto gameupdatefn = [&mainWindow, &mainRenderer]() -> void
	{
		while (mainWindow.updatecheck())
		{
			mainWindow.update();

			mainRenderer.update();

			mainWindow.updateend();
		}
	};
	
	std::thread gamethread{ gameupdatefn };
	mainConsole.update(run);
	gamethread.join();

	return 0;
}