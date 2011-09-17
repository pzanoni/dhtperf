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

#ifndef DHTPERF_CONTROLLER_PROFILES_POISSONRANDOM_H
#define DHTPERF_CONTROLLER_PROFILES_POISSONRANDOM_H

#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "../../protocol.h"
#include "../Utils.h"

template <class EventType>
class PoissonRandom: public Profile<EventType>
{
    public:
	PoissonRandom(const std::string& params);
	~PoissonRandom();
	dhtperfproto::timeUnit
	    scheduleNextEvent(dhtperfproto::timeUnit currentTime);
    private:
	void pushElement(dhtperfproto::timeUnit time);

	dhtperfproto::timeUnit lastEvent_;
	Utils::RandomDictionary *dictionary_;
	Utils::PoissonRandomNumber *poisson_;
};

template <class EventType>
PoissonRandom<EventType>::PoissonRandom(const std::string& params)
{
    std::vector<std::string> paramsVector;
    dhtperfproto::timeUnit timeBegin;
    unsigned int setSize;
    double lambda;

    boost::split(paramsVector, params, boost::is_any_of("\t "));

    Utils::removeEmptyStringsFromVector(paramsVector);

    if (paramsVector.size() != 3) {
        std::cerr << "Error: poissonrandom nees 3 parameters: "
                  << " setSize, lambda and timeBegin\n";
        exit(EXIT_FAILURE);
    }

    setSize = boost::lexical_cast<unsigned int>(paramsVector[0]);
    lambda = boost::lexical_cast<double>(paramsVector[1]);
    timeBegin = boost::lexical_cast<dhtperfproto::timeUnit>(paramsVector[2]);

    dictionary_ = new Utils::RandomDictionary(setSize);
    poisson_ = new Utils::PoissonRandomNumber(lambda);

    lastEvent_ = timeBegin + poisson_->get();
    pushElement(lastEvent_);
}

template <class EventType>
PoissonRandom<EventType>::~PoissonRandom()
{
    delete dictionary_;
    delete poisson_;
}

template <class EventType>
dhtperfproto::timeUnit
PoissonRandom<EventType>::scheduleNextEvent(dhtperfproto::timeUnit
						/*currentTime*/)
{
    if (this->eventQueue_.getNextEventTime() == 0) {
	lastEvent_ = lastEvent_ + poisson_->get();
	pushElement(lastEvent_);
    }

    dhtperfproto::timeUnit ret = this->eventQueue_.getNextEventTime();
    std::cout << "PoissonRandom scheduleNext: " << ret << "\n";
    return ret;
}


template <class EventType>
void PoissonRandom<EventType>::pushElement(dhtperfproto::timeUnit time)
{
    this->eventQueue_.push(EventType(time));
}


template <>
void PoissonRandom<DhtPutEvent>::pushElement(dhtperfproto::timeUnit time);

template <>
void PoissonRandom<DhtGetEvent>::pushElement(dhtperfproto::timeUnit time);

#endif
