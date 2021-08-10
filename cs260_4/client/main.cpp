/* Start Header
*****************************************************************/
/*!
\file windows.h
\author Chloe Lim Jia-Han, 440003018
		Edwin Khoo Bing Shen, 390001918
		Javier Foo, 440002318

\par j.lim\@digipen.edu	b.khoo\@digipen.edu javier.foo\@digipen.edu
\date 18/7/2021
\brief 	the main function

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

#include "assignment.h"

int main(int argc, char** argv)
{
	try
	{
		assignment cs260_a4{ argc, argv };
		cs260_a4.update();
	}
	catch (const globalException& controlled)
	{
		print({ "Controlled Exception Thrown\n", controlled.what() });
	}
	catch (const std::exception& external)
	{
		print({ "External Exception Thrown\n", external.what() });
	}
	catch (...)
	{
		print({ "Undefined Exception Thrown\n", "Something appears to have broken the client." });
	}

	return 0;
}