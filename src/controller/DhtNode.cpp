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

#include "DhtNode.h"

#include "dhtnodes/DummyDht.h"
//#include "dhtnodes/GenericRubyWrapper.h" not in use anymore
#include "dhtnodes/KaddemoWrapper.h"
#include "dhtnodes/OpenchordWrapper.h"
#include "dhtnodes/OwdhtshellWrapper.h"
#include "dhtnodes/PastConsoleWrapper.h"

namespace bpt = boost::posix_time;

// Factory
std::auto_ptr<DhtNode> DhtNode::make(std::string type)
{
    if (type == "dummy") {
	return std::auto_ptr<DhtNode>(new DummyDht());
    } else if (type == "kaddemo") {
	return std::auto_ptr<DhtNode>(new KaddemoWrapper());
    } else if (type == "openchord") {
	return std::auto_ptr<DhtNode>(new OpenchordWrapper());
    } else if (type == "owshellchord") {
	return std::auto_ptr<DhtNode>(new OwdhtshellWrapper("Chord"));
    } else if (type == "owshellkad") {
	return std::auto_ptr<DhtNode>(new OwdhtshellWrapper("Kademlia"));
    } else if (type == "owshellpastry") {
	return std::auto_ptr<DhtNode>(new OwdhtshellWrapper("Pastry"));
    } else if (type == "pastconsole") {
	return std::auto_ptr<DhtNode>(new PastConsoleWrapper());
    } else {
	std::cout << "DhtNode \"" << type << "\" doesn't exist" << std::endl;
	exit(1);
    }
}

DhtNode::DhtNode()
    : connected_(false),
      entryPointHost_(""),
      entryPointPort_(0),
      selfPort_(0)
{}

DhtNode::~DhtNode()
{}

void DhtNode::setEntryPoint(std::string host, uint16_t portOffset)
{
    entryPointHost_ = host;
    entryPointPort_ = portOffsetToPort(portOffset);
}

void DhtNode::setSelfPortOffset(uint16_t portOffset)
{
    selfPort_ = portOffsetToPort(portOffset);
}

void DhtNode::processJoinEvent(const DhtJoinEvent& /*event*/)
{
    bool success;

    if (connected_) {
	std::cerr << "Received a join event, but I'm connected!" << std::endl;
	exit(1);
    }

    std::cout << "- Joining the DHT" << std::endl;

    while (true) {
	bpt::ptime t0(bpt::microsec_clock::universal_time());
	success = doJoin();
	bpt::ptime t1(bpt::microsec_clock::universal_time());
	bpt::time_duration td(t1 - t0);
	joinLatency_.push(td);

	if (success) {
	    joinSuccessRate_.hit();
	    break;
	} else {
	    joinSuccessRate_.miss();
	    std::cout << "Failed to connect. Retrying." << std::endl;
	    boost::this_thread::sleep(boost::posix_time::seconds(1));
	}
    }
    connected_ = true;
}

void DhtNode::processLeaveEvent(const DhtLeaveEvent& /*event*/)
{
    if (!connected_) {
	std::cerr << "Received a leave event, but I'm disconnected!"
		  << std::endl;
	exit(1);
    }

    std::cout << "- Leaving the DHT" << std::endl;

    bpt::ptime t0(bpt::microsec_clock::universal_time());
    doLeave();
    bpt::ptime t1(bpt::microsec_clock::universal_time());
    bpt::time_duration td(t1 - t0);
    leaveLatency_.push(td);

    connected_ = false;
}

void DhtNode::processPutEvent(const DhtPutEvent& event)
{
    if (!connected_) {
	std::cerr << "Received a put event, but I'm disconnected!" << std::endl;
	return;
    }

    std::cout << "- Putting (" << event.key << ", " << event.value << ")"
	      << std::endl;

    bpt::ptime t0(bpt::microsec_clock::universal_time());
    doPut(event);
    bpt::ptime t1(bpt::microsec_clock::universal_time());
    bpt::time_duration td(t1 - t0);
    putLatency_.push(td);
}

void DhtNode::processGetEvent(const DhtGetEvent& event)
{
    bool success;

    if (!connected_) {
	std::cerr << "Received a get event, but I'm disconnected!" << std::endl;
	return;
    }

    std::cout << "- Getting (" << event.key << ") ...";

    bpt::ptime t0(bpt::microsec_clock::universal_time());
    success = doGet(event);
    if (success)
	getSuccessRate_.hit();
    else
	getSuccessRate_.miss();
    bpt::ptime t1(bpt::microsec_clock::universal_time());
    bpt::time_duration td(t1 - t0);
    getLatency_.push(td);

    if (success)
	std::cout << " success!" << std::endl;
    else
	std::cout << " fail!" << std::endl;
}

void DhtNode::getResults(std::string& joinLatencyString,
			 std::string& leaveLatencyString,
			 std::string& putLatencyString,
			 std::string& getLatencyString,
			 std::string& joinSuccessRateString,
			 std::string& getSuccessRateString)
{
    joinLatencyString = joinLatency_.toString();
    leaveLatencyString = leaveLatency_.toString();
    putLatencyString = putLatency_.toString();
    getLatencyString = getLatency_.toString();
    joinSuccessRateString = joinSuccessRate_.toString();
    getSuccessRateString = getSuccessRate_.toString();
}

bool DhtNode::nodeStartsNewDht()
{
    return (entryPointHost_ == "");
}

bool DhtNode::connected()
{
    return connected_;
}
