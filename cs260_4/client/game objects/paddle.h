/* Start Header
*****************************************************************/
/*!
\file paddle.h
\author Chloe Lim Jia-Han, 440003018
		Edwin Khoo Bing Shen, 390001918

\par j.lim\@digipen.edu	b.kohh\@digipen.edu
\date 20/6/2021
\brief 	the paddle class declaration

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#pragma once
#include "../backendwrap.h"

class ball;

class paddle
{
	glm::vec2 _pos, _nrm, _size;
public:
	paddle(const glm::vec2& = NULLVEC2,
		const glm::vec2 & = glm::vec2(1.0f, 0.0f),
		const glm::vec2& = glm::vec2(0.5f, 0.1f));
	~paddle() = default;

	glm::mat4 getmodelmat() const;

	bool collideball(ball& ballobj);
};