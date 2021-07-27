/* Start Header
*****************************************************************/
/*!
\file wrapSOCKET.cpp
\author Chloe Lim Jia-Han, 440003018

\par j.lim\@digipen.edu
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
wrapSOCKET::wrapSOCKET(const wrapAddrinfo& addinf): // listener
	_socket{ socket(addinf._hints.ai_family,
		addinf._hints.ai_socktype,
		addinf._hints.ai_protocol) }
{
	if (_socket == INVALID_SOCKET)
		THROW("wrapSOCKET() (listener) failed\n\tsocket() failed");

	if (bind(_socket, addinf._rspinfo->ai_addr,
		CAST_INT(addinf._rspinfo->ai_addrlen)) != NO_ERROR
		|| _socket == INVALID_SOCKET)
		THROW("wrapSOCKET() (listener) failed\n\tbind() failed");

	//char ip_addr[256];
	//sockaddr_in* in = (sockaddr_in*)addinf._rspinfo->ai_addr;
	//inet_ntop(AF_INET, &(in->sin_addr), ip_addr, 256);
	//
	//print({ "IP address = " + std::string(ip_addr) });
	//if (listen(_socket, SOMAXCONN) != NO_ERROR)
	//	THROW("wrapSOCKET() (listener) failed\n\tlisten() failed");
}
wrapSOCKET::~wrapSOCKET()
{
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
	char buffer[4096] = { '\0' };
	bool collect = true;
	size_t buffermax = 4095, offset = 0;

	//SecureZeroMemory(&_addr, sizeof(_addr));
	int addresssize = sizeof(_addr);

	while (collect)
	{
		int bytesrecv = recvfrom(_socket, buffer + offset, static_cast<int>(buffermax - offset), 0,
			&_addr, &addresssize);
		switch (bytesrecv)
		{
		case SOCKET_ERROR:
			DBGPRINT({ "wrapSOCKET::sockrecv() failed\n\trecvfrom() failed" });
			return false;
		case 0:
			DBGPRINT({ "wrapSOCKET::sockrecv() succeeded\n\trecvfrom() done" });
			return false;
		default:
			for (int i = 0; i < bytesrecv; ++i)
			{
				if (buffer[offset + i] == '\b')
				{
					if (offset > 0)
					{
						if (bytesrecv == 1)
						{
							--offset;
							--bytesrecv;
							break;
						}
						memcpy(buffer + offset + i - 1, buffer + offset + i + 1, bytesrecv);
						--bytesrecv;
					}
					else break;
				}
			}

			if (bytesrecv == 0)
				continue;

			offset += bytesrecv;
			if (offset > buffermax)
			{
				DBGPRINT({ "wrapSOCKET::sockrecv() failed\n\tinput too long" });
				return false;
			}

			string = std::string(buffer);
			if (string.find("\r\n\r\n") != std::string::npos)
			{
				collect = false; // only terminate when end is found
				string.erase(string.find("\r\n\r\n"));
			}
			break;
		}
	}

	return true;
}
bool wrapSOCKET::socksend(sockaddr& _addr, const STRINGCONTAINER& strings)
{
	std::string finalstring;
	for (const std::string& string : strings)
		finalstring += string;
	finalstring += "\r\n\r\n";

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