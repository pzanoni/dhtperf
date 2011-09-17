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

#include <cstdlib>
#include <iostream>

#include "MasterWorkloadProfile.h"

MasterWorkloadProfile::MasterWorkloadProfile(const std::string& name)
    : name_(name)
{
    joinProfile_ = "";
    leaveProfile_ = "";
    putProfile_ = "";
    getProfile_ = "";
}

bool MasterWorkloadProfile::isComplete()
{
    if (joinProfile_ == "" || leaveProfile_ == "" ||
	putProfile_ == "" || getProfile_ == "")
	return false;
    else
	return true;
}

bool MasterWorkloadProfile::setProfile(DhtOperation type,
				       const std::string& profile)
{
    switch(type) {
    case Join:
	if (joinProfile_ != "") {
	    std::cerr << "Error: resetting joinProfile\n";
	    exit(1);
	}
	joinProfile_ = profile;
	break;
    case Leave:
	if (leaveProfile_ != "") {
	    std::cerr << "Error: resetting leaveProfile\n";
	    exit(1);
	}
	leaveProfile_ = profile;
	break;
    case Put:
	if (putProfile_ != "") {
	    std::cerr << "Error: resetting putProfile\n";
	    exit(1);
	}
	putProfile_ = profile;
	break;
    case Get:
	if (getProfile_ != "") {
	    std::cerr << "Error: resetting getProfile\n";
	    exit(1);
	}
	getProfile_ = profile;
	break;
    }
    return true;
}

void MasterWorkloadProfile::getProfile(std::string& joinProfile,
				       std::string& leaveProfile,
				       std::string& putProfile,
				       std::string& getProfile)
{
    joinProfile = joinProfile_;
    leaveProfile = leaveProfile_;
    putProfile = putProfile_;
    getProfile = getProfile_;
}
