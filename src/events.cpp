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

#include "events.h"

// DhtEvent
DhtEvent::DhtEvent(dhtperfproto::timeUnit time)
    : time(time)
{}

bool DhtEvent::operator>(const DhtEvent e) const
{
    return time > e.time;
}

// DhtJoinEvent
DhtJoinEvent::DhtJoinEvent(dhtperfproto::timeUnit time/*,
			   std::string knownNodeIp*/)
    : DhtEvent(time)/*, knownNodeIp(knownNodeIp) */
{}

// DhtLeaveEvent
DhtLeaveEvent::DhtLeaveEvent(dhtperfproto::timeUnit time)
    : DhtEvent(time)
{}

// DhtPutEvent
DhtPutEvent::DhtPutEvent(dhtperfproto::timeUnit time,
			 const std::string& key,
			 const std::string& value)
    : DhtEvent(time), key(key), value(value)
{}

// DhtGetEvent
DhtGetEvent::DhtGetEvent(dhtperfproto::timeUnit time, const std::string& key)
    : DhtEvent(time), key(key)
{}

