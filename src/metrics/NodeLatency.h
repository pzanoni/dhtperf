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

#ifndef DHTPERF_METRICS_NODELATENCY_H
#define DHTPERF_METRICS_NODELATENCY_H

#include <string>

#include <boost/date_time/posix_time/posix_time.hpp>

#include "Statistics.h"

class NodeLatency
{
    public:
	typedef std::map<boost::posix_time::ptime,
			 boost::posix_time::time_duration> MeasurementsMap;

	NodeLatency();
	NodeLatency(std::string fromString);

	void push(boost::posix_time::time_duration duration);
	double meanInMicroseconds();
	long int maxInMicroseconds();
	long int minInMicroseconds();
	long int count();
	MeasurementsMap measurements();

	std::string toString();

    private:
	void push_with_time(boost::posix_time::ptime time,
			    boost::posix_time::time_duration duration);

	MeasurementsMap measurements_;
	Statistics<long int> statistics_;
};

#endif
