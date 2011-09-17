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

#include <cstring>
#include <iostream>

#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "protocol.h"

namespace ba = boost::asio;

void printUsage(const char *progName)
{
    std::cout << "Usage:\n"
	      << "  " << progName << " host command\n"
	      << "Where command can be:\n"
	      << "  ping\n"
	      << "  start master_host\n"
	      << "  stop\n"
	      << "  update\n"
	      << "  version\n";
}

void timeout_thread_function() {
    // Wait up to five seconds only
    ba::io_service ioService;
    ba::deadline_timer timer(ioService, boost::posix_time::seconds(20));
    timer.wait();
    std::cerr << "=== Timeout! ===" << std::endl;
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    if (argc < 3) {
	printUsage(argv[0]);
	return 1;
    }

    boost::thread timeout_thread(&timeout_thread_function);

    std::stringstream portStr;
    portStr << DEFAULT_PORT;

    std::cout << "- Connecting..." << std::endl;
    ba::ip::tcp::iostream stream(argv[1], portStr.str().c_str());
    std::cout << "- Conected!" << std::endl;

    if (strcmp(argv[2], "ping") == 0) {
	std::cout << "- Sending ping\n";
	stream << MSG_PING << std::endl;

	std::cout << "- Ping sent\n";

    } else if (strcmp(argv[2], "start") == 0) {

	if (argc != 4) {
	    printUsage(argv[0]);
	    return 1;
	}

	std::cout << "- Sending start\n";
	stream << MSG_START << " " << argv[3] << std::endl;
	std::cout << "- Done\n";

    } else if (strcmp(argv[2], "stop") == 0) {
	std::cout << "- Sending stop\n";
	stream << MSG_STOP << std::endl;
	std::cout << "- Done\n";

    } else if (strcmp(argv[2], "update") == 0) {
	std::cout << "- Sending update\n";
	stream << MSG_UPDATE << std::endl;
	std::cout << "- Done\n";

    } else if (strcmp(argv[2], "version") == 0) {
	std::cout << "- Sending version\n";
	stream << MSG_VERSION << std::endl;
	std::cout << "- Waiting reply\n";
	std::string version;
	stream >> version;
	std::cout << version << std::endl;

    } else {
	printUsage(argv[0]);
	return 1;
    }

    // Now wait for the reply
    std::string reply;
    stream >> reply;

    if (reply == MSG_ACK) {
        std::cout << "- Received ack\n";
    } else {
        std::cerr << "=== Received wrong message: [" << reply << "]===\n";
	exit(EXIT_FAILURE);
    }

    return 0;
}
