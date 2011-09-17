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

// Profile.h includes Exact.h
#include "../Profile.h"

template <>
void Exact<DhtPutEvent>::pushElement(const std::vector<std::string>& params,
				     unsigned int& i)
{
    dhtperfproto::timeUnit time;
    std::string key, value;

    time  = Utils::getTimeFromString(params[i]);
    key   = params[i+1];
    value = params[i+2];
    eventQueue_.push(DhtPutEvent(time, key, value));
    i+=3;
}

template <>
void Exact<DhtGetEvent>::pushElement(const std::vector<std::string>& params,
				     unsigned int& i)
{
    dhtperfproto::timeUnit time;
    std::string key;

    time  = Utils::getTimeFromString(params[i]);
    key   = params[i+1];
    eventQueue_.push(DhtGetEvent(time, key));
    i+=2;
}

