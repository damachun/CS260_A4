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
	std::cout << "Compiles" << std::endl;

	window mainWindow;
	renderer mainRenderer(mainWindow);

	while (mainWindow.updatecheck())
	{
		mainWindow.update();

		mainRenderer.update();

		mainWindow.updateend();
	}

	return 0;
}