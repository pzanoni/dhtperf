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

#include <sstream>

#include "NodeSuccessRate.h"

NodeSuccessRate::NodeSuccessRate()
    : hits_(0),
      misses_(0)
{}

NodeSuccessRate::NodeSuccessRate(std::string fromString)
    : hits_(0),
      misses_(0)
{
    std::stringstream tmp(fromString);

    tmp >> hits_;
    tmp >> misses_;
}

void NodeSuccessRate::hit()
{
    hits_++;
}

void NodeSuccessRate::miss()
{
    misses_++;
}

int64_t NodeSuccessRate::count()
{
    // May overflow!
    return hits_ + misses_;
}

double NodeSuccessRate::successRate()
{
    // Don't divide by zero
    if (misses_ + hits_ == 0)
	return 0;
    else
	return ((double)hits_) / (misses_ + hits_);
}

std::string NodeSuccessRate::toString()
{
    std::stringstream ret;
    ret << hits_ << " " << misses_;
    return ret.str();
}
