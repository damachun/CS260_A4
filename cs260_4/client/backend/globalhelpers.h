/* Start Header
*****************************************************************/
/*!
\file globalhelpers.h
\author Chloe Lim Jia-Han, 440003018
		Edwin Khoo Bing Shen, 390001918
		Javier Foo, 440002318

\par j.lim\@digipen.edu	b.khoo\@digipen.edu javier.foo\@digipen.edu
\date 18/7/2021
\brief 	the global print (and STRINGCONTAINER helper) function declaration(s)

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

#pragma once
#include <iostream>  // std::cout
#include <mutex>     // mutex, unique_lock, lock_guard
#include <string>    // string
#include <sstream>   // stringstream
#include <vector>    // vector
#include <exception> // exception
#include <chrono>    // now

using RAIILOCK = std::lock_guard<std::mutex>;
using NONRAIILOCK = std::unique_lock<std::mutex>;
using STRINGCONTAINER = std::vector<std::string>;

using CLOCKTIME = std::chrono::steady_clock::time_point;
CLOCKTIME gettime();
float     deltatime(const CLOCKTIME&, const CLOCKTIME&);

void print(const STRINGCONTAINER&);

#ifdef _DEBUG
	#define DBGPRINT(x) print(x)
#else
	#define DBGPRINT(x) ;
#endif

size_t stringcontsize(const STRINGCONTAINER&);
STRINGCONTAINER stringtostringcont(std::string, const std::string& delim = "\n");
std::string stringconttostring(const STRINGCONTAINER&);

std::string loadfile(const std::string&);
void printfile(const std::string& title, const std::string& text);

class globalException : public std::exception
{
	std::string _what;
public:
	globalException(const std::string&);
	virtual const char* what() const noexcept final;
};
#define THROW(x) throw globalException(x)

#define CAST_STR(x) std::to_string(x)
#define CAST_CHAR(x) static_cast<char>(x)
#define CAST_INT(x) static_cast<int>(x)
#define CAST_SIZET(x) static_cast<size_t>(x)
#define CAST_UINT16(x) static_cast<uint16_t>(x)
#define CAST_FLOAT(x) static_cast<float>(x)

bool localstoi(uint16_t&, const std::string&);
bool localstoi(int&, const std::string&);
bool localstof(float&, const std::string&);
bool localstol(size_t&, const std::string&);
