/* Start Header
*****************************************************************/
/*!
\file globalhelpers.cpp
\author Chloe Lim Jia-Han, 440003018

\par j.lim\@digipen.edu
\date 20/6/2021
\brief 	the global print (and STRINGCONTAINER helper) function definition(s)

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

#include "globalhelpers.h"
#include <fstream>

static std::mutex _mutexprint; // couts are overlapping

void print(const STRINGCONTAINER& toprint)
{
	RAIILOCK lockprint{ _mutexprint };

	for (const std::string& line : toprint)
		std::cout << line;
	std::cout << std::endl;
}

size_t stringcontsize(const STRINGCONTAINER& cont)
{
	size_t size = 0;
	for (const std::string& line : cont)
		size += line.size();
	return size;
}

STRINGCONTAINER stringtostringcont(std::string str, const std::string& delim)
{
	STRINGCONTAINER strcont;
	while (!str.empty())
	{
		size_t pos = str.find(delim);
		if (pos != std::string::npos)
		{
			strcont.push_back(str.substr(0, pos - 1));
			str.erase(0, pos);
		}
		else
		{
			strcont.push_back(str);
			break;
		}
		// I STILL HAVE NO CLUE WHEN POS WILL NOT BE FOUND GG
	}
	return strcont;
}

std::string loadfile(const std::string& path)
{
	std::string toReturnStr = std::string();
	std::ifstream iStream(path);

	if (iStream.is_open())
	{
		std::string line;

		while (std::getline(iStream, line))
		{
			toReturnStr.append(line);
			toReturnStr.append("\n");
		}
		iStream.close();
	}
	else std::cout << "Unable to open " << path << std::endl;

	return toReturnStr;
}

void printfile(const std::string& title, const std::string& text)
{
	std::ofstream oStream(title);
	if (oStream.is_open())
	{
		oStream << text;
		oStream.close();
	}
}

globalException::globalException(const std::string& what) :
	std::exception{ }, _what{ what } {}

const char* globalException::what() const noexcept
{
	return _what.c_str();
}

bool localstoi(uint16_t& val, const std::string& line)
{
	try
	{
		val = CAST_UINT16(std::stoi(line));
	}
	catch (const std::invalid_argument& err)
	{
		print({ err.what() });
		return false;
	}
	return true;
}
bool localstoi(int& val, const std::string& line)
{
	try
	{
		val = std::stoi(line);
	}
	catch (const std::invalid_argument& err)
	{
		print({ err.what() });
		return false;
	}
	return true;
}
bool localstof(float& val, const std::string& line)
{
	try
	{
		val = std::stof(line);
	}
	catch (const std::invalid_argument& err)
	{
		print({ err.what() });
		return false;
	}
	return true;
}
bool localstol(size_t& val, const std::string& line)
{
	try
	{
		val = std::stol(line);
	}
	catch (const std::invalid_argument& err)
	{
		print({ err.what() });
		return false;
	}
	return true;
}
