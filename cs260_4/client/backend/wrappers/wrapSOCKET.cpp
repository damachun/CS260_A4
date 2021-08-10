/* Start Header
*****************************************************************/
/*!
\file wrapSOCKET.cpp
\author Chloe Lim Jia-Han, 440003018
		Edwin Khoo Bing Shen, 390001918
		Javier Foo, 440002318
		 
\par j.lim\@digipen.edu	b.khoo\@digipen.edu javier.foo\@digipen.edu
\date 18/7/2021
\brief 	wrapSOCKET class definitions

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

#include "wrapSOCKET.h"
#include "wrapAddrinfo.h"

// ctor dtor
bool wrapSOCKET::trybind(const wrapAddrinfo& addinf)
{
	_socket = socket( addinf._hints.ai_family,
					  addinf._hints.ai_socktype,
					  addinf._hints.ai_protocol );

	if( _socket == INVALID_SOCKET )
	{
		std::cerr << "wrapSOCKET() (listener) failed\n\tsocket() failed" << std::endl;
		return false;
	}

	if( bind( _socket, addinf._rspinfo->ai_addr,
			  CAST_INT( addinf._rspinfo->ai_addrlen ) ) != NO_ERROR
		|| _socket == INVALID_SOCKET )
	{
		std::cerr << "wrapSOCKET() (listener) failed\n\tbind() failed" << std::endl;
		return false;
	}

	// set a timeout
	struct timeval tv;
	tv.tv_sec = 10000;
	tv.tv_usec = 0;
	setsockopt(_socket, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char*>(&tv), sizeof(tv));

	return true;
}
wrapSOCKET::~wrapSOCKET()
{
	DBGPRINT({ "Socket exiting..." });
	exit();
}
void wrapSOCKET::exit()
{
	if (_socket != INVALID_SOCKET)
	{
		shutdown(_socket, SD_BOTH);
		closesocket(_socket);
		_socket = INVALID_SOCKET;
	}
}

// special
bool wrapSOCKET::sockrecv(sockaddr& _addr, std::string& string)
{
	char buffer[256] = { '\0' };
	const int buffermax = 255;

	//SecureZeroMemory(&_addr, sizeof(_addr));
	int addresssize = sizeof(_addr);

	while (true)
	{
		int bytesrecv = recvfrom(_socket, buffer, buffermax, 0,
			&_addr, &addresssize);

		switch (bytesrecv)
		{
		case SOCKET_ERROR:
			DBGPRINT({ "wrapSOCKET::sockrecv() failed\n\trecvfrom() failed" });
			return false;
		case 0:
			DBGPRINT( { "wrapSOCKET::sockrecv() failed\n\tclient disconnected" } );
			//DBGPRINT({ "wrapSOCKET::sockrecv() succeeded\n\trecvfrom() done" });
			return false;
		default:
			buffer[ bytesrecv ] = '\0';

			string = std::string( buffer, bytesrecv );
			DBGPRINT( { string } );

			//break;
			return true;
		}
	}

	return true;
}
bool wrapSOCKET::socksend(sockaddr& _addr, const std::string& string)
{
	std::string finalstring = string;

	//SecureZeroMemory(&_addr, sizeof(_addr));
	int addresssize = sizeof(_addr);

	if (sendto(_socket, finalstring.c_str(), static_cast<int>(finalstring.size()), 0,
		&_addr, addresssize) == SOCKET_ERROR)
	{
		DBGPRINT({ "wrapSOCKET::socksend() failed\n\tsendto() failed" });
		return false;
	}

	return true;
}

// NOT IN USE
bool wrapSOCKET::ipaddress(std::string&) const
{
	/*SOCKADDR_IN info = { 0 };
	int infosize = sizeof(info);
	if (getpeername(_socket, (sockaddr*)&info, &infosize) != 0)
	{
		DBGPRINT({ "wrapSOCKET::ipaddress() failed\n\tgetpeername() failed" });
		return false;
	}*/

	//ipaddress = inet_ntoa(info.sin_addr);
	return true;
}