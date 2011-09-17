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

#ifndef DHTPERF_CONTROLLER_PROFILES_WEIBULLRANDOM_H
#define DHTPERF_CONTROLLER_PROFILES_WEIBULLRANDOM_H

#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "../../protocol.h"
#include "../Utils.h"

template <class EventType>
class WeibullRandom: public Profile<EventType>
{
    public:
	WeibullRandom(const std::string& params);
	~WeibullRandom();
	dhtperfproto::timeUnit
	    scheduleNextEvent(dhtperfproto::timeUnit currentTime);
    private:
	void pushElement(dhtperfproto::timeUnit time);

	Utils::RandomDictionary *dictionary_;
	Utils::WeibullRandomNumber *weibull_;
};

template <class EventType>
WeibullRandom<EventType>::WeibullRandom(const std::string& params)
{
    std::vector<std::string> paramsVector;
    unsigned int setSize;
    double shape, scale;

    boost::split(paramsVector, params, boost::is_any_of("\t "));

    Utils::removeEmptyStringsFromVector(paramsVector);

    if (paramsVector.size() != 3) {
	std::cerr << "Error: weibullrandom needs 3 parameters: "
		  << " setSize, shape and scale\n";
	exit(EXIT_FAILURE);
    }

    setSize = boost::lexical_cast<unsigned int>(paramsVector[0]);
    shape = boost::lexical_cast<double>(paramsVector[1]);
    scale = boost::lexical_cast<double>(paramsVector[2]);

    dictionary_ = new Utils::RandomDictionary(setSize);
    weibull_ = new Utils::WeibullRandomNumber(shape, scale);
}

template <class EventType>
WeibullRandom<EventType>::~WeibullRandom()
{
    delete dictionary_;
    delete weibull_;
}

template <class EventType>
dhtperfproto::timeUnit
WeibullRandom<EventType>::scheduleNextEvent(dhtperfproto::timeUnit currentTime)
{
    // If the queue is empty, schedule the next event based on the current time
    if (this->eventQueue_.getNextEventTime() == 0) {
	pushElement(currentTime + (int)round(weibull_->get()));
    }

    dhtperfproto::timeUnit ret = this->eventQueue_.getNextEventTime();
    std::cout << "WeibullRandom scheduleNext: " << ret << "\n";
    return ret;
}


template <class EventType>
void WeibullRandom<EventType>::pushElement(dhtperfproto::timeUnit time)
{
    this->eventQueue_.push(EventType(time));
}


template <>
void WeibullRandom<DhtPutEvent>::pushElement(dhtperfproto::timeUnit time);

template <>
void WeibullRandom<DhtGetEvent>::pushElement(dhtperfproto::timeUnit time);

#endif
