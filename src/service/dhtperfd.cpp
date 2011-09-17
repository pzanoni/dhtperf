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
#include <string>

#include <unistd.h>

#include <boost/asio.hpp>

#include "config.h"
#include "protocol.h"

#ifndef LOG_DIR
#define LOG_DIR "/tmp/"
#endif

namespace ba = boost::asio;

void do_system(const char *cmd)
{
    int rc;
    rc = system(cmd);
    if (rc != 0)
	std::cerr << "Command [" << cmd << "] returned " << rc  << std::endl;
}

void handle_client(ba::ip::tcp::iostream& stream)
{
    std::string msg;

    stream >> msg;

    if (msg == MSG_PING) {
	std::cout << "Received client ping\n";

    } else if (msg == MSG_START) {
	static int controllerNumber = 0;

	std::cout << "Received client start\n";

	std::string host;
	stream >> host;

	std::cout << "Starting with host " << host << "\n";

	std::stringstream cmd;
	cmd << "dhtperf-controller " << host << " > " << LOG_DIR
	    << "controller." << controllerNumber << ".log 2>&1 &";
	do_system(cmd.str().c_str());
	controllerNumber++;

    } else if (msg == MSG_STOP) {
	std::cout << "Received client stop\n";
	do_system("pkill dhtperf-master");
	do_system("pkill dhtperf-controller");
	do_system("pkill java");

    } else if (msg == MSG_UPDATE) {
	std::cout << "Received client update\n";
	do_system("sudo yum update -y &");

    } else if (msg == MSG_VERSION) {
	std::cout << "Reveived client version\n";
	stream << DHTPERF_VERSION << std::endl;

    } else {
	std::cout << "Received unknown message: [" << msg << "]\n";
	return;
    }

    // Send an ack indicating we've done what ne needed to do
    stream << MSG_ACK << std::endl;

}

int do_serve(int port)
{
    ba::io_service io_service;

    ba::ip::tcp::endpoint endpoint(ba::ip::tcp::v4(), port);
    ba::ip::tcp::acceptor acceptor(io_service, endpoint);

    while (true) {
	ba::ip::tcp::iostream stream;

	std::cout << "Waiting for client\n";
	acceptor.accept(*stream.rdbuf());
	std::cout << "Client connected\n";
	handle_client(stream);
    }

    return 0;
}

int main(int argc, char *argv[])
{
    if (argc == 2) {
	if (strcmp(argv[1], "-d") == 0) {
	    if (daemon(0, 0) == -1)
		exit(EXIT_FAILURE);
	}
    }
    return do_serve(DEFAULT_PORT);
}
