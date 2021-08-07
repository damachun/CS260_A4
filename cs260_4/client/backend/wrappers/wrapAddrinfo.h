/* Start Header
*****************************************************************/
/*!
\file wrapAddrinfo.h
\author Chloe Lim Jia-Han, 440003018

\par j.lim\@digipen.edu
\date 18/7/2021
\brief 	wrapAddrinfo class

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

#pragma once

#include "../baseincludes.h"
#include "wrapSOCKET.h"

class wrapAddrinfo
{
private:
	addrinfo    _hints;      // desc addr info for use with tcp/ip
	addrinfo* _rspinfo;      // response info about the host
	std::string _portstring; // service name/ port number as a string

public:
	wrapAddrinfo(const uint16_t& port) :
		_hints{ }, _rspinfo{ nullptr },
		_portstring{ std::to_string(port) }
	{
		SecureZeroMemory( &_hints, sizeof( _hints ) );

		_hints.ai_family = AF_INET;
		_hints.ai_socktype = SOCK_DGRAM;
		_hints.ai_protocol = IPPROTO_UDP;

		if (getaddrinfo(nullptr, _portstring.c_str(), &_hints, &_rspinfo) != NO_ERROR
			|| _rspinfo == nullptr)
			THROW("wrapAddrinfo() failed\n\tgetaddrinfo() failed");
	}
	~wrapAddrinfo()
	{
		freeaddrinfo(_rspinfo);
	}

	// for listenersocket
	friend bool wrapSOCKET::trybind(const wrapAddrinfo&);
};