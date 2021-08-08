/* Start Header
*****************************************************************/
/*!
\file hash.h
\author Chloe Lim Jia-Han, 440003018
		Edwin Khoo Bing Shen, 390001918

\par j.lim\@digipen.edu	b.kohh\@digipen.edu
\date 20/6/2021
\brief 	the hash function

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

#pragma once
#include <string>

unsigned hashfn( const std::string& input )
{
	unsigned uhash = 0;

	const char* curr = input.c_str();
	const char* const end = curr + input.size();

	while( curr < end )
	{
		unsigned utemp;

		std::memcpy( &utemp, curr, 4 );

		uhash ^= utemp;
		uhash = uhash << 13 | uhash >> 19;

		curr += 4;
	}

	if(curr != end )
	{
		unsigned utemp;

		std::memcpy( &utemp, end - 4, 4 );

		uhash ^= utemp;
		uhash = uhash << 13 | uhash >> 19;

		curr += 4;
	}

	//while( curr != end )
	//{
	//	uhash *= 53;
	//	uhash += *curr;
	//	++uhash;
	//}

	return uhash;
}
