/* Start Header
*****************************************************************/
/*!
\file game.h
\author Chloe Lim Jia-Han, 440003018
		Edwin Khoo Bing Shen, 390001918

\par j.lim\@digipen.edu	b.kohh\@digipen.edu
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
		int       _playerid;
		int       _score;
		playerpos _pos;
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
				if (_ballupdate)
				{
					STRINGCONTAINER wballupdate = {
						"0", _scoreupdate ? "1" : "0", "1",
						std::to_string(_playerp.x), " ", std::to_string(_playerp.y), " ",
						std::to_string(_ballp.x),   " ", std::to_string(_ballp.y),   " ",
						std::to_string(_ballv.x),   " ", std::to_string(_ballv.y),   " ",
					};
					return stringconttostring(wballupdate);
				}
				else
				{
					STRINGCONTAINER woballupdate = {
						"0", _scoreupdate ? "1" : "0", "0",
						std::to_string(_playerp.x), " ", std::to_string(_playerp.y), " "
					};
					return stringconttostring(woballupdate);
				}
			}
			else return std::string("1");
		}
	};

	ClientsHandler& _clienthandler;

	gamestate _gamestate;

	int    _currid;
	ball   _ballobj;
	player _players[4];

	bool _gamedone;

	// EVERYONE CHECK THEIR OWN
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