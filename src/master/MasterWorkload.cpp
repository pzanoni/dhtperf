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

#include <fstream>
#include <iostream>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>

#include "MasterWorkload.h"

using std::cerr;
using std::cout;
using std::endl;
using std::string;

using boost::lexical_cast;
using boost::regex_match;

unsigned int MasterWorkload::nodes()
{
    return nodes_;
}

dhtperfproto::timeUnit MasterWorkload::duration()
{
    return duration_;
}

void MasterWorkload::getNodeProfile(int nodeNumber, string& joinProfile,
				    string& leaveProfile, string& putProfile,
				    string& getProfile)
{
    boost::shared_ptr<MasterWorkloadProfile> nodeProfile =
	profiles_[nodeProfiles_[nodeNumber]];
    nodeProfile->getProfile(joinProfile, leaveProfile, putProfile, getProfile);
}

int MasterWorkload::getNodeEntryPointHost(int /*nodeNumber*/)
{
    // XXX: All nodes will use node 0 as an entry point to the DHT
    return 0;
}

bool MasterWorkload::loadConfig(const string& workloadFilePath, int nodes)
{
    std::ifstream file;
    string line;

    file.open(workloadFilePath.c_str());

    if (!file.is_open()) {
	cerr << "Can't open workload file '" << workloadFilePath << "'\n";
	return false;
    }

    nodes_ = nodes;
    duration_ = 0;
    profiles_.clear();
    nodeProfiles_.clear();

    while (!file.eof()) {
	getline(file, line);
	while (line.length() && line[line.length()-1] == '\\') {
	    string nextLine;

	    getline(file, nextLine);
	    line = line.substr(0, line.length()-1) + nextLine;
	}

	if (!processConfigLine(line))
	    return false;
    }

    // XXX: now check if everything is set!

    if (nodes_ == 0) {
	cerr << "Workload file error: 0 nodes\n";
	return false;
    }

    if (duration_ == 0) {
	cerr << "Workload file error: 0 duration\n";
	return false;
    }

    if (profiles_.size() == 0) {
	cerr << "Workload file error: no profiles\n";
	return false;
    }

    BOOST_FOREACH(ProfileMap::value_type p_it, profiles_) {
	if (!p_it.second->isComplete()) {
	    cerr << "Workload file error: profile " << p_it.first <<
		    " is incomplete\n";
	    return false;
	}
    }


    for (unsigned int i = 0; i < nodes_; i++) {
	if (nodeProfiles_.find(i) == nodeProfiles_.end()) {
	    cerr << "Workload file error: node " << i << " without profile\n";
	    return false;
	}
	if (profiles_.find(nodeProfiles_[i]) == profiles_.end()) {
	    cerr << "Profile " << nodeProfiles_[i] << " doesn't exist\n";
	    return false;
	}

    }

    file.close();
    return true;
}


// Returns false when config is invalid
bool MasterWorkload::processConfigLine(const string& line)
{
    boost::regex re;
    boost::cmatch matches;

    // XXX: static var in class method is ugly
    static boost::shared_ptr<MasterWorkloadProfile> currentProfile;

    if (line[0] == '#') {
	cout << "||comment\n";
	return true;
    }

    // headers
    re = "^nodes (\\d+)$";
    if (regex_match(line.c_str(), matches, re)) {
	// Nodes can be set via command line, so if the value is already set,
	// do not overwrite it.
	// FIXME: this way, we allow the user to specify the "nodes" line
	// multiple times on his workload file. Anyway, only the first one will
	// be used/valid (if not overwritten by command line)
	if (!nodes_) {
	    nodes_ = lexical_cast<unsigned int>(matches[1]);
	    cout << "||nodes: " << nodes_ << endl;
	}
	return true;
    }

    re = "^duration (\\d+)$";
    if (regex_match(line.c_str(), matches, re)) {
	duration_ = lexical_cast<dhtperfproto::timeUnit>(matches[1]);
	cout << "||duration: " << nodes_ << endl;
	return true;
    }

    // profiles
    re = "^profile (\\w+)$";
    if (regex_match(line.c_str(), matches, re)) {
	string profileName = matches[1];

	profiles_[profileName] = boost::shared_ptr<MasterWorkloadProfile>(new
				 MasterWorkloadProfile(profileName));
	currentProfile = profiles_[profileName];
	cout << "||profile " << profileName << endl;
	return true;
    }

    re = "^(join|leave|put|get) (\\w+.*)$";
    if (regex_match(line.c_str(), matches, re)) {
	string type = matches[1];
	string profile = matches[2];

	if (currentProfile == NULL) {
	    cerr << "Workload file: need to specify profile at line '" <<
		    line << "'\n";
	    return false;
	}

	if (type == "join") {
	    currentProfile->setProfile(Join, profile);
	} else if (type == "leave") {
	    currentProfile->setProfile(Leave, profile);
	} else if (type == "put") {
	    currentProfile->setProfile(Put, profile);
	} else if (type == "get") {
	    currentProfile->setProfile(Get, profile);
	}

	cout << "||join profile: " << profile << endl;
	return true;
    }


    // nodes
    re = "^default-profile (\\w+)$";
    if (regex_match(line.c_str(), matches, re)) {
	string profileName = matches[1];

	for (unsigned int i = 0; i < nodes_; i++)
	    nodeProfiles_[i] = profileName;

	cout << "||node default: " << profileName << endl;
	return true;
    }

    re = "^node (\\w+) (\\w+)$";
    if (regex_match(line.c_str(), matches, re)) {

	unsigned int nodeId;
	nodeId = lexical_cast<unsigned int>(matches[1]);
	string profileName = matches[2];

	nodeProfiles_[nodeId] = profileName;
	cout << "||node " << nodeId << ": " << profileName << endl;
	return true;
    }

    cout << line << endl;
    return true;

    //err << "Workload file: invalid line '" << line << "'\n";
    //return false;
}
