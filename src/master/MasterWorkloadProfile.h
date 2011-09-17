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

#ifndef DHTPERF_MASTER_WORKLOAD_PROFILE_H
#define DHTPERF_MASTER_WORKLOAD_PROFILE_H

#include <string>

#include "../common.h"

class MasterWorkloadProfile
{
    public:
	MasterWorkloadProfile(const std::string& name);
	bool isComplete();

	bool setProfile(DhtOperation type, const std::string& profile);
	void getProfile(std::string& joinProfile, std::string& leaveProfile,
			std::string& putProfile, std::string& getProfile);

    private:
	std::string name_;

	std::string joinProfile_;
	std::string leaveProfile_;
	std::string putProfile_;
	std::string getProfile_;
};

#endif
