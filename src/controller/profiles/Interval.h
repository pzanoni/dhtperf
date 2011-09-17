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

#ifndef DHTPERF_CONTROLLER_PROFILES_INTERVAL_H
#define DHTPERF_CONTROLLER_PROFILES_INTERVAL_H

#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "../../protocol.h"
#include "../Utils.h"

template <class EventType>
class Interval: public Profile<EventType>
{
    public:
	Interval(const std::string& params);
	~Interval();
	dhtperfproto::timeUnit
	    scheduleNextEvent(dhtperfproto::timeUnit currentTime);
    private:
	void pushElement(dhtperfproto::timeUnit time);
	Utils::RandomDictionary *dictionary_;

	dhtperfproto::timeUnit lastEventTime_;
	dhtperfproto::timeUnit frequency_;
	unsigned int repetitions_;
};

template <class EventType>
Interval<EventType>::Interval(const std::string& params)
{
    std::vector<std::string> paramsVector;
    dhtperfproto::timeUnit startTime;
    unsigned int setSize;

    boost::split(paramsVector, params, boost::is_any_of("\t "));

    Utils::removeEmptyStringsFromVector(paramsVector);

    if (paramsVector.size() != 4) {
	std::cerr << "Profile Interval requires 4 paramsters: setSize, "
		     "repetitions, frequency, startTime" << std::endl;
	exit(EXIT_FAILURE);
    }

    setSize = boost::lexical_cast<unsigned int>(paramsVector[0]);
    repetitions_ = boost::lexical_cast<unsigned int>(paramsVector[1]);
    frequency_ = boost::lexical_cast<unsigned int>(paramsVector[2]);
    startTime = Utils::getTimeFromString(paramsVector[3]);

    dictionary_ = new Utils::RandomDictionary(setSize);

    lastEventTime_ = startTime;
    pushElement(startTime);
}

template <class EventType>
Interval<EventType>::~Interval()
{
    delete dictionary_;
}

template <class EventType>
dhtperfproto::timeUnit
Interval<EventType>::scheduleNextEvent(dhtperfproto::timeUnit /*currentTime*/)
{
    if (this->eventQueue_.getNextEventTime() == 0) {
	lastEventTime_ = lastEventTime_ + frequency_;
	pushElement(lastEventTime_);
    }
    dhtperfproto::timeUnit ret = this->eventQueue_.getNextEventTime();
    std::cout << "Interval scheduleNext: " << ret << "\n";
    return ret;
}


template <class EventType>
void Interval<EventType>::pushElement(dhtperfproto::timeUnit /*time*/)
{
    std::cerr << "Interval is only implemented for Put and Get events!\n";
    exit(1);
}


template <>
void Interval<DhtPutEvent>::pushElement(dhtperfproto::timeUnit time);

template <>
void Interval<DhtGetEvent>::pushElement(dhtperfproto::timeUnit time);

#endif
