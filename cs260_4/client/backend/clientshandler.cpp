/* Start Header
*****************************************************************/
/*!
\file clientshandler.cpp
\author Chloe Lim Jia-Han, 440003018
		Edwin Khoo Bing Shen, 390001918

\par j.lim\@digipen.edu	b.kohh\@digipen.edu
\date 20/6/2021
\brief 	the clientshandler class definition

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

#include "clientshandler.h"

bool ClientsHandler::init( int argc, char** argv )
{
	return true;
}

void ClientsHandler::sendto( size_t index )
{
	if( index >= MAX_PLAYERS || index == _playerid )
	{
		return;
	}
}

void ClientsHandler::sendall()
{
	for( size_t i = 0; i < MAX_PLAYERS; ++i )
	{
		sendto( i );
	}
}

bool ClientsHandler::recvfrom( size_t index )
{
	return false;
}

std::string ClientsHandler::retrieve( size_t index )
{
	if( index < MAX_PLAYERS && index != _playerid )
	{
		return _inputs[ index ];
	}

	return "";
}

STRINGCONTAINER ClientsHandler::retrieveall()
{
	STRINGCONTAINER container;

	for( auto& input :_inputs )
	{
		container.push_back( input );
	}

	return container;
}
