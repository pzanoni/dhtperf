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
#include <boost/thread.hpp>

#include "OwdhtshellWrapper.h"

OwdhtshellWrapper::OwdhtshellWrapper(const std::string& dhtType)
    : DhtNode(),
      Wrapper("owdhtshell", "Ready.", selfPort_),
      dhtType_(dhtType)
{
    if (dhtType_ != "Chord"  && dhtType_ != "Kademlia" &&
	dhtType_ != "Koorde" && dhtType_ != "LinearWalker" &&
	dhtType_ != "Pastry" && dhtType_ != "Tapestry") {
	std::cerr << "Error: invalid DHT type " << dhtType_ << "\n";
	exit(EXIT_FAILURE);
    }
}

bool OwdhtshellWrapper::doJoin()
{
    std::vector<std::string> command;

    command.push_back(binaryName_);
    command.push_back("--no-upnp");
    command.push_back("-d");
    command.push_back(".");
    command.push_back("-s");
    std::stringstream selfPortStr;
    selfPortStr << selfPort_;
    command.push_back(selfPortStr.str());
    command.push_back("-a");
    command.push_back(dhtType_);
    if (!nodeStartsNewDht()) {
	std::stringstream entryHostStr;
	entryHostStr << entryPointHost_ << ":" << entryPointPort_;
	command.push_back(entryHostStr.str());
    }

    try {
	startWrapped(command);
	return true;
    } catch (RWPopenEOF& e) {
    }
    stopWrapped();
    return false;
}

void OwdhtshellWrapper::doLeave()
{
    std::cout << "Leaving the dht!\n";

    wrapped_->write("stop\n");
    wrapped_->write("quit\n");

    stopWrapped();
}


void OwdhtshellWrapper::doPut(const DhtPutEvent& event)
{
    std::stringstream command;

    command << "put " << event.key << " " << event.value << "\n";
    wrapped_->write(command.str().c_str());
    waitPrompt();
}

bool OwdhtshellWrapper::doGet(const DhtGetEvent& event)
{
    std::stringstream command;
    std::string line;
    //std::string value;
    bool ret = false;

    command << "get " << event.key << "\n";
    wrapped_->write(command.str().c_str());

    while (1) {
	line = wrapped_->readline();
	if (line.compare(0, 7, "value: ") == 0) {
	    //value = std::string(&line[7]);
	    ret = true;
	} else if (line.compare(promptString_) == 0) {
	    break;
	}
    }

    return ret;
}

uint16_t OwdhtshellWrapper::portOffsetToPort(uint16_t offset)
{
    return offset + 5000;
}
