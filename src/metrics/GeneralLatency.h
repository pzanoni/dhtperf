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

#ifndef DHTPERF_METRICS_GENERALLATENCY_H
#define DHTPERF_METRICS_GENERALLATENCY_H

#include "NodeLatency.h"
#include "Statistics.h"

class GeneralLatency
{
    public:
	GeneralLatency(long int frequencyMapInterval);
        void push(NodeLatency node);
        double meanOfMeansInMicroseconds() const;
        double maxMeanInMicroseconds() const;
        double minMeanInMicroseconds() const;
	double meanInMicroseconds() const;
	long int generalMaxInMicroseconds() const;
	long int generalMinInMicroseconds() const;
	long int count() const;
	std::map<long int, uintmax_t> getFrequencyMap(long int& interval) const;

    private:
        std::vector<NodeLatency> nodes_;
	Statistics<double>   meansStatistics_;
	Statistics<long int> generalStatistics_;
};

#endif
