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

#include <boost/thread.hpp>

#include "PastConsoleWrapper.h"

PastConsoleWrapper::PastConsoleWrapper()
    : DhtNode(),
      Wrapper("past-console", "pc$", selfPort_)
{}

bool PastConsoleWrapper::doJoin()
{
    std::vector<std::string> command;

    command.push_back(binaryName_);
    std::stringstream selfPortStr;
    selfPortStr << selfPort_;
    command.push_back(selfPortStr.str());
    if (nodeStartsNewDht()) {
	command.push_back("bootstrap");
    } else {
	command.push_back(entryPointHost_);
	std::stringstream entryPointPortStr;
	entryPointPortStr << entryPointPort_;
	command.push_back(entryPointPortStr.str());
    }

    try {
	startWrapped(command);
	return true;
    } catch (RWPopenEOF& e) {
    }
    stopWrapped();
    return false;
}

void PastConsoleWrapper::doLeave()
{
    std::string line;

    std::cout << "Leaving the dht!\n";
    wrapped_->write("exit\n");

    do {
	line = wrapped_->readline();
    } while (line != "Bye!");

    stopWrapped();
}

void PastConsoleWrapper::doPut(const DhtPutEvent& event)
{
    std::stringstream command;

    command << "put " << event.key << " " << event.value << "\n";
    wrapped_->write(command.str().c_str());

    waitPrompt();
}

bool PastConsoleWrapper::doGet(const DhtGetEvent& event)
{
    std::stringstream command;
    std::string line;
    bool ret = false;

    command << "get " << event.key << "\n";
    wrapped_->write(command.str().c_str());

    while (1) {
	line = wrapped_->readline();
	if (line.compare(0, 7, "Value: ") == 0) {
	    ret = true;
	} else if (line.compare(promptString_) == 0) {
	    break;
	}
    }

    return ret;
}

uint16_t PastConsoleWrapper::portOffsetToPort(uint16_t offset)
{
    return offset + 5000;
}
