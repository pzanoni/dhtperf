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

#ifndef DHTPERF_CONTROLLER_DHTNODES_RWPOPEN_H
#define DHTPERF_CONTROLLER_DHTNODES_RWPOPEN_H

#include <string>
#include <stdexcept>
#include <vector>

#include <unistd.h>

// This is a hack. We really should make the "readline" function return an empty
// string and then make the caller check for the string size.
class RWPopenEOF : public virtual std::runtime_error
{
    public:
	RWPopenEOF();
};

class RWPopen
{
    public:
	RWPopen(const char *command);
	RWPopen(const std::vector<std::string>& command);
	~RWPopen();
	void write(const char *s);
	std::string readline();
    private:
	bool initAndFork();

	int in_;
	int out_;
	pid_t childPid_;
};

#endif
