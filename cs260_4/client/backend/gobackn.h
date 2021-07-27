/* Start Header
*****************************************************************/
/*!
\file gobackn.h
\author Chloe Lim Jia-Han, 440003018

\par j.lim\@digipen.edu
\date 18/7/2021
\brief 	gobackn exposed functions

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

#pragma once
#include "wrappers/wrapSOCKET.h"

//[TODO JH: REDO THIS]

#define ACK "AAAA"
#define SUCCESSACK "BBBB"
#define ENDACK "CCCCCC"
#define WINDOWSIZE 4
#define HEADSIZE 6
	// headsize is the packet head size in string from
			// sending
		// 2 for frame index
		// 4 for file size
			// recv
		// 4 for ack
		// 2 for frame acknowledged
#define MAXSIZE 1024 - HEADSIZE

bool sendWindow(wrapSOCKET&, sockaddr&, const STRINGCONTAINER&);

bool recvWindow(wrapSOCKET&, sockaddr&, STRINGCONTAINER&, const float& = 0.0f);
