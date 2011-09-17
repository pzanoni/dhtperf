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

#ifndef DHTPERF_CONTROLLER_PROFILES_NONE_H
#define DHTPERF_CONTROLLER_PROFILES_NONE_H

#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "../../protocol.h"

template <class EventType>
class None: public Profile<EventType>
{
    public:
	None(const std::string& params);
	dhtperfproto::timeUnit
	    scheduleNextEvent(dhtperfproto::timeUnit currentTime);
};

template <class EventType>
None<EventType>::None(const std::string& /*params*/)
{}

template <class EventType>
dhtperfproto::timeUnit
None<EventType>::scheduleNextEvent(dhtperfproto::timeUnit /*currentTime*/)
{
    return 0;
}

#endif
