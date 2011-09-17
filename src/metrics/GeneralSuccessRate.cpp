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

#include "GeneralSuccessRate.h"

void GeneralSuccessRate::push(NodeSuccessRate node)
{
    // Don't count nodes with no results:
    if (node.count() == 0)
	return;

    nodes_.push_back(node);
    statistics_.push(node.successRate());
}

double GeneralSuccessRate::mean() const
{
    return statistics_.count() ? statistics_.mean() : 0.0;
}

double GeneralSuccessRate::min() const
{
    return statistics_.count() ? statistics_.min() : 0.0;
}

double GeneralSuccessRate::max() const
{
    return statistics_.count() ? statistics_.max() : 0.0;
}

