/* Start Header
*****************************************************************/
/*!
\file paddle.cpp
\author Chloe Lim Jia-Han, 440003018
		Edwin Khoo Bing Shen, 390001918
		Javier Foo, 440002318

\par j.lim\@digipen.edu	b.khoo\@digipen.edu javier.foo\@digipen.edu
\date 18/7/2021
\brief 	the paddle class definition

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

#include "paddle.h"
#include "ball.h"

paddle::paddle(const glm::vec2& pos, const glm::vec2& nrm,
	const glm::vec2& size) : _pos{ pos }, _nrm{ nrm }, _size{ size } {}

glm::mat4 paddle::getmodelmat() const
{
	glm::mat4 trans{ 1.0f }, scale{ 1.0f };
	trans = glm::translate(trans, glm::vec3(_pos.x, _pos.y, 0.0f));
	scale = glm::scale(scale, glm::vec3(_size.x, _size.y, 0.0f));
	return trans * scale;
}

bool paddle::collideball(ball& ballobj)
{
	if (glm::dot(ballobj._vel, _nrm) > 0.0f) return false;

	glm::vec2 aabb_halfex(_size.x / 2.0f, _size.y / 2.0f);

	glm::vec2 difference = ballobj._pos - _pos;
	glm::vec2 clamped = glm::clamp(difference, -aabb_halfex.x, aabb_halfex.y);
	glm::vec2 closest = _pos + clamped;
	difference = closest - ballobj._pos;

	if (glm::length(difference) <= ballobj._size) // colliding rn
	{
		ballobj._pos -= glm::normalize(difference) * ballobj._size;
		ballobj._vel = glm::normalize(glm::reflect(ballobj._vel, _nrm));
		return true;
	}
	else
	{
		// the following is cs230 code adapted to use glm

		glm::vec2 p0 = glm::vec2(_pos.x - aabb_halfex.x, _pos.y + aabb_halfex.y)
			+ ballobj._size * _nrm;
		glm::vec2 p1 = glm::vec2(_pos.x + aabb_halfex.x, _pos.y + aabb_halfex.y)
			+ ballobj._size * _nrm;

		glm::vec2 m = { ballobj._vel.y, 0.0f - ballobj._vel.x };

		glm::vec2 bsp0 = p0 - ballobj._pos;
		glm::vec2 bsp1 = p1 - ballobj._pos;

		if ((glm::dot(m, bsp0) * glm::dot(m, bsp1)) < 0)
		{
			float nv = glm::dot(_nrm, ballobj._vel);
			if (nv != 0.0f) /* hard guard check to ensure no divide by 0 */
			{
				float interTime = (glm::dot(_nrm, glm::vec2(_pos.x - aabb_halfex.x, _pos.y + aabb_halfex.y))
					- glm::dot(_nrm, ballobj._pos)
					+ ballobj._size)
					/ nv; /* calculate the time of intersection */
				if (0.0f <= interTime && interTime <= 1.0f) /* will intersect in this frame */
				{
					ballobj._pos += ballobj._vel * interTime;
					ballobj._vel = glm::normalize(glm::reflect(ballobj._vel, _nrm));

					return true;
				}
			}
		}

		return false;
	}
}