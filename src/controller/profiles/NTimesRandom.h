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

#ifndef DHTPERF_CONTROLLER_PROFILES_NTIMESRANDOM_H
#define DHTPERF_CONTROLLER_PROFILES_NTIMESRANDOM_H

#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "../../protocol.h"
#include "../Utils.h"

template <class EventType>
class NTimesRandom: public Profile<EventType>
{
    public:
	NTimesRandom(const std::string& params);
	~NTimesRandom();
	dhtperfproto::timeUnit
	    scheduleNextEvent(dhtperfproto::timeUnit currentTime);
    private:
	void pushElement(dhtperfproto::timeUnit time,
			 unsigned int repetitions);
	Utils::RandomDictionary *dictionary_;
};

template <class EventType>
NTimesRandom<EventType>::NTimesRandom(const std::string& params)
{
    unsigned int i;
    std::vector<std::string> paramsVector;
    dhtperfproto::timeUnit time;
    unsigned int repetitions, setSize;

    boost::split(paramsVector, params, boost::is_any_of("\t "));

    Utils::removeEmptyStringsFromVector(paramsVector);

    setSize = boost::lexical_cast<unsigned int>(paramsVector[0]);
    dictionary_ = new Utils::RandomDictionary(setSize);

    for (i = 1; i < paramsVector.size(); i += 2) {
	time = boost::lexical_cast<dhtperfproto::timeUnit>(paramsVector[i]);
	repetitions =
	    boost::lexical_cast<dhtperfproto::timeUnit>(paramsVector[i+1]);
	pushElement(time, repetitions);
    }
}

template <class EventType>
NTimesRandom<EventType>::~NTimesRandom()
{
    delete dictionary_;
}

template <class EventType>
dhtperfproto::timeUnit
NTimesRandom<EventType>::scheduleNextEvent(dhtperfproto::timeUnit /*currentTime*/)
{
    dhtperfproto::timeUnit ret = this->eventQueue_.getNextEventTime();
    std::cout << "NTimesRandom scheduleNext: " << ret << "\n";
    return ret;
}


template <class EventType>
void NTimesRandom<EventType>::pushElement(dhtperfproto::timeUnit /*time*/,
					  unsigned int /*repetitions*/)
{
    std::cerr << "NTimesRandom is only implemented for Put and Get events!\n";
    exit(1);
}


template <>
void NTimesRandom<DhtPutEvent>::pushElement(dhtperfproto::timeUnit time,
					    unsigned int repetitions);

template <>
void NTimesRandom<DhtGetEvent>::pushElement(dhtperfproto::timeUnit time,
					    unsigned int repetitions);

#endif
