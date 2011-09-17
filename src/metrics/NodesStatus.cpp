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

#include "NodesStatus.h"

NodesStatus::NodesStatus()
    : finished_(0),
      failed_(0),
      unfinished_(0)
{}

void NodesStatus::set(const std::string& value)
{
    std::stringstream stream;
    stream << value;
    stream >> finished_ >> failed_ >> unfinished_;
}

int NodesStatus::finished()
{
    return finished_;
}

int NodesStatus::failed()
{
    return failed_;
}

int NodesStatus::unfinished()
{
    return unfinished_;
}
