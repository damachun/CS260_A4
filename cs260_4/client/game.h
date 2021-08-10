/* Start Header
*****************************************************************/
/*!
\file game.h
\author Chloe Lim Jia-Han, 440003018
		Edwin Khoo Bing Shen, 390001918
		Javier Foo, 440002318

\par j.lim\@digipen.edu	b.khoo\@digipen.edu javier.foo\@digipen.edu
\date 18/7/2021
\brief 	game system class declaration

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

#pragma once
#include "game objects/ball.h"
#include "game objects/paddle.h"

class game
{
	enum class gamestate : int
	{
		WAITING = 0,
		INPROGRESS
	};

	enum class playerpos : int
	{
		CURR = 0,
		RIGHT,
		OPP, // for opposite
		LEFT
	};

	struct player
	{
		int       _playerid = 0;
		int       _score = 0;
		playerpos _pos = playerpos::CURR;
		paddle    _paddle;
	};

	struct packetdata
	{
		bool      _gamekill;
		bool      _scoreupdate;
		bool      _ballupdate;
		glm::vec2 _playerp;
		glm::vec2 _ballp;
		glm::vec2 _ballv;

		packetdata(std::string);
		packetdata(const bool& scoreu, const bool& ballu, 
			const glm::vec2& playerp, 
			const glm::vec2& ballp = NULLVEC2, const glm::vec2& ballv = NULLVEC2):
			_gamekill{ false }, _scoreupdate{ scoreu }, _ballupdate{ ballu },
			_playerp{ playerp }, _ballp{ ballp }, _ballv{ ballv } {}
		operator std::string() const
		{
			if (!_gamekill)
			{
				// prepare the packet according to the state of the game
				if (_ballupdate)
				{
					STRINGCONTAINER wballupdate = {
						"0", _scoreupdate ? "1" : "0", "1", // game end, score update, ball update
						std::to_string(_playerp.x), " ", std::to_string(_playerp.y), " ", // player position
						std::to_string(_ballp.x),   " ", std::to_string(_ballp.y),   " ", // ball position
						std::to_string(_ballv.x),   " ", std::to_string(_ballv.y),   " ", // ball velocity
					};

					return stringconttostring(wballupdate);
				}
				else
				{
					STRINGCONTAINER woballupdate = {
						"0", _scoreupdate ? "1" : "0", "0", // game end, score update, ball update
						std::to_string(_playerp.x), " ", std::to_string(_playerp.y), " " // ball doesn't need to be updated, so just send the player position
					};

					return stringconttostring(woballupdate);
				}
			}
			else return std::string("1"); // game has ended
		}
	};

	ClientsHandler& _clienthandler;

	gamestate _gamestate;

	int    _currid;
	ball   _ballobj;
	player _players[4];

	bool _gamedone;

	// these are handled by the individual clients
	bool processinput();
	bool balledge();

public:
	game(ClientsHandler&);
	~game() = default;

	void init();
	bool update(const bool&);
	void end();

	void updateplayers_gamekill();
	void updateplayers(const bool& , const bool&);

	void render(renderer&);
};