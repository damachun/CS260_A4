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
#include "wrappers/wrapAddrinfo.h"
#include <chrono>
#include <thread>

bool ClientsHandler::init( int argc, char** argv )
{
	if( _playerid != NO_PLAYER_ID || argc <= MAX_PLAYERS )
	{
		return false;
	}

	wrapSOCKET tempsock;

	for( int i = 0; i < MAX_PLAYERS; ++i )
	{
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

			wrapAddrinfo myinfo(
				nullptr,
				hostport.substr( found + 1 ).c_str() );

			_addrs[ i ] = *info.getaddr();

			if( _playerid == NO_PLAYER_ID &&
				tempsock.trybind( info ) &&
				_ws.trybind( myinfo ) )
			{
				_playerid = i;

				unsigned char add = 1;

				add <<= i;

				ack |= add;
			}
		}
		catch( ... )
		{
			return false;
		}
	}

	chrecv.produce( this );

	return true;
}

bool ClientsHandler::startable()
{
	if( ack ^ 15 )
	{
		return true;
	}

	std::string msg{ "connect" };

	msg += static_cast<char>( _playerid );
	sendcmd( msg );

	std::this_thread::sleep_for( std::chrono::seconds( 1 ) );

	return ack ^ 15;
}

void ClientsHandler::acknowledge( size_t i )
{
	unsigned char add = 1;

	add <<= i;

	ack |= add;
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

void ClientsHandler::sendcmd( const std::string& text )
{
	for( size_t i = 0; i < MAX_PLAYERS; ++i )
	{
		sendto( i, text );
	}
}

void ClientsHandler::sendmove( const std::string& text )
{
	std::string hash{ "h:" };

	for( size_t i = 0; i < MAX_PLAYERS; ++i )
	{
		sendto( i, text );
	}
}

bool ClientsHandler::recvfrom( std::string* msg )
{
	sockaddr addr{};
	std::string text;

	if( !_ws.sockrecv( addr, text ) )
	{
		for( size_t i = 0; i < MAX_PLAYERS; ++i )
		{
			if( !std::memcmp( &addr, &_addrs[ i ], sizeof( addr ) ) )
			{
				unsigned char remove = 1;

				remove <<= i;

				ack ^= remove;

				break;
			}
		}

		return false;
	}

	if( msg )
	{
		*msg = text;
	}

	for( int i = 0; i < MAX_PLAYERS; ++i )
	{
		if( !std::memcmp( &addr, &_addrs[ i ], sizeof( addr ) ) )
		{
			_inputs[ i ] = text;

			std::cout << "player " << i << " sent" << std::endl;

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

bool ClientsHandler::waitrecv( ClientsHandler* handler )
{
	std::string text;
	while( true )
	{
		if( handler->recvfrom( &text ) )
		{
			if( text[ 0 ] == '1' )
			{
				break;
			}
			else if( !std::strncmp( "connect", text.c_str(), 7 ) )
			{
				char c = text[ 7 ] - '0';

				if( c >= 0 && c < MAX_PLAYERS )
				{
					handler->acknowledge( c );

					std::string msg = "ack";
					msg += static_cast<char>( handler->_playerid );
					handler->sendto( c, msg );
				}
			}
			else if( !std::strncmp( "ack", text.c_str(), 3 ) )
			{
				char c = text[ 3 ] - '0';

				if( c >= 0 && c < MAX_PLAYERS)
				{
					handler->acknowledge( c );
				}
			}
		}
	}

	return false;
}
