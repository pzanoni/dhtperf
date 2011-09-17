/*
 * dhtperf: DHT performance evaluation tool
 * Copyright (C) 2011 Paulo Zanoni <przanoni@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef DHTPERF_EVENTS_H
#define DHTPERF_EVENTS_H

#include <list>
#include <queue>
#include <string>

#include "protocol.h"

// DhtEvent
class DhtEvent
{
    public:
	DhtEvent(dhtperfproto::timeUnit time);

	dhtperfproto::timeUnit time;

	bool operator>(const DhtEvent e) const;
};

// DhtJoinEvent
class DhtJoinEvent : public DhtEvent
{
    public:
	//DhtJoinEvent(dhtperfproto::timeUnit time, std::string knownNodeIp);
	//std::string knownNodeIp;
	DhtJoinEvent(dhtperfproto::timeUnit time);
};

// DhtLeaveEvent
class DhtLeaveEvent : public DhtEvent
{
    public:
	DhtLeaveEvent(dhtperfproto::timeUnit time);
};

// DhtPutEvent
class DhtPutEvent : public DhtEvent
{
    public:
	DhtPutEvent(dhtperfproto::timeUnit time, const std::string& key,
		    const std::string& value);
	std::string key;
	std::string value;
};

// DhtGetEvent
class DhtGetEvent : public DhtEvent
{
    public:
	DhtGetEvent(dhtperfproto::timeUnit time, const std::string& key);
	std::string key;
};

// EventQueue
template <class T>
class EventQueue
{
    public:
	void push(const T& element);
	dhtperfproto::timeUnit getNextEventTime();
	std::list<T> popEvents(dhtperfproto::timeUnit currentTime);
    private:
	T pop();
	std::priority_queue<T, std::vector<T>, std::greater<T> > queue;
};

template <class T>
void EventQueue<T>::push(const T& element)
{
    queue.push(element);
}

template <class T>
T EventQueue<T>::pop()
{
    T ret = queue.top();
    queue.pop();
    return ret;
}

template <class T>
dhtperfproto::timeUnit EventQueue<T>::getNextEventTime()
{
    if (queue.empty())
        return 0;
    else
        return queue.top().time;
}

template <class T>
std::list<T> EventQueue<T>::popEvents(dhtperfproto::timeUnit currentTime)
{
    std::list<T> ret;

    while (!queue.empty() && queue.top().time <= currentTime) {
	ret.push_back(pop());
    }
    return ret;
}


#endif
