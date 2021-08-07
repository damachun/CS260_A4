
#include "clientshandler.h"
#include "wrappers/wrapAddrinfo.h"

bool ClientsHandler::init( int argc, char** argv )
{
	if( argc <= MAX_PLAYERS )
	{
		return false;
	}

	if( _playerid == NO_PLAYER_ID )
	{
		for( int i = 1; i <= MAX_PLAYERS; ++i )
		{
			std::string port = argv[ i ];
			auto found = port.find( ':' );

			if( found == port.npos )
			{
				return false;
			}

			try
			{
				wrapAddrinfo info(
					nullptr,
					port.substr( found + 1 ).c_str() );

				if( _ws.trybind( info ) )
				{
					_playerid = i - 1;
					break;
				}
			}
			catch( ... )
			{
				return false;
			}
		}
	}

	for( int i = 0; i < MAX_PLAYERS; ++i )
	{
		if( _playerid == i )
		{
			continue;
		}

		std::string hostport = argv[ i + 1 ];
		auto found = hostport.find( ':' );

		if( found == hostport.npos )
		{
			return false;
		}

		try
		{
			wrapAddrinfo info(
				hostport.substr( 0, found ).c_str(),
				hostport.substr( found + 1 ).c_str() );

			_addrs[ i ] = *info.getaddr();
		}
		catch( ... )
		{
			return false;
		}
	}

	return true;
}

void ClientsHandler::sendto( size_t index, const std::string& text )
{
	if( index >= MAX_PLAYERS || index == _playerid )
	{
		return;
	}

	sockaddr addr = _addrs[ index ];

	_ws.socksend( addr, text );
}

void ClientsHandler::sendall( const std::string& text )
{
	for( size_t i = 0; i < MAX_PLAYERS; ++i )
	{
		sendto( i, text );
	}
}

bool ClientsHandler::recvfrom()
{
	sockaddr addr{};
	std::string text;

	if( !_ws.sockrecv( addr, text ) )
	{
		return false;
	}

	for( int i = 0; i < MAX_PLAYERS; ++i )
	{
		if( !std::memcmp( &addr, &_addrs[ i ], sizeof( addr ) ) )
		{
			_inputs[ i ] = text;

			return true;
		}
	}

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
