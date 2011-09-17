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
#include <sstream>

#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

#include "../Utils.h"
#include "Wrapper.h"

Wrapper::Wrapper(const char *binaryName, const char *promptString, int selfPort)
    : binaryName_(binaryName),
      promptString_(promptString)
{
    // We're going to kill our all our children later:
    if (setpgid(0,0) == -1) {
        std::cerr << "Error: setpgid() failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::stringstream dirName;
    dirName << binaryName << "." << selfPort;

    Utils::mkdir_p(dirName.str().c_str());

    if (chdir(dirName.str().c_str()) == -1) {
        std::cerr << "Error: can't chdir to " << dirName.str() << std::endl;
        exit(EXIT_FAILURE);
    }
}

void Wrapper::startWrapped(const char *command)
{
    std::cout << "Command: " << command << std::endl;

    wrapped_ = std::auto_ptr<RWPopen>(new RWPopen(command));

    std::cout << "Started!" << std::endl;
    waitPrompt();
}

void Wrapper::startWrapped(const std::vector<std::string>& command)
{
    wrapped_ = std::auto_ptr<RWPopen>(new RWPopen(command));
    std::cout << "Started!" << std::endl;
    waitPrompt();
}

void Wrapper::stopWrapped()
{
    struct sigaction act;
    act.sa_flags = 0;

    wrapped_.reset();

    act.sa_handler = SIG_IGN;
    sigaction(SIGTERM, &act, NULL);

    kill(SIGTERM, 0);

    act.sa_handler = SIG_DFL;
    sigaction(SIGTERM, &act, NULL);
}

void Wrapper::waitPrompt()
{
    std::string line;
    while (1) {
        line = wrapped_->readline();
        if (line.compare(promptString_) == 0) {
            return;
        }
    }
}
