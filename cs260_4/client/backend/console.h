/* Start Header
*****************************************************************/
/*!
\file windows.h
\author Chloe Lim Jia-Han, 440003018
		Edwin Khoo Bing Shen, 390001918
		Javier Foo, 440002318

\par j.lim\@digipen.edu	b.khoo\@digipen.edu javier.foo\@digipen.edu
\date 18/7/2021
\brief 	the console functions

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

#pragma once
#include "globalhelpers.h"

class console
{
	// console
	std::mutex    _mutexconsole;

public:
	console() : _mutexconsole{ } 
	{
		//FreeConsole();
		//if (AllocConsole() == 0) THROW("Console creation failed");
	}
	~console() = default;

	void input(std::string& string)
	{
		RAIILOCK lockconsole(_mutexconsole);
		std::getline(std::cin, string);
	}
	void output(const STRINGCONTAINER& strings)
	{
		RAIILOCK lockconsole(_mutexconsole);
		print({ strings });
		std::cout << std::endl;
	}

	void update(bool& running)
	{
		while (running)
		{
			std::string line;
			input(line);

			if (line == "quit") running = false;

			output({ "Input is: ", line });
		}
	}
};