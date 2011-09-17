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

#ifndef DHTPERF_CONTROLLER_PROFILE_H
#define DHTPERF_CONTROLLER_PROFILE_H

#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <string>
#include <boost/algorithm/string.hpp>

#include "../protocol.h"
#include "../events.h"

template <class EventType>
class Profile
{
    public:
	static std::auto_ptr<Profile<EventType> >
	       makeProfile(const std::string& profile);

	virtual ~Profile();

	virtual dhtperfproto::timeUnit
		scheduleNextEvent(dhtperfproto::timeUnit currentTime) = 0;
	std::list<EventType> getEvents(dhtperfproto::timeUnit currentTime);

    protected:
	EventQueue<EventType> eventQueue_;

    private:
	static void splitProfileIntoTypeAndParams(const std::string& profile,
						  std::string& type,
						  std::string& params);
};

#include "profiles/Exact.h"
#include "profiles/FullDictionary.h"
#include "profiles/Interval.h"
#include "profiles/None.h"
#include "profiles/Now.h"
#include "profiles/NTimesRandom.h"
#include "profiles/PoissonRandom.h"
#include "profiles/WeibullRandom.h"

template <class EventType>
std::auto_ptr<Profile<EventType> >
Profile<EventType>::makeProfile(const std::string& profile)
{
    std::string type;
    std::string params;

    splitProfileIntoTypeAndParams(profile, type, params);

    if (type == "exact") {
        return std::auto_ptr<Profile<EventType> >(
	    new Exact<EventType>(params));
    } else if (type == "fulldictionary") {
	return std::auto_ptr<Profile<EventType> >(
	    new FullDictionary<EventType>(params));
    } else if (type == "interval") {
	return std::auto_ptr<Profile<EventType> >(
	    new Interval<EventType>(params));
    } else if (type == "none") {
	return std::auto_ptr<Profile<EventType> >(
	    new None<EventType>(params));
    } else if (type == "now") {
	return std::auto_ptr<Profile<EventType> >(
	    new Now<EventType>(params));
    } else if (type == "ntimesrandom") {
        return std::auto_ptr<Profile<EventType> >(
	    new NTimesRandom<EventType>(params));
    } else if (type == "poissonrandom") {
	return std::auto_ptr<Profile<EventType> >(
	    new PoissonRandom<EventType>(params));
    } else if (type == "weibullrandom") {
	return std::auto_ptr<Profile<EventType> >(
	    new WeibullRandom<EventType>(params));
    } else {
        std::cout << "profile type '" << type << "' doesn't exist\n";
        exit(1);
    }
}

template <class EventType>
Profile<EventType>::~Profile()
{}

template <class EventType>
std::list<EventType>
Profile<EventType>::getEvents(dhtperfproto::timeUnit currentTime)
{
    return eventQueue_.popEvents(currentTime);
}

template <class EventType>
void Profile<EventType>::splitProfileIntoTypeAndParams(
    const std::string& profile,
    std::string& type,
    std::string& params)
{
    // FIXME: this function silently converts '\t' into ' '. If we ever allow
    // some kind of space into our config files we'll need to add some kind of
    // quoting/escaping mechanism and take it into consideration before start
    // using boost::split
    std::deque<std::string> deq;
    boost::split(deq, profile, boost::is_any_of("\t "));

    type = deq.front();
    deq.pop_front();

    params = boost::join(deq, " ");
}

#endif
