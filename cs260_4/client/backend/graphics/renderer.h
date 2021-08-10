/* Start Header
*****************************************************************/
/*!
\file renderer.h
\author Chloe Lim Jia-Han, 440003018
		Edwin Khoo Bing Shen, 390001918
		Javier Foo, 440002318

\par j.lim\@digipen.edu	b.khoo\@digipen.edu javier.foo\@digipen.edu
\date 18/7/2021
\brief 	the renderer class declaration

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

#include "../baseincludes.h"

/*
	TO KEEP THINGS SIMPLE
		ALL GAME OBJECTS ARE WHITE
		BACKGROUND IS BLACK
*/

class window;

class renderer
{
	struct shader
	{
		GLuint _prog;
		shader(): _prog{ 0 } {}
	};
	struct data
	{
		GLuint _vao, _vbo;
		data(): _vao{ 0 }, _vbo{ 0 } {}
	};
	typedef GLint SHADERLOC;

	const glm::vec3        _bgcol, _objcol;

	window&   _winref;
	float     _width, _height;
	shader    _shader;
	data      _rectdata, _circledata;
	SHADERLOC _mmat, _vmat, _pmat;

	glm::mat4              _viewmat, _projmat, _circlemodelmat;
	std::vector<glm::mat4> _modelmats;

	shader loadShader(const std::string&, const std::string&) const;
	GLuint compileShaderMod(const GLuint&, const std::string&) const;
	data initRectData() const;
	data initCircleData() const;

public:
	renderer(window&);
	~renderer();

	void update();

	void clearModels();
	void addModelMat(const glm::mat4&);
	void editCircleMat(const glm::mat4&);
};