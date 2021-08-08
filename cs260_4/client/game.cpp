/* Start Header
*****************************************************************/
/*!
\file game.cpp
\author Chloe Lim Jia-Han, 440003018
		Edwin Khoo Bing Shen, 390001918

\par j.lim\@digipen.edu	b.kohh\@digipen.edu
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
		_scoreupdate = string[0] == '1';
		_ballupdate = string[0] == '1';
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
	_currid{ 0 }, _ballobj{ { 0.0f, 0.0f }, 0.1f, 0.25f, { 0.0f, 0.0f } }, _players{ }
{}

void game::init()
{
	// wait for all four to connect
	while (!_clienthandler.startable() && run);

	_currid = _clienthandler.getplayerid();

	const glm::mat4 rotate{ glm::rotate(glm::mat4(1.0f), glm::half_pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f)) };
	glm::vec2 currpos{ 0.0f, -0.5f }, currnormal{ 0.0f, 1.0f };

	for (int i = 0; i < 4; ++i)
	{
		_players[i]._playerid = (i + _currid) % 4;
		_players[i]._score = 0;
		_players[i]._pos = static_cast<playerpos>(i);
		_players[i]._paddle = paddle(currpos, currnormal);

		currpos = glm::vec2(rotate * glm::vec4(currpos.x, currpos.y, 0.0f, 1.0f));
		currnormal = glm::vec2(rotate * glm::vec4(currnormal.x, currnormal.y, 0.0f, 0.0f));
	}

	glm::vec4 ballvel{ 0.0f, -1.0f, 0.0f, 0.0f };
	for (int i = 0; i < _currid; ++i)
		ballvel = rotate * ballvel;
	_ballobj.setvel(glm::vec2(ballvel.x, ballvel.y));

	print({ "You control the paddle at the bottom\n",
		"Left and Right arrow keys to move", // ????????????
		"First to 3 wins\nGood luck!" });
}
void game::update()
{
	const glm::mat4 rotation[4] =
	{
		glm::mat4(1.0f),
		glm::rotate(glm::mat4(1.0f), glm::half_pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f)),
		glm::rotate(glm::mat4(1.0f), glm::pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f)),
		glm::rotate(glm::mat4(1.0f), glm::three_over_two_pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f))
	};

	CLOCKTIME startt = gettime();

	while (run)
	{
		if (_players[0]._score >= 3)
		{
			updateplayers_gamekill();
			break;
		}

		bool collision = false, localcollision = false, gameend = false;

		for (int i = 1; i < 4; ++i)
		{
			packetdata currpacket{ _clienthandler.retrieve(_players[i]._playerid) };
			if (currpacket._gamekill)
			{
				gameend = true;
				break;
			}
			glm::vec4 pos{ currpacket._playerp.x, currpacket._playerp.y, 0.0f, 1.0f };
			_players[i]._paddle.setpos(glm::vec2(rotation[i] * pos));
			if (currpacket._ballupdate && !collision) // only update ball upon first collision
			{
				if (currpacket._scoreupdate)
				{
					for (int j = 0; j < 4; ++j)
						if (i != j) ++_players[j]._score;
					print({ "Your current score is: ", std::to_string(_players[0]._score) });
				}

				pos = glm::vec4{ currpacket._ballp.x, currpacket._ballp.y, 0.0f, 1.0f };
				glm::vec4 vel{ currpacket._ballv.x, currpacket._ballv.y, 0.0f, 0.0f };
				_ballobj.setpos(glm::vec2(rotation[i] * pos));
				_ballobj.setvel(glm::vec2(rotation[i] * vel));
				collision = true;
			}
		}
		
		if (gameend)
			break;

		bool toupdate = processinput(); // NOT IMPLEMENTED YET
		if (!collision)
			localcollision = _players[0]._paddle.collideball(_ballobj);

		CLOCKTIME currt = gettime();
		_ballobj.move(deltatime(startt, currt));
		startt = currt;

		bool scoreupdate = balledge();

		if (toupdate || scoreupdate || localcollision) 
			updateplayers(scoreupdate, localcollision || scoreupdate);
	}

	if (_players[0]._score >= 3)
		print({ "Congratulations!\nYou won!" });
	else
		print({ "Better luck next time!" });
}
void game::end()
{
	print({ "Thank you for playing, hope you had fun!" });
}

void game::updateplayers_gamekill()
{
	_clienthandler.sendcmd("1"); // for game ended
}
void game::updateplayers(const bool& scoreupdate, const bool& ballupdate)
{
	packetdata currpacket(scoreupdate, ballupdate,
		_players[0]._paddle.getpos(), _ballobj.getpos(), _ballobj.getvel());
	_clienthandler.sendmove(currpacket);
}

bool game::processinput()
{
	//[TODO EK]
	// IF EITHER LEFT OR RIGHT IS PRESSED
	// RETURN TRUE

	return false;
}
bool game::balledge()
{
	if (_ballobj.checkfront({ 0.0f, -0.5f }, { 0.0f, 1.0f }))
	{
		_ballobj.setpos({0.0f, 0.0f});
		return true;
	}
	return false;
}