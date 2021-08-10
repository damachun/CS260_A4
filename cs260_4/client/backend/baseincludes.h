/* Start Header
*****************************************************************/
/*!
\file baseincludes.h
\author Chloe Lim Jia-Han, 440003018
		Edwin Khoo Bing Shen, 390001918
		Javier Foo, 440002318

\par j.lim\@digipen.edu	b.khoo\@digipen.edu javier.foo\@digipen.edu
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

// graphics
#define GLEW_STATIC
#include <glew.h>

// windows
#include <glm.hpp>
const glm::vec3 NULLVEC3 = glm::vec3(0.0f, 0.0f, 0.0f);
const glm::vec2 NULLVEC2 = glm::vec2(0.0f, 0.0f);
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
//#include <simd/common.h>
