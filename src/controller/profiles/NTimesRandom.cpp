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

// Profile.h includes NTimesRandom.h
#include "../Profile.h"

namespace dp = dhtperfproto;

template <>
void NTimesRandom<DhtPutEvent>::pushElement(dp::timeUnit time,
					    unsigned int repetitions)
{
    std::string key, value;

    for(unsigned int i = 0; i < repetitions; i++) {
	key = dictionary_->getRandomWord();
	value = key + std::string("_value");
	eventQueue_.push(DhtPutEvent(time, key, value));
    }
}

template <>
void NTimesRandom<DhtGetEvent>::pushElement(dp::timeUnit time,
					    unsigned int repetitions)
{
    std::string key;

    for(unsigned int i = 0; i < repetitions; i++) {
	key = dictionary_->getRandomWord();
	eventQueue_.push(DhtGetEvent(time, key));
    }
}

