/* Start Header
*****************************************************************/
/*!
\file clientshandler.cpp
\author Chloe Lim Jia-Han, 440003018
		Edwin Khoo Bing Shen, 390001918
		Javier Foo, 440002318

\par j.lim\@digipen.edu	b.khoo\@digipen.edu javier.foo \@digipen.edu
\date 18/7/2021
\brief 	the clientshandler class definition

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

#include "clientshandler.h"
#include "wrappers/wrapAddrinfo.h"
#include "globalhelpers.h"
#include <chrono>
#include <ctime>
#include <thread>
#include "hash.h"

ClientsHandler::ClientsHandler(int argc, char** argv)
{
	if (_playerid != NO_PLAYER_ID || argc <= MAX_PLAYERS)
	{
		THROW("ClientsHandler() failed\n\tToo little players or Client is messed up");
	}

	wrapSOCKET tempsock;

	for (int i = 0; i < MAX_PLAYERS; ++i)
	{
		std::string hostport = argv[i + 1];
		auto found = hostport.find(':');

		if (found == hostport.npos)
		{
			THROW("ClientsHandler() failed\n\tNO HOSTPORT");
		}

		try
		{
			wrapAddrinfo info(
				hostport.substr(0, found).c_str(),
				hostport.substr(found + 1).c_str());

			wrapAddrinfo myinfo(
				nullptr,
				hostport.substr(found + 1).c_str());

			_addrs[i] = *info.getaddr();

			if( _playerid == NO_PLAYER_ID && // attempt to make a connection and assign yourself a player number
				tempsock.trybind( info ) )
			{
				tempsock.exit();
				if( _ws.trybind( myinfo ) )
				{
					_playerid = i;

					_plyrc = 1;
					_plyrc <<= i;
					_cnnct |= _plyrc;
				}
			}
		}
		catch (...)
		{
			THROW("ClientsHandler() failed\n\tINIT FAILED");
		}
	}

	chrecv.produce(this);
	chsend.produce(this);
}

bool ClientsHandler::startable()
{
	if( _cnnct == 15 )
	{
		return true;
	}

	std::string msg{ "connect" };

	msg += static_cast<char>( _playerid );
	sendcmd( msg );

	std::this_thread::sleep_for( std::chrono::seconds( 1 ) );

	return _cnnct == 15;
}

void ClientsHandler::acknowledge( size_t i )
{
	unsigned char add = 1;

	add <<= i;

	_cnnct |= add;
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
	if( !_plyrc )
	{
		return;
	}

	for( size_t i = 0; i < MAX_PLAYERS; ++i )
	{
		sendto( i, text );
	}
}

bool ClientsHandler::sendmove( const std::string& text )
{
	if( !_plyrc )
	{
		return false;
	}

	_text = text;
	_lshv |= _plyrc;

	return true;
}

bool ClientsHandler::recvfrom( std::string* msg )
{
	if( !_plyrc )
	{
		return false;
	}

	sockaddr addr{};
	std::string text;

	if( !_ws.sockrecv( addr, text ) ) // if SOCKET_ERROR or no bytes received
	{
		for( size_t i = 0; i < MAX_PLAYERS; ++i )
		{
			if( !std::memcmp( &addr, &_addrs[ i ], sizeof( addr ) ) )
			{
				unsigned char remove = 1;

				remove <<= i;

				if( _cnnct & remove )
				{
					_plyrc = 0;
					return false;
				}

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
			if( !std::memcmp( "h:", text.c_str(), 2 ) ) // received a hash
			{
				std::memcpy( &_hashvalue[ i ], &text[ 2 ], 4 );

				unsigned char add = 1;

				add <<= i;

				_lshv |= add;

				break;
			}
			else if( !std::memcmp( "m:", text.c_str(), 2 ) ) // received a message
			{
				text.erase( 0, 2 );

				if( _hashvalue[ i ] ^ hashfn( text ) )
				{
					_inputs[ i ] = text;

					unsigned char add = 1;

					add <<= i;

					_updated |= add;

					return true;
				}
			}
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

bool ClientsHandler::isupdated()
{
	if( _updated == 15 )
	{
		_updated = _plyrc;
		return true;
	}

	return false;
}

bool ClientsHandler::ended()
{
	return !_plyrc;
}

bool ClientsHandler::waitrecv( ClientsHandler* handler )
{
	std::string text;
	while( true )
	{
		handler->recvfrom( &text );

		if (!handler->_plyrc)
			return false;

		if( text.length() )
		{
			if( text[ 0 ] == '1' )
			{
				handler->_plyrc = 0;
				break;
			}
			else if( !std::strncmp( "connect", text.c_str(), 7 ) )
			{
				char c = text[ 7 ];

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
				char c = text[ 3 ];

				if( c >= 0 && c < MAX_PLAYERS)
				{
					handler->acknowledge( c );
				}
			}
		}
	}

	return false;
}

bool ClientsHandler::waitsend( ClientsHandler* handler )
{
	while( true )
	{
		if( !handler->_plyrc )
		{
			return false;
		}

		if( !( handler->_lshv & handler->_plyrc ) )
		{
			continue;
		}

		const std::string text{ handler->_text };
		std::string hash{ "h:" };

		{
			char cstr[ 4 ]{ 0 };
			unsigned uh = hashfn( text );

			std::memcpy( cstr, &uh, 4 );

			hash += std::string( cstr, 4 );
		}

		while( true )
		{
			if( !handler->_plyrc )
			{
				return false;
			}

			handler->sendcmd( hash );

			auto start = std::chrono::steady_clock::now();
			std::chrono::duration<float> elapsed;

			while( handler->_lshv ^ 15 )
			{
				elapsed = std::chrono::steady_clock::now() - start;

				if( elapsed.count() >= 1.f )
				{
					break;
				}
			}

			if( handler->_lshv == 15 )
			{
				break;
			}
		}

		std::string msg{ "m:" };
		msg += text;

		handler->sendcmd( msg );

		handler->_lshv = 0;

		return true;
	}
}
