/* Start Header
*****************************************************************/
/*!
\file backendwrap.h
\author Chloe Lim Jia-Han, 440003018
		Edwin Khoo Bing Shen, 390001918
		Javier Foo, 440002318

\par j.lim\@digipen.edu	b.khoo\@digipen.edu javier.foo\@digipen.edu
\date 18/7/2021
\brief 	backend includes

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

#pragma once

#include "backend/baseincludes.h"

#include "backend/producerconsumer.h"
#include "backend/clientshandler.h"

#include "backend/wrappers/wrapWSADATA.h"
#include "backend/wrappers/wrapAddrinfo.h"
#include "backend/wrappers/wrapSOCKET.h"

#include "backend/window/windows.h"
#include "backend/graphics/renderer.h"
#include "backend/console.h"