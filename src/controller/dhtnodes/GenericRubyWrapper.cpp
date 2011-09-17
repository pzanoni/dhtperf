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

#include "GenericRubyWrapper.h"

GenericRubyWrapper::GenericRubyWrapper(const std::string& dhtType)
    : DhtNode(),
      dhtType_(dhtType)
{}

GenericRubyWrapper::~GenericRubyWrapper()
{
    std::cout << "~GenericRubyWrapper()\n";
}

bool GenericRubyWrapper::doJoin()
{
    std::stringstream command;

    command << dhtType_ << "-wrapper --sp " << selfPort_;
    if (!nodeStartsNewDht()) {
	command << " --eph " << entryPointHost_ << " --epp " << entryPointPort_;
    }

    std::cout << "Command: " << command.str() << "\n";

    wrapperPipe_ = std::auto_ptr<RWPopen>(new RWPopen(command.str().c_str()));
    waitPrompt();
    return true;
}

void GenericRubyWrapper::doLeave()
{
    std::string line;

    std::cout << "Leaving the dht!\n";
    wrapperPipe_->write("stop\n");
    while (1) {
	line = getNonCommentLine();
	if (line == "Bye =)")
	    break;
    }
}

void GenericRubyWrapper::doPut(const DhtPutEvent& event)
{
    std::stringstream command;

    command << "put " << event.key << " " << event.value << "\n";
    wrapperPipe_->write(command.str().c_str());
    waitPrompt();
}

bool GenericRubyWrapper::doGet(const DhtGetEvent& event)
{
    std::stringstream command;
    std::string line;
    bool ret;

    command << "get " << event.key << "\n";
    wrapperPipe_->write(command.str().c_str());

    while (1) {
	line = getNonCommentLine();
	if (line == "Value not found") {
	    ret = false;
	    break;
	} else if (line == "Value found:") {
	    // get the value but do nothing with it:
	    line = wrapperPipe_->readline();
	    ret = true;
	    break;
	}
    }

    waitPrompt();
    return ret;
}

uint16_t GenericRubyWrapper::portOffsetToPort(uint16_t offset)
{
    // Our wrapper also needs a port to get node ids, so we reserve 2 ports per
    // node
    return (offset * 2) + 5000;
}

void GenericRubyWrapper::waitPrompt()
{
    std::string line;
    while (1) {
	line = wrapperPipe_->readline();
	if (line == "w:$") {
	    return;
	}
    }
}

std::string GenericRubyWrapper::getNonCommentLine()
{
    std::string line;
    while (1) {
	line = wrapperPipe_->readline();
	if (line[0] != '#') {
	    return line;
	}
    }
}
