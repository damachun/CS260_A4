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
