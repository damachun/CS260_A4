/* Start Header
*****************************************************************/
/*!
\file baseincludes.h
\author Chloe Lim Jia-Han, 440003018

\par j.lim\@digipen.edu
\date 18/7/2021
\brief 	main includes

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>                // entire win32 api
#include <ws2tcpip.h>               // getaddrinfo()
#pragma comment(lib, "ws2_32.lib")  // link ws2_32.lib

#include "globalhelpers.h"          // print, STRINGCONTAINER, THROW
