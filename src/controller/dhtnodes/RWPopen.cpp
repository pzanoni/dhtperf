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
#include <cstdio>
#include <cstring>
#include <iostream>

extern "C" {
#include <alloca.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
}

#include <boost/foreach.hpp>

#include "RWPopen.h"

RWPopenEOF::RWPopenEOF()
    : runtime_error("Error: EOF reading RWPopen object")
{}

// Runs command inside a shell
// Pro: you can provide a single command line with all arguments together
// Con: since it runs a shell, it may be harder to kill the children
RWPopen::RWPopen(const char *command)
{
    std::cout << "RWPOPEN: " << command << std::endl;
    if (initAndFork()) {
	execlp("/bin/sh", "/bin/sh", "-c", command, (char *) NULL);
	exit(EXIT_FAILURE);
    }
}

// Runs the provided command
// Pro: you exec the command, so there's no intermediate shell
// Con: you need to split the arguments
RWPopen::RWPopen(const std::vector<std::string>& command)
{
    if (!initAndFork())
	return;

    std::cout << "RWPOPEN:";
    BOOST_FOREACH(std::string s, command) {
	std::cout << " " << s;
    }
    std::cout << std::endl;

    char *exec_file = (char *)alloca(sizeof(char) * command[0].size());
    strcpy(exec_file, command[0].c_str());

    char **exec_argv = (char **)alloca(sizeof(char*) * (command.size()+1));

    unsigned int i;
    for(i = 0; i < command.size(); i++) {
	exec_argv[i] = (char *)alloca(sizeof(char) * command[i].size());
	strcpy(exec_argv[i], command[i].c_str());
    }
    exec_argv[i] = NULL;

    execvp(exec_file, exec_argv);
    exit(EXIT_FAILURE);
}

// Returns "true" if we're the child
bool RWPopen::initAndFork()
{
    int pair0[2];
    int pair1[2];
    pid_t pid;

    if (pipe(pair0) == -1) {
	std::cerr << "Error: failed to open pipe" << std::endl;
	exit(EXIT_FAILURE);
    }
    if (pipe(pair1) == -1) {
	std::cerr << "Error: failed to open pipe" << std::endl;
	exit(EXIT_FAILURE);
    }

    // 0 == read
    // 1 == write
    pid = fork();
    if (pid == -1) {
	std::cerr << "Error: failed to fork" << std::endl;
	exit(EXIT_FAILURE);

    } else if (pid == 0) {
	// XXX: check for errors on close/dup
	// Child
	close(pair0[0]);
	close(pair1[1]);
	dup2(pair0[1], STDOUT_FILENO);
	dup2(pair1[0], STDIN_FILENO);
	close(pair0[1]);
	close(pair1[0]);

	// Finally, move stderr to stdout
	dup2(pair0[1], STDERR_FILENO);

	return true;
	//execlp("/bin/sh", "/bin/sh", "-c", command, (char *) NULL);
	//exit(EXIT_FAILURE);

    } else {
	close(pair0[1]);
	close(pair1[0]);
	in_ = pair0[0];
	out_ = pair1[1];
	childPid_ = pid;
	return false;
    }
}

RWPopen::~RWPopen()
{
    if (close(in_) == -1 || close(out_) == -1) {
	std::cerr << "Error: failed to close RWPopen" << std::endl;
	exit(EXIT_FAILURE);
    }

    if (kill(childPid_, SIGTERM) == -1) {
	std::cerr << "Error: failed to kill RWPopen child" << std::endl;
	exit(EXIT_FAILURE);
    }

    int status;
    std::cout << "Waiting for child to finish..." << std::endl;
    while (wait(&status) != childPid_);
    std::cout << "Child finished!" << std::endl;
}

void RWPopen::write(const char *s)
{
    size_t written = 0;
    ssize_t ret;
    size_t s_len;

    s_len = strlen(s);

    std::cout << "RWPOPEN: [" << s << "]" << std::endl;

    while (written != s_len) {
	ret = ::write(out_, &s[written], s_len - written);
	if (ret == -1) {
	    std::cerr << "Error: failed to write to RWPopen object"
		      << std::endl;
	    exit(EXIT_FAILURE);
	}
	written += ret;
    }
    fsync(out_);
}

std::string RWPopen::readline()
{
    std::string ret;
    ssize_t readed;
    char buf;

    while (1) {
	readed = read(in_, &buf, 1);
	if (readed == 0) {
	    throw RWPopenEOF();
	} else if (readed == -1) {
	    std::cerr << "Error: failed to read from RWPopen object"
		      << std::endl;
	    exit(EXIT_FAILURE);
	}

	if (buf == '\n') {
	    if (ret[ret.size()-1] == '\r')
		ret.resize(ret.size()-1);

	    std::cout << "RWPOPEN: [" << ret << "]" << std::endl;
	    return ret;
	} else {
	    ret += buf;
	}
    }
}

