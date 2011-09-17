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

#include <boost/thread/thread.hpp>

#include "KaddemoWrapper.h"
#include "KaddemoIdWrapper.h"
#include "../Utils.h"

KaddemoWrapper::KaddemoWrapper()
    : DhtNode(),
      Wrapper("kaddemo_static", "demo >", selfPort_),
      threadStop_(false)
{}

bool KaddemoWrapper::doJoin()
{
    std::stringstream command;

    command << binaryName_ << " ";

    command << "-p " << selfPort_ << " ";
    if (nodeStartsNewDht()) {
	command << "--externalport " << selfPort_ << " "
		<< "--externalip " << Utils::getSelfIp() << " ";
    } else {
	std::string entryPointIp = Utils::getHostIp(entryPointHost_);
	std::string entryPointId = getEntryPointId();

	command << "--bs_ip " << entryPointIp << " "
		<< "--bs_local_ip " << entryPointIp << " "
		<< "--bs_port " << entryPointPort_ << " "
		<< "--bs_local_port " << entryPointPort_ << " "
		<< "--bs_id " << entryPointId << " ";
    }

    startWrapped(command.str().c_str());

    getSelfId();

    if (nodeStartsNewDht()) {
	std::cout << "Starting id wrapper server:\n";
	startIdWrapperServer();
    }

    return true;
}

void KaddemoWrapper::doLeave()
{
    std::string line;

    std::cout << "Leaving the dht!\n";
    wrapped_->write("exit\n");

    if (nodeStartsNewDht()) {
	threadStop_ = true;
	wrapperIdServerThread_->join();
	delete wrapperIdServerThread_;
    }

    stopWrapped();
}


void KaddemoWrapper::doPut(const DhtPutEvent& event)
{
    std::stringstream command;
    int ttl = 600; // minutes

    command << "storevalue " << event.key << " " << event.value << " " << ttl
	    << "\n";
    wrapped_->write(command.str().c_str());
    waitPrompt();
}

bool KaddemoWrapper::doGet(const DhtGetEvent& event)
{
    std::stringstream command;
    std::string line;
    //std::string value;
    bool ret = false;

    command << "findvalue " << event.key << "\n";
    wrapped_->write(command.str().c_str());

    while (1) {
	line = wrapped_->readline();
	if (line.compare(0, 20, "Values found for key") == 0) {
	    line = wrapped_->readline();
	    if (line.compare(0, 4, "1.  ") == 0) {
		//value = std::string(&line[4]);
		ret = true;
	    }
	} else if (line.compare(promptString_) == 0) {
	    break;
	}
    }

    return ret;
}

uint16_t KaddemoWrapper::portOffsetToPort(uint16_t offset)
{
    // Our wrapper also needs a port to get node ids, so we reserve 2 ports per
    // node
    return (offset * 2) + 5000;
}

std::string KaddemoWrapper::getEntryPointId()
{
    KaddemoIdClient c(entryPointHost_, entryPointPort_ + 1);
    return c.getId();
}

void KaddemoWrapper::getSelfId()
{
    std::string line;

    wrapped_->write("getinfo\n");

    while (1) {
	line = wrapped_->readline();
	// XXX: use regexp here to eliminate the ", " ?
	if (line.compare(0, 11, ", Node_id: ") == 0) {
	    id_ = std::string(&line[11]);
	    break;
	}
    }

    std::cout << "My id: " << id_  << "\n";

    waitPrompt();
}

void KaddemoWrapper::startIdWrapperServer()
{
    wrapperIdServerThread_ = new boost::thread(boost::bind(
	&KaddemoWrapper::wrapperIdServerRun, this));
}

void KaddemoWrapper::wrapperIdServerRun()
{
    KaddemoIdServer server(id_, selfPort_ +1);
    server.run(&threadStop_);
}
