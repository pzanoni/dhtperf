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

#ifndef DHTPERF_MASTER_H
#define DHTPERF_MASTER_H

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>

#include "MasterWorkload.h"
#include "ControllerConnection.h"
#include "ResultProcessor.h"

class DhtperfMaster {

    public:
	DhtperfMaster(int port, const std::string& dhtType, bool lowLatency);
	void loadWorkloadFile(const std::string& workloadFilePath,
			      int nodes = 0);
	void run();
	void handleMessage(int controllerNumber,
	     std::auto_ptr<dhtperfproto::message::Message> message);
	void handleDisconnection(int controllerNumber);

    private:

	void asyncAccept();
	void handleAccept(boost::shared_ptr<ControllerConnection>
			    controllerConnection,
			  const boost::system::error_code& error);

	void sendDhtTypeMessage(int controllerNumber);
	void sendProfileMessage(int controllerNumber);
	void sendStartMessages();
	void handleReportMessage(int controllerNumber,
				 dhtperfproto::message::Report *report);
	void handleFinishMessage(int controllerNumber);
	void closeConnections();

	void pushNodesStatus();
	void processResults();

	void handleWorkloadTimeout(const boost::system::error_code& error);
	void controllerFinished();

	MasterWorkload workload_;
	int port_;
	std::string dhtType_;

	boost::asio::io_service ioService_;
        boost::asio::ip::tcp::acceptor acceptor_;

	bool startMessagesSent_;

	std::vector<boost::shared_ptr<ControllerConnection> > controllers_;
	// "finished" includes the failed ones too
	unsigned int finishedControllers_;
	unsigned int failedControllers_;

	ResultProcessor resultProcessor_;
};

#endif
