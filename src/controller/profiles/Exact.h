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

#ifndef DHTPERF_CONTROLLER_PROFILES_EXACT_H
#define DHTPERF_CONTROLLER_PROFILES_EXACT_H

#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>

#include "../../protocol.h"
#include "../Utils.h"


template <class EventType>
class Exact: public Profile<EventType>
{
    public:
	Exact(const std::string& params);
	dhtperfproto::timeUnit
	    scheduleNextEvent(dhtperfproto::timeUnit currentTime);
    private:
	void pushElement(const std::vector<std::string>& params,
			 unsigned int& i);
};

template <class EventType>
Exact<EventType>::Exact(const std::string& params)
{
    unsigned int i;
    std::vector<std::string> paramsVector;

    boost::split(paramsVector, params, boost::is_any_of("\t "));

    Utils::removeEmptyStringsFromVector(paramsVector);

    i = 0;
    while (i < paramsVector.size()) {
	pushElement(paramsVector, i);
    }
}

template <class EventType>
dhtperfproto::timeUnit
Exact<EventType>::scheduleNextEvent(dhtperfproto::timeUnit /*currentTime*/)
{
    dhtperfproto::timeUnit ret = this->eventQueue_.getNextEventTime();
    std::cout << "Exact scheduleNext: " << ret << "\n";
    return ret;
}


template <class EventType>
void Exact<EventType>::pushElement(const std::vector<std::string>& params,
				   unsigned int& i)
{
    dhtperfproto::timeUnit time;

    time = Utils::getTimeFromString(params[i]);
    this->eventQueue_.push(EventType(time));
    i++;
}


template <>
void Exact<DhtPutEvent>::pushElement(const std::vector<std::string>& params,
				     unsigned int& i);

template <>
void Exact<DhtGetEvent>::pushElement(const std::vector<std::string>& params,
				     unsigned int& i);

#endif
