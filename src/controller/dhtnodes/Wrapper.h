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

#ifndef DHTPERF_CONTROLLER_DHTNODES_WRAPPER_H
#define DHTPERF_CONTROLLER_DHTNODES_WRAPPER_H

#include <cstdlib>
#include <memory>
#include <string>
#include <vector>

#include "RWPopen.h"

class Wrapper
{
    public:
	Wrapper(const char *binaryName, const char *promptString, int selfPort);

	void startWrapped(const char *command);
	void startWrapped(const std::vector<std::string>& command);
	void stopWrapped();

	void waitPrompt();

    protected:
	const char *binaryName_;
	const char *promptString_;
	std::auto_ptr<RWPopen> wrapped_;
};

#endif
