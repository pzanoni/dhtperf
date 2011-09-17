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

#include <iostream>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>

#include "NodeLatency.h"

namespace bpt = boost::posix_time;

NodeLatency::NodeLatency()
{}

NodeLatency::NodeLatency(std::string fromString)
{
    std::vector<std::string> subStrings;

    boost::split(subStrings, fromString, boost::is_any_of(" "));

    for(unsigned int i = 0; i < subStrings.size(); i+=2) {

	if (subStrings[i].empty())
	    continue;

	bpt::ptime time = bpt::from_iso_string(subStrings[i]);

	if (i+1 == subStrings.size()) {
	    std::cerr << "Error creating NodeLatency from string: invalid "
			 "string\n";
	    exit(1);
	}
	bpt::time_duration duration(bpt::duration_from_string(subStrings[i+1]));

	push_with_time(time, duration);
    }
}

void NodeLatency::push(bpt::time_duration duration)
{
    push_with_time(bpt::microsec_clock::universal_time(), duration);
}

void NodeLatency::push_with_time(bpt::ptime time, bpt::time_duration duration)
{
    measurements_[time] = duration;
    statistics_.push(duration.total_microseconds());
}

double NodeLatency::meanInMicroseconds()
{
    return count() ? statistics_.mean() : 0.0;
}

long int NodeLatency::maxInMicroseconds()
{
    return count() ? statistics_.max() : 0;
}

long int NodeLatency::minInMicroseconds()
{
    return count() ? statistics_.min() : 0;
}

long int NodeLatency::count()
{
    return statistics_.count();
}

NodeLatency::MeasurementsMap NodeLatency::measurements()
{
    return measurements_;
}

std::string NodeLatency::toString()
{
    std::string ret;

    bool first = true;
    BOOST_FOREACH(MeasurementsMap::value_type measurement, measurements_) {
	if (first)
	    first = false;
	else
	    ret += std::string(" ");

	ret += bpt::to_iso_string(measurement.first) + std::string(" ");
	ret += bpt::to_simple_string(measurement.second);
    }

    return ret;
}
