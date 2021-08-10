/* Start Header
*****************************************************************/
/*!
\file wrapSOCKET.h
\author Chloe Lim Jia-Han, 440003018
		Edwin Khoo Bing Shen, 390001918
		Javier Foo, 440002318

\par j.lim\@digipen.edu	b.khoo\@digipen.edu javier.foo\@digipen.edu
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
	wrapSOCKET() : _socket{ INVALID_SOCKET } {};
	/* wrapSOCKET( wrapSOCKET& ) = delete;
	wrapSOCKET( wrapSOCKET&& ) = delete;
	wrapSOCKET( const wrapSOCKET& ) = delete;
	wrapSOCKET( const wrapSOCKET&& ) = delete; */

	bool trybind( const wrapAddrinfo& );	// listener
	~wrapSOCKET();
	void exit();

	// special
	bool sockrecv(sockaddr&, std::string&);
	bool socksend(sockaddr&, const std::string&);

	SOCKET& getsock() { return _socket; }

	inline bool active() const { return _socket != INVALID_SOCKET; }
	bool ipaddress(std::string&) const;
};