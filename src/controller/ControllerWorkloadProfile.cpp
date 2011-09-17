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

#include "ControllerWorkloadProfile.h"

namespace dp = dhtperfproto;

ControllerWorkloadProfile::ControllerWorkloadProfile()
    : joinProfile_(NULL),
      leaveProfile_(NULL),
      putProfile_(NULL),
      getProfile_(NULL),
      duration_(0)
{}

// XXX: no one calls this
bool ControllerWorkloadProfile::isComplete()
{
    if (joinProfile_.get() == NULL)
	return false;
    if (leaveProfile_.get() == NULL)
	return false;
    if (putProfile_.get() == NULL)
	return false;
    if (getProfile_.get() == NULL)
	return false;
    return true;
}

bool ControllerWorkloadProfile::setProfile(DhtOperation type,
					   const std::string& profile)
{
    switch (type) {
    case Join:
	joinProfile_ = Profile<DhtJoinEvent>::makeProfile(profile);
	if (joinProfile_.get() == NULL)
	    return false;
	else
	    return true;
	break;
    case Leave:
	leaveProfile_ = Profile<DhtLeaveEvent>::makeProfile(profile);
	if (leaveProfile_.get() == NULL)
	    return false;
	else
	    return true;
	break;
    case Put:
	putProfile_ = Profile<DhtPutEvent>::makeProfile(profile);
	if (putProfile_.get() == NULL)
	    return false;
	else
	    return true;
	break;
    case Get:
	getProfile_ = Profile<DhtGetEvent>::makeProfile(profile);
	if (getProfile_.get() == NULL)
	    return false;
	else
	    return true;
	break;
    }
    exit(1);
}

dp::timeUnit ControllerWorkloadProfile::duration()
{
    return duration_;
}

void ControllerWorkloadProfile::setDuration(dp::timeUnit duration)
{
    duration_ = duration;
}

dp::timeUnit ControllerWorkloadProfile::getNextEventTime(
    dp::timeUnit currentTime,
    bool connected)
{
    dp::timeUnit ret;
    if (connected) {
	dp::timeUnit aux;
	ret = leaveProfile_->scheduleNextEvent(currentTime);
	aux = putProfile_->scheduleNextEvent(currentTime);
	if ((ret == 0) || (aux < ret && aux != 0))
	    ret = aux;
	aux = getProfile_->scheduleNextEvent(currentTime);
	if ((ret == 0) || (aux < ret && aux != 0))
	    ret = aux;
    } else {
	ret = joinProfile_->scheduleNextEvent(currentTime);
    }

    std::cout << "Next event time (" << currentTime << "): " << ret
	      << std::endl;
    return ret;
}

std::list<DhtJoinEvent> ControllerWorkloadProfile::getJoinEvents(
						    dp::timeUnit currentTime)
{
    return joinProfile_->getEvents(currentTime);
}

std::list<DhtLeaveEvent> ControllerWorkloadProfile::getLeaveEvents(
						    dp::timeUnit currentTime)
{
    return leaveProfile_->getEvents(currentTime);
}

std::list<DhtPutEvent> ControllerWorkloadProfile::getPutEvents(
						    dp::timeUnit currentTime)
{
    return putProfile_->getEvents(currentTime);
}

std::list<DhtGetEvent> ControllerWorkloadProfile::getGetEvents(
						    dp::timeUnit currentTime)
{
    return getProfile_->getEvents(currentTime);
}
