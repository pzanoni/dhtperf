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

#include <boost/foreach.hpp>

#include "GeneralLatency.h"

namespace bpt = boost::posix_time;

GeneralLatency::GeneralLatency(long int frequencyMapInterval)
    : generalStatistics_(frequencyMapInterval)
{}

void GeneralLatency::push(NodeLatency node)
{
    // Don't count nodes with no results:
    if (node.count() == 0)
	return;

    nodes_.push_back(node);
    meansStatistics_.push(node.meanInMicroseconds());

    BOOST_FOREACH(NodeLatency::MeasurementsMap::value_type measurement,
		  node.measurements()) {
	std::cout << "# " << measurement.second.total_microseconds() << "\n";
	generalStatistics_.push(measurement.second.total_microseconds());
    }
}

double GeneralLatency::meanOfMeansInMicroseconds() const
{
    return nodes_.size() ? meansStatistics_.mean() : 0.0;
}

double GeneralLatency::maxMeanInMicroseconds() const
{
    return nodes_.size() ? meansStatistics_.max() : 0.0;
}

double GeneralLatency::minMeanInMicroseconds() const
{
    return nodes_.size() ? meansStatistics_.min() : 0.0;
}

double GeneralLatency::meanInMicroseconds() const
{
    return nodes_.size() ? generalStatistics_.mean() : 0.0;
}

long int GeneralLatency::generalMaxInMicroseconds() const
{
    return nodes_.size() ? generalStatistics_.max() : 0;
}

long int GeneralLatency::generalMinInMicroseconds() const
{
    return nodes_.size() ? generalStatistics_.min() : 0;
}

long int GeneralLatency::count() const
{
    return nodes_.size() ? generalStatistics_.count() : 0;
}

std::map<long int, uintmax_t> GeneralLatency::getFrequencyMap(
    long int& interval) const
{
    return generalStatistics_.getFrequencyMap(interval);
}
