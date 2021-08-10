/* Start Header
*****************************************************************/
/*!
\file shader.fs
\author Chloe Lim Jia-Han, 440003018
		Edwin Khoo Bing Shen, 390001918

\par j.lim\@digipen.edu	b.kohh\@digipen.edu
\date 20/6/2021
\brief 	fragment shader

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

#version 330 core

out vec4 FragColor;

in vec3 col;

void main()
{
	FragColor = vec4(col, 1.0f);
}