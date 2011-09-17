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

#ifndef DHTPERF_CONTROLLER_PROFILES_NOW_H
#define DHTPERF_CONTROLLER_PROFILES_NOW_H

#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>

#include "../../protocol.h"
#include "../Utils.h"

template <class EventType>
class Now: public Profile<EventType>
{
    public:
	Now(const std::string& params);
	dhtperfproto::timeUnit
	    scheduleNextEvent(dhtperfproto::timeUnit currentTime);
    private:
	void pushElement(dhtperfproto::timeUnit time);
};

template <class EventType>
Now<EventType>::Now(const std::string& params)
{
    std::vector<std::string> paramsVector;

    boost::split(paramsVector, params, boost::is_any_of("\t "));

    Utils::removeEmptyStringsFromVector(paramsVector);

    if (paramsVector.size() != 1) {
	std::cerr << "Profile now only allows one paramater: minimum time"
		  << std::endl;
	exit(EXIT_FAILURE);
    }

    pushElement(Utils::getTimeFromString(paramsVector[0]));
}

template <class EventType>
dhtperfproto::timeUnit
Now<EventType>::scheduleNextEvent(dhtperfproto::timeUnit currentTime)
{
    if (this->eventQueue_.getNextEventTime() == 0) {
	pushElement(currentTime);
    }
    dhtperfproto::timeUnit ret = this->eventQueue_.getNextEventTime();
    std::cout << "Now scheduleNext: " << ret << "\n";
    return ret;
}


template <class EventType>
void Now<EventType>::pushElement(dhtperfproto::timeUnit time)
{
    this->eventQueue_.push(EventType(time));
}


template <>
void Now<DhtPutEvent>::pushElement(dhtperfproto::timeUnit time);

template <>
void Now<DhtGetEvent>::pushElement(dhtperfproto::timeUnit time);

#endif
