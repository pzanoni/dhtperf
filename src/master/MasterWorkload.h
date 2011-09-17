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

#ifndef DHTPERF_MASTER_WORKLOAD_H
#define DHTPERF_MASTER_WORKLOAD_H

#include <map>
#include <string>

#include <boost/shared_ptr.hpp>

#include "../protocol.h"

#include "MasterWorkloadProfile.h"

class MasterWorkload
{
    public:
	bool loadConfig(const std::string& workloadFilePath, int nodes);

	unsigned int nodes();
	dhtperfproto::timeUnit duration();

	void getNodeProfile(int nodeNumber, std::string& joinProfile,
			    std::string& leaveProfile, std::string& putProfile,
			    std::string& getProfile);
	int getNodeEntryPointHost(int nodeNumber);

    private:
	unsigned int nodes_;
	dhtperfproto::timeUnit duration_;

	typedef std::map<std::string, boost::shared_ptr<MasterWorkloadProfile>
		> ProfileMap;
	typedef std::map<unsigned int, std::string> NodeProfileMap;

	ProfileMap profiles_;
	NodeProfileMap nodeProfiles_;

	bool processConfigLine(const std::string& line);
};

#endif
