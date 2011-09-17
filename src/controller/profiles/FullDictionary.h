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

#ifndef DHTPERF_CONTROLLER_PROFILES_FULLDICTIONARY_H
#define DHTPERF_CONTROLLER_PROFILES_FULLDICTIONARY_H

#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "../../protocol.h"
#include "../Utils.h"

template <class EventType>
class FullDictionary: public Profile<EventType>
{
    public:
	FullDictionary(const std::string& params);
	dhtperfproto::timeUnit
	    scheduleNextEvent(dhtperfproto::timeUnit currentTime);
    private:
	void pushElement(dhtperfproto::timeUnit time, const char *key);
};

template <class EventType>
FullDictionary<EventType>::FullDictionary(const std::string& params)
{
    unsigned int i, j;
    std::vector<std::string> paramsVector;
    dhtperfproto::timeUnit time;
    unsigned int setSize;

    boost::split(paramsVector, params, boost::is_any_of("\t "));

    Utils::removeEmptyStringsFromVector(paramsVector);

    setSize = boost::lexical_cast<unsigned int>(paramsVector[0]);
    if (setSize > Utils::dictionarySize) {
	std::cerr << "Error: maximum dictionary size is "
		  << Utils::dictionarySize << "\n";
	exit(EXIT_FAILURE);
    }

    for (i = 1; i < paramsVector.size(); i ++) {
	time = Utils::getTimeFromString(paramsVector[i]);
	for(j = 0; j < setSize; j++) {
	    pushElement(time, Utils::dictionary[j]);
	}
    }
}

template <class EventType>
dhtperfproto::timeUnit
FullDictionary<EventType>::scheduleNextEvent(dhtperfproto::timeUnit /*currentTime*/)
{
    dhtperfproto::timeUnit ret = this->eventQueue_.getNextEventTime();
    std::cout << "FullDictionary scheduleNext: " << ret << "\n";
    return ret;
}


template <class EventType>
void FullDictionary<EventType>::pushElement(dhtperfproto::timeUnit /*time*/,
					    const char* /*key*/)
{
    std::cerr << "FullDictionary is only implemented for Put and Get events!\n";
    exit(EXIT_FAILURE);
}


template <>
void FullDictionary<DhtPutEvent>::pushElement(dhtperfproto::timeUnit time,
					      const char *key);

template <>
void FullDictionary<DhtGetEvent>::pushElement(dhtperfproto::timeUnit time,
					      const char *key);

#endif
