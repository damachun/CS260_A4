/* Start Header
*****************************************************************/
/*!
\file renderer.cpp
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

#include "renderer.h"
#include "../window/windows.h"

const unsigned int CIRCLEVTXCOUNT = 15;

renderer::renderer(window& winref):
	_bgcol{ 0.0f, 0.0f, 0.0f }, _objcol{ 1.0f, 1.0f, 1.0f },
	_winref{ winref },
	_width{ CAST_FLOAT(winref.getwindowwidth()) }, _height{ CAST_FLOAT(winref.getwindowheight()) },
	_shader{ loadShader("Dependencies/shaders/shader.vs", "Dependencies/shaders/shader.fs") },
	_rectdata{ initRectData() }, _circledata{ initCircleData() },
	_mmat{ glGetUniformLocation(_shader._prog, "model") },
	_vmat{ glGetUniformLocation(_shader._prog, "view") },
	_pmat{ glGetUniformLocation(_shader._prog, "projection") },
	_viewmat{ /*glm::scale(glm::mat4(1.0f), glm::vec3(CAST_FLOAT(_width)/2.0f, CAST_FLOAT(_height)/2.0f, 0.0f))*/ 1.0f },
	_projmat{ glm::ortho(-_width / 2.0f, _width / 2.0f,
		-_height / 2.0f, _height / 2.0f) },
	_circlemodelmat{ }, _modelmats{ }
{
	glUseProgram(_shader._prog);
	glUniformMatrix4fv(_vmat, 1, GL_FALSE, glm::value_ptr(_viewmat));
	glUniformMatrix4fv(_pmat, 1, GL_FALSE, glm::value_ptr(_projmat));
	glUseProgram(0);

	glClearColor(_bgcol.r, _bgcol.g, _bgcol.b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

renderer::~renderer()
{
	glDeleteShader(_shader._prog);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &_rectdata._vbo);
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &_rectdata._vao);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &_circledata._vbo);
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &_circledata._vao);
}

void renderer::update()
{
	glUseProgram(_shader._prog);

	if (_winref.getwindowwidth() != _width || _winref.getwindowheight() != _height)
	{
		_width = CAST_FLOAT(_winref.getwindowwidth());
		_height = CAST_FLOAT(_winref.getwindowheight());
		
		_projmat = glm::ortho(-_width / 2.0f, _width / 2.0f,
			-_height / 2.0f, _height / 2.0f);
		//_projmat = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 0.0f));
		glUniformMatrix4fv(_pmat, 1, GL_FALSE, glm::value_ptr(_projmat));
	}

	glClearColor(_bgcol.r, _bgcol.g, _bgcol.b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(_rectdata._vao);
	for (const glm::mat4& model: _modelmats)
	{
		glUniformMatrix4fv(_mmat, 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	glBindVertexArray(0);

	glBindVertexArray(_circledata._vao);
	glUniformMatrix4fv(_mmat, 1, GL_FALSE, glm::value_ptr(_circlemodelmat));
	glDrawArrays(GL_TRIANGLES, 0, CIRCLEVTXCOUNT * 3);
	glBindVertexArray(0);

	glUseProgram(0);
}

void renderer::clearModels()
{
	_modelmats.clear();
}
void renderer::addModelMat(const glm::mat4& model)
{
	_modelmats.push_back(model);
}
void renderer::editCircleMat(const glm::mat4& circlemat)
{
	_circlemodelmat = circlemat;
}

renderer::shader renderer::loadShader(const std::string& svs, const std::string& sfs) const
{
	//[TODO JH: FIND A BETTER PLACE TO CALL GLEW INIT]
	glewInit();

	shader ret;

	std::string vertex = loadfile(svs), index = loadfile(sfs);

	ret._prog = glCreateProgram();
	GLuint vs = compileShaderMod(GL_VERTEX_SHADER, vertex);
	GLuint fs = compileShaderMod(GL_FRAGMENT_SHADER, index);

	glAttachShader(ret._prog, vs);
	glAttachShader(ret._prog, fs);
	glLinkProgram(ret._prog);
	glValidateProgram(ret._prog);

	glDetachShader(ret._prog, vs);
	glDetachShader(ret._prog, fs);

	return ret;
}
GLuint renderer::compileShaderMod(const GLuint& type, const std::string& source) const
{
	GLuint id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);

	if (result == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(id, maxLength, &maxLength, &errorLog[0]);

		std::cout << "Error: Failed to compile shader:" << source << std::endl;

		for (auto character : errorLog)
			std::cout << character;
		std::cout << std::endl;
		glDeleteShader(id);

		return 0;
	}

	return id;
}
renderer::data renderer::initRectData() const
{
	const float vtx[] = {  
		-0.5f, -0.5f, 0.0f, _objcol.r, _objcol.g, _objcol.b,
		 0.5f, -0.5f, 0.0f, _objcol.r, _objcol.g, _objcol.b,
		 0.5f,  0.5f, 0.0f, _objcol.r, _objcol.g, _objcol.b,
		 0.5f,  0.5f, 0.0f, _objcol.r, _objcol.g, _objcol.b,
		-0.5f,  0.5f, 0.0f, _objcol.r, _objcol.g, _objcol.b,
		-0.5f, -0.5f, 0.0f, _objcol.r, _objcol.g, _objcol.b
	};

	data ret;

	glGenVertexArrays(1, &ret._vao);
	glGenBuffers(1, &ret._vbo);

	glBindVertexArray(ret._vao);

	glBindBuffer(GL_ARRAY_BUFFER, ret._vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 36, vtx, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return ret;
}
renderer::data renderer::initCircleData() const
{
	constexpr float pi2 = glm::two_pi<float>();
	std::vector<glm::vec3> pos;

	float angoffset = 1.0f / CAST_FLOAT(CIRCLEVTXCOUNT);
	float fi = 0.0f;
	for (unsigned int i = 0; i < CIRCLEVTXCOUNT; ++i)
	{
		// v0
		pos.push_back(NULLVEC3);

		// v1
		float theta = pi2 * fi * angoffset;
		pos.push_back({ 0.5f * cosf(theta), 0.5f * sinf(theta), 0.0f });


		// v2
		fi += 1.0f;
		theta = pi2 * fi * angoffset;
		pos.push_back({ 0.5f * cosf(theta), 0.5f * sinf(theta), 0.0f });
	}

	std::vector<float> vtx;
	for (const glm::vec3& cpos : pos)
	{
		vtx.push_back(cpos.x);
		vtx.push_back(cpos.y);
		vtx.push_back(cpos.z);

		vtx.push_back(_objcol.r);
		vtx.push_back(_objcol.g);
		vtx.push_back(_objcol.b);
	}

	data ret;

	glGenVertexArrays(1, &ret._vao);
	glGenBuffers(1, &ret._vbo);

	glBindVertexArray(ret._vao);

	glBindBuffer(GL_ARRAY_BUFFER, ret._vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vtx.size(), vtx.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return ret;
}