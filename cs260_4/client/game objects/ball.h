/* Start Header
*****************************************************************/
/*!
\file ball.h
\author Chloe Lim Jia-Han, 440003018
		Edwin Khoo Bing Shen, 390001918

\par j.lim\@digipen.edu	b.kohh\@digipen.edu
\date 20/6/2021
\brief 	the ball class declaration

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#pragma once
#include "../backendwrap.h"
#include "paddle.h"

class ball
{
	glm::vec2   _pos;
	const float _size, _speed;
	glm::vec2   _vel;
public:
	ball(const glm::vec2& pos = NULLVEC2,
		const float& size = 0.1f,
		const float& speed = 0.5f,
		const glm::vec2& vel = NULLVEC2) :
		_pos{ pos }, _vel{ vel }, _speed{ speed }, _size(size) {}
	~ball() = default;

	glm::mat4 getmodelmat() const
	{
		glm::mat4 trans{}, scale{};
		glm::translate(trans, glm::vec3(_pos.x, _pos.y, 0.0f));
		glm::scale(scale, glm::vec3(_size, _size, 0.0f));
		return trans * scale;
	}

	friend bool paddle::collideball(ball&);
};