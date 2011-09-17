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

#include "DummyDht.h"

DummyDht::DummyDht()
    : DhtNode()
{}

DummyDht::~DummyDht()
{
    std::cout << "~DummyDht()\n";
}

bool DummyDht::doJoin()
{
    if (nodeStartsNewDht())
	std::cout << "Creating my own dht";
    else
	std::cout << "Joining the DHT using entry point " << entryPointHost_
		  << ":" << entryPointPort_;

    std::cout << " at port " << selfPort_ << "\n";
    return true;
}

void DummyDht::doLeave()
{
    std::cout << "Leaving the dht!\n";
}

void DummyDht::doPut(const DhtPutEvent& /*event*/)
{}

bool DummyDht::doGet(const DhtGetEvent& /*event*/)
{
    return false;
}

uint16_t DummyDht::portOffsetToPort(uint16_t offset)
{
    return offset;
}
