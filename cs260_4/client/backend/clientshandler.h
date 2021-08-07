/* Start Header
*****************************************************************/
/*!
\file clientshandler.h
\author Chloe Lim Jia-Han, 440003018
		Edwin Khoo Bing Shen, 390001918

\par j.lim\@digipen.edu	b.kohh\@digipen.edu
\date 20/6/2021
\brief 	the ClientsHandler class declaration

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

#pragma once
#include "gobackn.h"

#define MAX_PLAYERS 4

class ClientsHandler
{
private:
	int _playerid = -1;

	sockaddr _addrs[ MAX_PLAYERS ]{};

	std::string _inputs[ MAX_PLAYERS ]{};

	wrapSOCKET _ws;

public:
	ClientsHandler( ClientsHandler& ) = delete;
	ClientsHandler( ClientsHandler&& ) = delete;
	ClientsHandler( const ClientsHandler& ) = delete;
	ClientsHandler( const ClientsHandler&& ) = delete;

	ClientsHandler() = default;
	~ClientsHandler() = default;

	bool init( int argc, char** argv );

	void sendto( size_t index );
	void sendall();
	bool recvfrom( size_t index );
	std::string retrieve( size_t index );

	STRINGCONTAINER retrieveall();
};
