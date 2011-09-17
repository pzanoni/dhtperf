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

#ifndef DHTPERF_CONTROLLER_WORKLOAD_PROFILE_H
#define DHTPERF_CONTROLLER_WORKLOAD_PROFILE_H

#include <string>

#include "../common.h"
#include "../protocol.h"
#include "Profile.h"

class ControllerWorkloadProfile
{
    public:
	ControllerWorkloadProfile();
	// XXX: no one calling isComplete
	bool isComplete();

	bool setProfile(DhtOperation type, const std::string& profile);
	dhtperfproto::timeUnit duration();
	void setDuration(dhtperfproto::timeUnit duration);

	dhtperfproto::timeUnit getNextEventTime(
	    dhtperfproto::timeUnit currentTime,
	    bool connected);

	std::list<DhtJoinEvent> getJoinEvents(dhtperfproto::timeUnit
					       currentTime);
	std::list<DhtLeaveEvent> getLeaveEvents(dhtperfproto::timeUnit
						 currentTime);
	std::list<DhtPutEvent> getPutEvents(dhtperfproto::timeUnit
					     currentTime);
	std::list<DhtGetEvent> getGetEvents(dhtperfproto::timeUnit
					     currentTime);
    private:
	std::auto_ptr<Profile<DhtJoinEvent> >  joinProfile_;
	std::auto_ptr<Profile<DhtLeaveEvent> > leaveProfile_;
	std::auto_ptr<Profile<DhtPutEvent> >   putProfile_;
	std::auto_ptr<Profile<DhtGetEvent> >   getProfile_;

	dhtperfproto::timeUnit duration_;
};

#endif
