/* Start Header
*****************************************************************/
/*!
\file producerconsumer.hpp
\author Chloe Lim Jia-Han, 440003018
		Edwin Khoo Bing Shen, 390001918
		Javier Foo, 440002318

\par j.lim\@digipen.edu	b.khoo\@digipen.edu javier.foo\@digipen.edu
\date 18/7/2021
\brief 	the member definitions of the producer consumer pattern

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

#pragma once

// public
	// ctor dtor
template <typename TItem, typename TAction, typename TOnDisconnect>
taskQueue<TItem, TAction, TOnDisconnect>::
	taskQueue(const size_t& wrkrthrdcnt, 
		TAction& actn, TOnDisconnect& discnnct):
	// threads
	_workers{ },
	// items
	_itemqueue{ }, _mutexitem{ },
	// item slots count
	_countslot{ wrkrthrdcnt }, _mutexslot{ }, _condslot{ },
	// available items count
	_countavail{ 0 }, _mutexavail{ }, _condavail{ },
	// status
	_active{ true }, _ondisconnect{ discnnct }
{
	for (size_t i = 0; i < wrkrthrdcnt; ++i) // create thread and put to sleep
		_workers.emplace_back(&work, std::ref(*this), std::ref(actn));

	//print({"taskQueue::taskQueue() done"});
}
template <typename TItem, typename TAction, typename TOnDisconnect>
taskQueue<TItem, TAction, TOnDisconnect>::~taskQueue()
{
	disconnect();
	for (std::thread& worker : _workers)
	{
		worker.join();

		// tedious omg
		std::stringstream ss;
		ss << worker.get_id();
		//print({ "taskQueue::~taskQueue() details\n\t", ss.str(), " joined" });
	}

	//print({ "taskQueue::~taskQueue() done" });
}
	// used outside
template <typename TItem, typename TAction, typename TOnDisconnect>
void taskQueue<TItem, TAction, TOnDisconnect>::produce(TItem soloitem)
{
	/*
		produce will wait for an available slot to introduce
		the item into the buffer. when there is available slot,
			1. decrements the number of available slots
			2. introduces the item into the buffer
			3. increments the number of available items
	*/

	// keep in scopes to auto destruct after use

	// get an available slot and reflect the appropriate stats (slots)
	{
		NONRAIILOCK lockslot{ _mutexslot }; // create lock
			// acquire lock and wait for condition to pass
			// NOTE: _CONDSLOT (producers) LOCKED HERE
		_condslot.wait(lockslot, [&]() { return _countslot > 0; });
		--_countslot; // condition has passed

		/*print({"taskQueue::produce() details\n\t_countslot decremented to ",
			std::to_string(_countslot),
			"\n\t_condslot (producers) locked" });*/
	}

	// add item to the item buffer
	{
		RAIILOCK lockitems{ _mutexitem }; 
		_itemqueue.push(soloitem);

		//print({"taskQueue::produce() details\n\t_itemqueue added one"});
	}

	// reflect appropriate stats (avail)
	{
		RAIILOCK lockavail{ _mutexavail };
		++_countavail;
		_condavail.notify_one(); // NOTE: _CONDAVAIL (consumers) FREED HERE

		/*print({ "taskQueue::produce() details\n\t_countavail incremented to ",
			std::to_string(_countavail),
			"\n\t_condavail (consumers) freed" });*/
	}
}
template <typename TItem, typename TAction, typename TOnDisconnect>
std::optional<TItem> taskQueue<TItem, TAction, TOnDisconnect>::consume()
{
	// keep the object to consume
	std::optional<TItem> consumeditem = std::nullopt;

	// get available item and reflect the appropriate stats (avail)
	{
		NONRAIILOCK lockavail{ _mutexavail }; // create lock
			// NOTE: _CONDAVAIL (consumers) LOCKED HERE
		_condavail.wait(lockavail, [&]()
			{ return (_countavail > 0) || (!_active); });
		if (_countavail == 0) // if got nothing
		{
			_condavail.notify_one(); // NOTE: _CONDAVAIL (consumers) FREED HERE

			/*print({"taskQueue::consume() details\n\t_countavail is 0",
				"\n\t_condavail (consumers) locked and freed" });*/
			return consumeditem; // just std::nullopt
		}
		--_countavail;

		/*print({ "taskQueue::consume() details\n\t_countavail decremented to ",
			std::to_string(_countavail),
			"\n\t_condavail (consumers) locked" });*/
	}

	// remove item from item buffer
	{
		RAIILOCK lockitems{ _mutexitem };
		consumeditem = _itemqueue.front();
		_itemqueue.pop();

		//print({ "taskQueue::consume() details\n\t_itemqueue removed one" });
	}

	// reflect appropriate stats (slots)
	{
		RAIILOCK lockslot{ _mutexslot };
		++_countslot;
		_condslot.notify_one(); // NOTE: _CONDSLOT (producers) FREED HERE

		/*print({ "taskQueue::produce() details\n\t_countslot incremented to ",
			std::to_string(_countslot),
			"\n\t_condslot (producers) freed" });*/
	}

	return consumeditem;
}

// private
template <typename TItem, typename TAction, typename TOnDisconnect>
void taskQueue<TItem, TAction, TOnDisconnect>::work(
	taskQueue<TItem, TAction, TOnDisconnect>& taskqueue,
	TAction& action)
{
	// tedious help
	std::stringstream ss;
	ss << std::this_thread::get_id();

	// infinite loop
	while (true)
	{
		print({ "taskQueue::work() details\n\tthread ",
			ss.str(), " is waiting for a task" });

		// thread will wait for task
		std::optional<TItem> item = taskqueue.consume();
		if (!item) // exit this thread because there is nothing
		{
			print({ "taskQueue::work() details\n\tthread ",
				ss.str(), " is exiting\n\tthread is idle and thus terminating" });
			break;
		}

		// thread has something

		print({ "taskQueue::work() details\n\tthread ",
			ss.str(), " is ready and going to call action" });

			// execute task
		if (!action(*item)) // item says to terminate
		{
			taskqueue.disconnect(); // terminate workers

			print({ "taskQueue::work() details\n\tthread ",
				ss.str(), " is exiting\n\taction returned false\n\ttaskQueue::disconnect() called" });
		}

#ifdef _DEBUG
		print({ "taskQueue::work() details\n\tthread ",
				ss.str(), " is continuing\n\taction returned true" });
#endif
	}

	print({ "taskQueue::work() details\n\tthread ",
		ss.str(), " is exiting\n\twork is done" });
}
template <typename TItem, typename TAction, typename TOnDisconnect>
void taskQueue<TItem, TAction, TOnDisconnect>::disconnect()
{
	_active = false;
	_ondisconnect();

	// tedious crying
#ifdef _DEBUG
	std::stringstream ss;
	ss << std::this_thread::get_id();
	print({ "taskQueue::disconnect() succeeded\n\tthread ",
		ss.str(), " is exiting\n\t_ondisconnect() is called" });
#endif
}
