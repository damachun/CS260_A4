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

			if( _playerid == NO_PLAYER_ID &&
				tempsock.trybind( info ) )
			{
				tempsock.exit();
				if( _ws.trybind( myinfo ) )
				{
					_playerid = i;

					std::cout << "player id: " << _playerid << std::endl;

					_plyrc = 1;
					_plyrc <<= i;
					_cnnct |= _plyrc;

					//std::cout << "initial connect " << (int)_cnnct << std::endl;
				}
			}
		}
		catch (...)
		{
			THROW("ClientsHandler() failed\n\tINIT FAILED");
		}
	}

	chrecv.produce(this);
}

//bool ClientsHandler::init( int argc, char** argv )
//{
//	if( _playerid != NO_PLAYER_ID || argc <= MAX_PLAYERS )
//	{
//		return false;
//	}
//
//	wrapSOCKET tempsock;
//
//	for( int i = 0; i < MAX_PLAYERS; ++i )
//	{
//		std::string hostport = argv[ i + 1 ];
//		auto found = hostport.find( ':' );
//
//		if( found == hostport.npos )
//		{
//			return false;
//		}
//
//		try
//		{
//			wrapAddrinfo info(
//				hostport.substr( 0, found ).c_str(),
//				hostport.substr( found + 1 ).c_str() );
//
//			wrapAddrinfo myinfo(
//				nullptr,
//				hostport.substr( found + 1 ).c_str() );
//
//			_addrs[ i ] = *info.getaddr();
//
//			if( _playerid == NO_PLAYER_ID &&
//				tempsock.trybind( info ) &&
//				_ws.trybind( myinfo ) )
//			{
//				_playerid = i;
//
//				_plyrc = 1;
//				_plyrc <<= i;
//				_cnnct |= _plyrc;
//			}
//		}
//		catch( ... )
//		{
//			return false;
//		}
//	}
//
//	chrecv.produce( this );
//
//	return true;
//}

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
	//std::cout << "ack came in: " << i << std::endl;

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
	for( size_t i = 0; i < MAX_PLAYERS; ++i )
	{
		sendto( i, text );
	}
}

bool ClientsHandler::sendmove( const std::string& text )
{
	std::string hash{ "h:" };

	{
		char cstr[ 4 ]{ 0 };
		unsigned uh = hashfn( text );

		std::memcpy( cstr, &uh, 4 );

		hash += std::string( cstr, 4 );
	}

	for( size_t i = 0; i < MAX_PLAYERS; ++i )
	{
		sendto( i, hash );
	}

	_lshv |= _plyrc;

	auto start = std::chrono::steady_clock::now();
	std::chrono::duration<float> elapsed;

	while( _lshv ^ 15 )
	{
		elapsed = std::chrono::steady_clock::now() - start;

		if( elapsed.count() >= 1.f )
		{
			return false;
		}
	}

	std::string msg{ "m:" };
	msg += text;

	for( size_t i = 0; i < MAX_PLAYERS; ++i )
	{
		sendto( i, msg );
	}

	_lshv = 0;

	return true;
}

bool ClientsHandler::recvfrom( std::string* msg )
{
	sockaddr addr{};
	std::string text;

	//std::cout << "msg otw?" << std::endl;
	if( !_ws.sockrecv( addr, text ) )
	{
		//std::cout << "msg failed?" << std::endl;
		for( size_t i = 0; i < MAX_PLAYERS; ++i )
		{
			if( !std::memcmp( &addr, &_addrs[ i ], sizeof( addr ) ) )
			{
				unsigned char remove = 1;

				remove <<= i;

				//std::cout << "current: " << _playerid
				//	<< "\nremoving: " << i << " with " << (int)remove
				//	<< "\nand with " << (int)(~remove) << std::endl;

				_cnnct &= ~remove;

				//std::cout << "connect now " << (int)_cnnct << std::endl;

				break;
			}
		}

		return false;
	}

	//std::cout << "msg recv" << std::endl;
	if( msg )
	{
		*msg = text;
	}

	for( int i = 0; i < MAX_PLAYERS; ++i )
	{
		if( !std::memcmp( &addr, &_addrs[ i ], sizeof( addr ) ) )
		{
			//std::cout << "player " << i << " sent" << std::endl;

			if( !std::memcmp( "h:", text.c_str(), 2 ) )
			{
				std::memcpy( &_hashvalue[ i ], &text[ 2 ], 4 );

				unsigned char add = 1;

				add <<= i;

				_lshv |= add;

				break;
				//return true;
			}
			else if( !std::memcmp( "m:", text.c_str(), 2 ) )
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

bool ClientsHandler::waitrecv( ClientsHandler* handler )
{
	std::string text;
	while( true )
	{
		//std::cout << "waitrecv\n";
		handler->recvfrom( &text );
		if( text.length() )
		{
			//std::cout << "received\n" << text << std::endl;
			if( text[ 0 ] == '1' )
			{
				break;
			}
			else if( !std::strncmp( "connect", text.c_str(), 7 ) )
			{
				char c = text[ 7 ];

				//std::cout << "incoming playerid: " << (int)c << std::endl;

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
