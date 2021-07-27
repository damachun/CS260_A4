/* Start Header
*****************************************************************/
/*!
\file wrapSOCKET.h
\author Chloe Lim Jia-Han, 440003018

\par j.lim\@digipen.edu
\date 18/7/2021
\brief 	wrapSOCKET class declarations

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

#pragma once

#include "../baseincludes.h"

class wrapAddrinfo;

class wrapSOCKET
{
private:
	SOCKET   _socket;  // socket to use
public:
	wrapSOCKET(const wrapAddrinfo&); // listener
	~wrapSOCKET();
	void exit();

	// special
	bool sockrecv(sockaddr&, std::string&);
	bool socksend(sockaddr&, const STRINGCONTAINER&);

	SOCKET& getsock() { return _socket; }

	inline bool active() const { return _socket != INVALID_SOCKET; }
	bool ipaddress(std::string&) const;
};