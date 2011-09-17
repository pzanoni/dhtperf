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

#ifndef DHTPERF_CONTROLLER_DHTCONTROLLER_H
#define DHTPERF_CONTROLLER_DHTCONTROLLER_H

#include <string>

#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "../protocol.h"
#include "ControllerWorkloadProfile.h"
#include "DhtNode.h"

class DhtperfController
{
    public:
	DhtperfController(std::string masterHost, int masterPort);
	~DhtperfController();
	void run();

    private:
	void connect();
	void getDhtType();
	void getProfile();
	void waitForStartMessage();
	void start();
	void reportResults();
	void sendFinishMessage();
	void timeoutThreadFunction();
	void cleanup();

	std::auto_ptr<dhtperfproto::message::Message>
	    getNextMessage(dhtperfproto::opcodeType opcode);

	std::string masterHost_;
	int masterPort_;

	boost::asio::io_service ioService_;
	boost::asio::ip::tcp::socket socket_;

	ControllerWorkloadProfile profile_;
	std::auto_ptr<DhtNode> dhtNode_;

	boost::thread timeoutThread_;
	boost::mutex cleanupMutex_;
};

#endif
