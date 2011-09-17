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

#include "OpenchordWrapper.h"
#include "../Utils.h"

OpenchordWrapper::OpenchordWrapper()
    : DhtNode(),
      Wrapper("openchord-console", "oc >", selfPort_)
{}

bool OpenchordWrapper::doJoin()
{
    std::stringstream joinCommand;
    if (nodeStartsNewDht()) {
	joinCommand << "joinN -port " << selfPort_ << "\n";
    } else {
	joinCommand << "joinN -port " << selfPort_ << " -bootstrap "
		    << entryPointHost_ << ":" << entryPointPort_
		    << "\n";
    }

    try {
	std::vector<std::string> command;
	command.push_back(binaryName_);
	startWrapped(command);

	wrapped_->write(joinCommand.str().c_str());
	//waitPrompt();
	std::string line;
	while (1) {
	    line = wrapped_->readline();
	    if (line.compare(promptString_) == 0) {
		// Success
		return true;
	    } else if (line.compare(0, 32,
		       "Join/Creation of network failed.") == 0) {
		wrapped_->write("exit\n");
		wrapped_->write("y\n");
		break;
	    }
	}
    } catch (RWPopenEOF& e) {
    }
    stopWrapped();
    return false;
}

void OpenchordWrapper::doLeave()
{
    std::cout << "Leaving the dht!\n";
    wrapped_->write("exit\n");
    wrapped_->write("y\n");

    stopWrapped();
}

void OpenchordWrapper::doPut(const DhtPutEvent& event)
{
    std::stringstream command;

    command << "insertN -key " << event.key << " -value " << event.value
	    << "\n";
    wrapped_->write(command.str().c_str());
    waitPrompt();
}

bool OpenchordWrapper::doGet(const DhtGetEvent& event)
{
    std::stringstream command;
    std::string line;
    //std::string value;
    bool ret = false;

    command << "retrieveN -key " << event.key << "\n";
    wrapped_->write(command.str().c_str());

    while (1) {
	line = wrapped_->readline();
	if (line.compare(0, 26, "Values associated with key") == 0) {
	    line = wrapped_->readline();
	    if (line != "") {
		//value = line;
		ret = true;
	    }
	} else if (line.compare(promptString_) == 0) {
	    break;
	}
    }

    return ret;
}

uint16_t OpenchordWrapper::portOffsetToPort(uint16_t offset)
{
    return offset + 5000;
}

