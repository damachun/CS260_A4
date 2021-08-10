/* Start Header
*****************************************************************/
/*!
\file wrapWSADATA.h
\author Chloe Lim Jia-Han, 440003018
		Edwin Khoo Bing Shen, 390001918
		Javier Foo, 440002318

\par j.lim\@digipen.edu	b.khoo\@digipen.edu javier.foo\@digipen.edu
\date 18/7/2021
\brief 	wrapWSADATA class

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

#pragma once

#include "../baseincludes.h"

class wrapWSADATA
{
private:
	WSADATA _wsadata; // contains info about win sockets impl
public:
	wrapWSADATA()
	{
		if (WSAStartup(MAKEWORD(2, 2), &_wsadata) != NO_ERROR)
			THROW("wrapWSADATA() failed\n\tWSAStartup() failed");
	}
	~wrapWSADATA()
	{
		WSACleanup();
	}
};