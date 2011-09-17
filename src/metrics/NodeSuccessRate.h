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

#ifndef DHTNODE_METRICS_NODESUCCESSRATE_H
#define DHTNODE_METRICS_NODESUCCESSRATE_H

#include <string>

#include <boost/cstdint.hpp>

class NodeSuccessRate
{
    public:
	NodeSuccessRate();
	NodeSuccessRate(std::string fromString);
	void hit();
	void miss();
	int64_t count();
	double successRate();

	std::string toString();
    private:
	int64_t hits_;
	int64_t misses_;
};


#endif
