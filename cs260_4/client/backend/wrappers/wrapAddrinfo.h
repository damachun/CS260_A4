/* Start Header
*****************************************************************/
/*!
\file wrapAddrinfo.h
\author Chloe Lim Jia-Han, 440003018
		Edwin Khoo Bing Shen, 390001918
		Javier Foo, 440002318

\par j.lim\@digipen.edu	b.khoo\@digipen.edu javier.foo\@digipen.edu
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
	std::string _hoststring; // service name/ port number as a string
	std::string _portstring; // service name/ port number as a string

public:
	wrapAddrinfo(const char* host, const char* port) :
		_hints{ }, _rspinfo{ nullptr },
		_hoststring{ host?host:"" },
		_portstring{ port }
	{
		SecureZeroMemory( &_hints, sizeof( _hints ) );

		_hints.ai_family = AF_INET;
		_hints.ai_socktype = SOCK_DGRAM;
		_hints.ai_protocol = IPPROTO_UDP;

		if (getaddrinfo(host, _portstring.c_str(), &_hints, &_rspinfo) != NO_ERROR
			|| _rspinfo == nullptr)
			THROW("wrapAddrinfo() failed\n\tgetaddrinfo() failed");
	}

	~wrapAddrinfo()
	{
		freeaddrinfo(_rspinfo);
	}

	sockaddr* getaddr()
	{
		return _rspinfo->ai_addr;
	}

	void printout() const
	{
		std::cout << "host: " << _hoststring
			<< "\nport: " << _portstring << std::endl;
	}

	// for listenersocket
	friend bool wrapSOCKET::trybind(const wrapAddrinfo&);
};