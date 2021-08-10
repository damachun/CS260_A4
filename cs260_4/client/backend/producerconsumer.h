/* Start Header
*****************************************************************/
/*!
\file producerconsumer.h
\author Chloe Lim Jia-Han, 440003018
		Edwin Khoo Bing Shen, 390001918
		Javier Foo, 440002318

\par j.lim\@digipen.edu	b.khoo\@digipen.edu javier.foo\@digipen.edu
\date 18/7/2021
\brief 	the class declaration of the producer consumer pattern

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

#pragma once

#include <vector>             // vector
#include <queue>              // queue
#include <condition_variable> // condition_variable
#include <optional>           // optional
#include "globalhelpers.h"    // printing

// code is attempting to follow what is taught

template <typename TItem,   // individual client object
	typename TAction,       // game UPDATE loop for individual client (function)
	typename TOnDisconnect> // terminate individual client object (function)
class taskQueue
{
private:
	// threads
	std::vector<std::thread> _workers;      // pool of worker threads
	// items
	std::queue<TItem>        _itemqueue;    // buffer of slots for items
	std::mutex               _mutexitem;    // sync object for item buffer
	// item slots count
	size_t                   _countslot;    // number of slots for items
	std::mutex               _mutexslot;    // sync object for slot count
	std::condition_variable  _condslot;     // cond to unblock thread for slot
	// available items count
	size_t                   _countavail;   // number of available items
	std::mutex               _mutexavail;   // sync object for avail item count
	std::condition_variable  _condavail;    // cond to unblock thread for avail
	// status
	volatile bool            _active;       // stay alive or nah
	TOnDisconnect&           _ondisconnect; // terminate client obj function

	// functions
	static void work(
		taskQueue<TItem, TAction, TOnDisconnect>&, 
		TAction&); // main thing the thread does
	void disconnect();    // ends everything

public:
		// let num of slots of equate to num of worker threads
	taskQueue(const size_t&, TAction&, TOnDisconnect&);
	~taskQueue();

	void produce(TItem); // store item in task queue
	std::optional<TItem> consume(); // get available item from task queue
};

// implementation
#include "producerconsumer.hpp"