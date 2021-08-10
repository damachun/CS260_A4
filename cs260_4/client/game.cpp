/* Start Header
*****************************************************************/
/*!
\file game.cpp
\author Chloe Lim Jia-Han, 440003018
		Edwin Khoo Bing Shen, 390001918
		Javier Foo, 440002318

\par j.lim\@digipen.edu	b.khoo\@digipen.edu javier.foo\@digipen.edu
\date 18/7/2021
\brief 	game system class definition

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

#include "game.h"

game::packetdata::packetdata(std::string string):
	_gamekill{ string[0] == '1' }, 
	_scoreupdate{ false }, _ballupdate{ false },
	_playerp{ }, _ballp{ }, _ballv{ }
{
	if (!_gamekill)
	{
		_scoreupdate = string[ 1 ] == '1';
		_ballupdate = string[ 2 ] == '1';
		string.erase(0, 3); // gamekill, scoreupdate, and ballupdate remove

		for (int i = 0; i < 2; ++i)
		{
			size_t delimpos = string.find(' ');
			if (!localstof(_playerp[i], string.substr(0, delimpos)))
				THROW("packetdate() failed\n\tfailed to init _playerp with: " + string);

			string.erase(0, delimpos + 1);
		}

		if (_ballupdate)
		{
			for (int i = 0; i < 2; ++i)
			{
				size_t delimpos = string.find(' ');
				if (!localstof(_ballp[i], string.substr(0, delimpos)))
					THROW("packetdate() failed\n\tfailed to init _ballp with: " + string);

				string.erase(0, delimpos + 1);
			}

			for (int i = 0; i < 2; ++i)
			{
				size_t delimpos = string.find(' ');
				if (!localstof(_ballv[i], string.substr(0, delimpos)))
					THROW("packetdate() failed\n\tfailed to init _ballv with: " + string);

				string.erase(0, delimpos + 1);
			}
		}
	}
}

game::game(ClientsHandler& clienthandler):
	_clienthandler{ clienthandler },
	_gamestate{ gamestate::WAITING }, 
	_currid{ 0 }, _ballobj{ { 0.0f, 0.0f }, 50.0f, 15.0f, { 0.0f, 0.0f } }, _players{ },
	_gamedone{ false }
{}

void game::init()
{
	// wait for all four to connect
	while (!_clienthandler.startable() && run);

	_currid = _clienthandler.getplayerid();

	glm::vec2 positions[4] = { {0.0f, -375.0f}, {375.0f, 0.0f}, {0.0f, 375.0f}, {-375.0f, 0.0f} };
	glm::vec2 normals[4] = { {0.0f, 1.0f}, {-1.0f, 0.0f}, {0.0f, -1.0f}, {1.0f, 0.0f} };
	glm::vec2 sizes[4] = { {200.0f, 50.0f}, {50.0f, 200.0f}, {200.0f, 50.0f}, {50.0f, 200.0f} };

	for (int i = 0; i < 4; ++i)
	{
		_players[i]._playerid = (i + _currid) % 4;
		_players[i]._score = 0;
		_players[i]._pos = static_cast<playerpos>(i);
		_players[i]._paddle = paddle(positions[i], normals[i], sizes[i]);
	}

	const glm::mat4 rotate{ glm::rotate(glm::mat4(1.0f), glm::half_pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f)) };
	glm::vec4 ballvel{ 0.25f, -0.8f, 0.0f, 0.0f };
	ballvel = glm::normalize(ballvel);
	for (int i = 0; i < _currid; ++i)
		ballvel = rotate * ballvel;
	_ballobj.setvel(glm::vec2(ballvel.x, ballvel.y));

	_gamestate = gamestate::INPROGRESS;

	print({ "You control the paddle at the bottom\n",
		"LEFT CONTROL and RIGHT CONTROL keys to move\n",
		"First to 3 wins\nGood luck!" });

	updateplayers( true, true );
}
bool game::update(const bool& getinput)
{
	static const glm::mat4 rotation[4] =
	{
		glm::mat4(1.0f),
		glm::rotate(glm::mat4(1.0f), glm::half_pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f)),
		glm::rotate(glm::mat4(1.0f), glm::pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f)),
		glm::rotate(glm::mat4(1.0f), glm::three_over_two_pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f))
	};

	static CLOCKTIME startt = gettime();

	if (_players[0]._score >= 3)
	{
		updateplayers_gamekill();
		return true;
	}

	if (_clienthandler.ended())
	{
		updateplayers_gamekill();
		return true;
	}

	bool collision = false, localcollision = false, gameend = false;

	if (getinput)
	{
		for (int i = 1; i < 4; ++i)
		{
			try
			{
				packetdata currpacket{ _clienthandler.retrieve( _players[ i ]._playerid ) };
				if( currpacket._gamekill )
				{
					gameend = true;
					_gamedone = true;
				}

				glm::vec4 pos{ currpacket._playerp.x, currpacket._playerp.y, 0.0f, 1.0f };
				_players[ i ]._paddle.setpos( glm::vec2( rotation[ i ] * pos ) );

				if( currpacket._ballupdate && !collision ) // only update ball upon first collision
				{
					if( currpacket._scoreupdate )
					{
						for( int j = 0; j < 4; ++j )
							if( i != j ) ++_players[ j ]._score;
						print( { "Your current score is: ", std::to_string( _players[ 0 ]._score ) } );
					}

					// update the current game state according to the packet - we simply set the position and velocity instead of interpolate so that all clients have the exact same game state
					pos = glm::vec4{ currpacket._ballp.x, currpacket._ballp.y, 0.0f, 1.0f };
					glm::vec4 vel{ currpacket._ballv.x, currpacket._ballv.y, 0.0f, 0.0f };
					_ballobj.setpos( glm::vec2( rotation[ i ] * pos ) );
					_ballobj.setvel( glm::vec2( rotation[ i ] * vel ) );
					collision = true;
				}
			}

			catch( ... )
			{
			}
		}
	}

	if (!gameend)
	{
		bool toupdate = processinput();
		if (!collision)
			localcollision = _players[0]._paddle.collideball(_ballobj);

		CLOCKTIME currt = gettime();
		_ballobj.move(deltatime(startt, currt));
		startt = currt;

		bool scoreupdate = balledge();

		if (toupdate || scoreupdate || localcollision)
			updateplayers(scoreupdate, localcollision || scoreupdate);

		return false;
	}
	else
	{
		if (_players[0]._score >= 3)
			print({ "Congratulations!\nYou won!" });
		else
			print({ "Better luck next time!" });

		updateplayers_gamekill();

		end();

		return true;
	}
}
void game::end()
{
	print({ "Thank you for playing, hope you had fun!" });
	Sleep(10); // sleep for 10s so the player can read the message
}

void game::updateplayers_gamekill()
{
	_clienthandler.sendcmd("1"); // for game ended
}
void game::updateplayers(const bool& scoreupdate, const bool& ballupdate)
{
	packetdata currpacket(scoreupdate, ballupdate,
		_players[0]._paddle.getpos(), _ballobj.getpos(), _ballobj.getvel());
	
	if( !_clienthandler.sendmove( currpacket ) )
	{
		updateplayers_gamekill();
	}
}

bool game::processinput()
{
	bool triggered = false;
	const glm::vec2 step{ 10.0f, 0.0f };

	// move left if left ctrl, move right if right ctrl
	if( GetKeyState( VK_LCONTROL ) & 0x8000 )
	{
		triggered = true;
		_players[0]._paddle.movepos(-step);
	}

	if( GetKeyState( VK_RCONTROL ) & 0x8000 )
	{
		triggered = true;
		_players[0]._paddle.movepos(step);
	}

	return triggered;
}

bool game::balledge() // this function checks if the ball is out of bounds
{
	/* if (!_ballobj.checkfront({ 0.0f, -400.0f }, { 0.0f, 1.0f }))
	{
		_ballobj.setpos({0.0f, 0.0f});
		return true;
	} */

	// ball out of range
	if (_ballobj.getpos().x < -400 || _ballobj.getpos().y < -400 || _ballobj.getpos().x > 400 || _ballobj.getpos().y > 400)
	{
		_ballobj.setpos({ 0.0f, 0.0f });
		return true;
	}

	return false;
}

void game::render(renderer& render)
{
	if (_gamestate == gamestate::WAITING) return;

	render.clearModels();
	render.editCircleMat(_ballobj.getmodelmat()); // create the ball

	for (const player& curr : _players) // create the player paddles
		render.addModelMat(curr._paddle.getmodelmat());
}