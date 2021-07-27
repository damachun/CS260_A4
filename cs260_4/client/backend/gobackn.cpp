/* Start Header
*****************************************************************/
/*!
\file gobackn.cpp
\author Chloe Lim Jia-Han, 440003018

\par j.lim\@digipen.edu
\date 18/7/2021
\brief 	gobackn functions

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

#include "gobackn.h"
// REDO SOCK SEND AND RECV HERE


bool sendFrame(wrapSOCKET&, sockaddr&, const STRINGCONTAINER&, const int&, bool&);
bool recvFrame(wrapSOCKET&, sockaddr&, STRINGCONTAINER&, const int&, const float&, bool&, bool&);

STRINGCONTAINER breakdownContainer(const STRINGCONTAINER& original)
{
	STRINGCONTAINER brokendown;
	int numofframes = 0;
	std::string mainline;

	for (std::string line : original)
	{
		mainline += line;
	}

	while (!mainline.empty())
	{
		std::string index = std::to_string(numofframes / 10) +
			std::to_string(numofframes % 10);

		size_t linesize = mainline.size() < MAXSIZE ? mainline.size() : MAXSIZE;
		std::string linesizestr = std::to_string(linesize / 1000) +
			std::to_string((linesize / 100) % 10) +
			std::to_string((linesize / 10) % 10) +
			std::to_string(linesize % 10);

		brokendown.push_back(index + linesizestr + mainline.substr(0, linesize));
		mainline.erase(0, linesize);
		++numofframes;
	}

	brokendown.push_back(ENDACK); // end acknowledgement

	return brokendown;
}

bool sendWindow(wrapSOCKET& sock, sockaddr& address, const STRINGCONTAINER& tosend)
{
	if (tosend.empty()) return true;

	STRINGCONTAINER brokendown = breakdownContainer(tosend);
	bool run = true;

	//size_t numofwindows = (tosend.size() - 1) / WINDOWSIZE + 1;
	int framestart = 0, numofframes = CAST_INT(brokendown.size());

	while (framestart < numofframes && run)
	{
		if (sendFrame(sock, address, brokendown, framestart, run))
		{
			framestart += WINDOWSIZE;
		}
		else 
			continue;
	}

	return run;
}
bool sendFrame(wrapSOCKET& sock, sockaddr& address, const STRINGCONTAINER& tosend, const int& framestart, bool& run)
{
	int addresssize = CAST_INT(sizeof(address));
	int maxframes = (tosend.size() - framestart) > WINDOWSIZE ?
		WINDOWSIZE : CAST_INT(tosend.size() - framestart);
	char acknowledgement[MAXSIZE + 1] = { '\0' };

	// send frame
	for (int i = 0; i < maxframes; ++i)
	{
		if (sendto(sock.getsock(), tosend[framestart + i].c_str(),
			CAST_INT(tosend[framestart + i].size()), 0,
			&address, addresssize) == SOCKET_ERROR)
		{
			DBGPRINT({ "sendFrame() failed\n\tsendto() failed" });
			run = false;
			return false;
		}
	}

	// check ack
	int bytesrecv = recvfrom(sock.getsock(), acknowledgement,
		MAXSIZE, 0,
		&address, &addresssize);
	switch (bytesrecv)
	{
	case SOCKET_ERROR:
		DBGPRINT({ "sendFrame() failed\n\trecvfrom() failed" });
		run = false;
		return false;
	case 0:
		DBGPRINT({ "sendFrame() succeeded\n\trecvfrom() done" });
		run = false;
		return false;
	default:
	{
		std::string notice{ acknowledgement };

		if (notice.substr(0, 4) == SUCCESSACK)
			return true;

		if (notice.substr(0, 4) != ACK)
		{
			print({ notice });
			DBGPRINT({ "sendFrame() failed\n\tacknowledgement bits not received" });
			run = false;
			return false;
		}

		int index = -1;
		if (!localstoi(index, notice.substr(4, 2)))
		{
			print({ notice.substr(4, 2) });
			print({ "sendFrame() failed\n\tstoi (final frame number) crashed again :))))))))" });
			run = false;
			return false;
		}

		if (index != framestart + maxframes - 1)
		{
			print({ "sendFrame() failed\n\tout of order", "\n\tacknowldged ", notice.substr(4, 2), " instead of ", std::to_string(framestart + maxframes - 1) });
			return false;
		}

		memset(acknowledgement, '\0', HEADSIZE + 1);
	}
	break;
	}

	return true;
}

bool recvWindow(wrapSOCKET& sock, sockaddr& address, STRINGCONTAINER& cont, const float& droprate)
{
	int prevframe = -1;
	cont.clear();

	bool run = true, success = false;
	while (run)
	{
		if (recvFrame(sock, address, cont, prevframe, droprate, run, success))
		{
			prevframe += WINDOWSIZE;
		}
		else 
			continue;
	}

	return success;
}
bool recvFrame(wrapSOCKET& sock, sockaddr& address, STRINGCONTAINER& cont, const int& prevframe, const float& droprate, bool& run, bool& success)
{
	int addresssize = CAST_INT(sizeof(address));
	char buffer[WINDOWSIZE][MAXSIZE + 1] = { '\0' };
	int indices[WINDOWSIZE];
	for (int i = 0; i < WINDOWSIZE; ++i)
		indices[i] = 100;
	int maxframes = WINDOWSIZE;

	// recv data
	for (int i = 0; i < maxframes; ++i)
	{
		int bytesrecv = recvfrom(sock.getsock(), buffer[i],
			MAXSIZE, 0,
			&address, &addresssize);
		switch (bytesrecv)
		{
		case SOCKET_ERROR:
			DBGPRINT({ "recvFrame() failed\n\trecvfrom() (header) failed" });
			run = false;
			return false;
		case 0:
			DBGPRINT({ "recvFrame() succeeded\n\trecvfrom() (header) done" });
			run = false;
			return false;
		default:
		{
			std::string header{ buffer[i] };

			if (header == SUCCESSACK || header == ACK)
			{
				DBGPRINT({ "recvFrame() failed\n\tacknowledgement received :):):) a g a i n :):):)" });
				return false;
			}

			if (header == ENDACK)
			{
				indices[i] = prevframe + i + 1;
				maxframes = i + 1;
				success = true;
				run = false;
				break;
			}

			int index = -1;
			if (!localstoi(index, header.substr(0, 2)))
			{
				print({ header.substr(0, 2) });
				print({ "recvFrame() failed\n\tstoi (frame number) crashed again :))))))))" });
				return false;
			}

			indices[i] = index;
			size_t sizeofbuffer = 0;
			if (!localstol(sizeofbuffer, header.substr(2, 4)))
			{
				print({ header.substr(2, 4) });
				print({ "recvFrame() failed\n\tstol (size of buffer) crashed again :))))))))" });
				return false;
			}

			memcpy(buffer[i], buffer[i] + HEADSIZE, sizeofbuffer);
			memset(buffer[i] + sizeofbuffer, '\0', MAXSIZE + 1 - sizeofbuffer);
		}
		break;
		}
	}

	// send ack
	int indexsent = maxframes - 1;
	bool error = false;
	for (int i = 0; i < maxframes; ++i)
	{
		indexsent = indices[i];
		if ((indices[i] != prevframe + i + 1) || droprate > CAST_FLOAT(rand()) / RAND_MAX)
		{
			indexsent = prevframe + i + 1;
			maxframes = i + 1; // end the loop
			error = true;
		}
	}
	std::string acknowledgement = error ?
		ACK + std::to_string(indexsent / 10) + std::to_string(indexsent % 10) : SUCCESSACK;
	if (sendto(sock.getsock(), acknowledgement.c_str(),
		CAST_INT(acknowledgement.size()), 0,
		&address, addresssize) == SOCKET_ERROR)
	{
		DBGPRINT({ "recvFrame() failed\n\tsendto() failed" });
		run = false;
		return false;
	}

	if (!error)
	{
		for (int i = 0; i < maxframes; ++i)
		{
			if (strcmp(buffer[i], ENDACK) == 0) break;
			cont.push_back(buffer[i]);
		}
	}

	return !error && !cont.empty();
}