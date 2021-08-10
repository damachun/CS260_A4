/* Start Header
*****************************************************************/
/*!
\file clientshandler.h
\author Chloe Lim Jia-Han, 440003018
		Edwin Khoo Bing Shen, 390001918
		Javier Foo, 440002318

\par j.lim\@digipen.edu	b.khoo\@digipen.edu javier.foo\@digipen.edu
\date 18/7/2021
\brief 	the ClientsHandler class declaration

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

#pragma once
#include "wrappers/wrapSOCKET.h"
#include "producerconsumer.h"

#define MAX_PLAYERS 4
#define NO_PLAYER_ID -1

class ClientsHandler
{
public:
	/* ClientsHandler( ClientsHandler& ) = delete;
	ClientsHandler( ClientsHandler&& ) = delete;
	ClientsHandler( const ClientsHandler& ) = delete;
	ClientsHandler( const ClientsHandler&& ) = delete;

	ClientsHandler& operator=( ClientsHandler& ) = delete;
	ClientsHandler& operator=( ClientsHandler&& ) = delete;
	ClientsHandler& operator=( const ClientsHandler& ) = delete;
	ClientsHandler& operator=( const ClientsHandler&& ) = delete; */

	ClientsHandler(int argc, char** argv);
	~ClientsHandler() = default;

	//bool init( int argc, char** argv );

	int getplayerid()
	{
		return _playerid;
	}

	bool startable();

	void sendcmd( const std::string& text );
	bool sendmove( const std::string& text );
	bool recvfrom( std::string* msg = nullptr );
	std::string retrieve( size_t index );

	STRINGCONTAINER retrieveall();

	bool isupdated();
	bool ended();

	static bool waitrecv( ClientsHandler* handler );
	static bool waitsend( ClientsHandler* handler );

private:
	int _playerid = NO_PLAYER_ID;

	sockaddr _addrs[ MAX_PLAYERS ]{};

	std::string _inputs[ MAX_PLAYERS ]{};
	unsigned _hashvalue[ MAX_PLAYERS ]{};

	wrapSOCKET _ws;

	unsigned char _cnnct = 0;
	unsigned char _lshv = 0;
	unsigned char _updated = 0;
	unsigned char _plyrc = 0;

	std::string _text;

	void acknowledge( size_t i );
	void sendto( size_t index, const std::string& text );

	static void disconnect()
	{
	}
	
	taskQueue<
		ClientsHandler*,
		decltype( waitrecv ),
		decltype( disconnect )
	> chrecv
	{ 1, ClientsHandler::waitrecv, disconnect };
	
	taskQueue<
		ClientsHandler*,
		decltype( waitsend ),
		decltype( disconnect )
	> chsend
	{ 1, ClientsHandler::waitsend, disconnect };
};
