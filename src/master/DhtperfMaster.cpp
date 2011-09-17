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

#include <boost/bind.hpp>

#include "DhtperfMaster.h"

namespace ba = boost::asio;
namespace bs = boost::system;
namespace dp = dhtperfproto;
namespace dpm = dp::message;

DhtperfMaster::DhtperfMaster(int port, const std::string& dhtType,
			     bool lowLatency)
    : port_(port),
      dhtType_(dhtType),
      acceptor_(ioService_, ba::ip::tcp::endpoint(ba::ip::tcp::v4(), port_)),
      startMessagesSent_(false),
      finishedControllers_(0),
      failedControllers_(0),
      resultProcessor_(dhtType, lowLatency)
{}

void DhtperfMaster::loadWorkloadFile(const std::string& workloadFilePath,
				     int nodes)
{
    if (!workload_.loadConfig(workloadFilePath, nodes)) {
        std::cerr << "Invalid workload config file!" << std::endl;
        exit(1);
    }
}

void DhtperfMaster::run()
{
    asyncAccept();
    // asyncAccept() will accept every client we need to accept and then send
    // the start messages after every node connects

    while (!startMessagesSent_)
	ioService_.run_one();

    std::string now = boost::posix_time::to_simple_string(
	boost::posix_time::second_clock::local_time());
    std::cout << "Starting: " << now << std::endl;

    // We will wait for the duration of the workload + some seconds for every
    // node to report their results. After that, we'll consider these nodes
    // "dead" and go on without them. Sorry, life happens, we must move on.
    std::cout << "Timeout: "
	      << workload_.duration() + dp::ReportAndFinishTimeout
	      << std::endl;
    ba::deadline_timer timer(ioService_,
		       boost::posix_time::seconds(workload_.duration() +
					dp::ReportAndFinishTimeout));
    timer.async_wait(boost::bind(&DhtperfMaster::handleWorkloadTimeout, this,
				 ba::placeholders::error));

    // This call will block until all messages we're waiting for are received,
    // i.e. until all nodes are finished or we timeout
    ioService_.run();

    closeConnections();
    pushNodesStatus();
    processResults();
}

void DhtperfMaster::asyncAccept()
{
    boost::shared_ptr<ControllerConnection> controller(new
	ControllerConnection(ioService_));
    acceptor_.async_accept(controller->socket(),
			   boost::bind(&DhtperfMaster::handleAccept, this,
				      controller, ba::placeholders::error));
}

void DhtperfMaster::handleAccept(boost::shared_ptr<ControllerConnection>
				    controller,
				 const bs::error_code& error)
{
    if (error) {
        std::cout << "Accept error: " << error.message() << std::endl;
	exit(1);
    }

    int controllerNumber = controllers_.size();
    std::cout << "Client accepted. Number: " << controllerNumber
	      << ". Endpoint: " << controller->socket().remote_endpoint()
	      << std::endl;
    controllers_.push_back(controller);
    controller->setHandlers(boost::bind(&DhtperfMaster::handleMessage,
					this, controllerNumber, _1),
			    boost::bind(&DhtperfMaster::handleDisconnection,
					this, controllerNumber));
    sendDhtTypeMessage(controllerNumber);
    sendProfileMessage(controllerNumber);

    if (controllers_.size() < workload_.nodes()) {
	asyncAccept();
    } else {
	std::cout << "All nodes connected! Sending start messages..."
		  << std::endl;
	acceptor_.close(); // XXX: not sure this is the right way...
	sendStartMessages();
    }
}

void DhtperfMaster::sendDhtTypeMessage(int controllerNumber)
{
    std::cout << "Sending DHT type" << std::endl;

    dpm::DhtType message(dhtType_);
    controllers_[controllerNumber]->sendMessage((dpm::Message*)&message);
}

void DhtperfMaster::sendProfileMessage(int controllerNumber)
{
    std::cout << "Sending profile" << std::endl;

    std::string joinProfile, leaveProfile, putProfile, getProfile;
    workload_.getNodeProfile(controllerNumber, joinProfile, leaveProfile,
			     putProfile, getProfile);

    dpm::Profile message(joinProfile, leaveProfile, putProfile, getProfile);
    controllers_[controllerNumber]->sendMessage((dpm::Message*)&message);
}

void DhtperfMaster::sendStartMessages()
{
    std::map<std::string, int> nodesPerHost;

    std::cout << "Sending start messages" << std::endl;

    for(unsigned int i = 0; i < controllers_.size(); ++i) {
	int entryPointNodeNumber;
	std::string entryPointHost;
	uint16_t entryPointPortOffset;
	std::string nodeHost;
	uint16_t nodePortOffset;

	entryPointNodeNumber = workload_.getNodeEntryPointHost(i);
	if (i == 0)
	    entryPointHost = "";
	else
	    entryPointHost = controllers_[entryPointNodeNumber]->socket().
			     remote_endpoint().address().to_string();

	// XXX: let's hardcode the port offset to 0 too
	entryPointPortOffset = 0;

	nodeHost = controllers_[i]->socket().remote_endpoint().address().
		   to_string();
	if (nodesPerHost.count(nodeHost))
	    nodesPerHost[nodeHost]++;
	else
	    nodesPerHost[nodeHost] = 1;

	nodePortOffset = nodesPerHost[nodeHost] -1;

	std::cout << "Node " << i << " will connect to " << entryPointHost
		  << ":+" << entryPointPortOffset << std::endl;

	dpm::Start message(workload_.duration(), entryPointHost,
			   entryPointPortOffset, nodePortOffset);
	controllers_[i]->sendMessage((dpm::Message*)&message);

	// Now wait Report messages:
	controllers_[i]->asyncWaitForMessage();
    }

    std::cout << "Start messsages sent" << std::endl;

    startMessagesSent_ = true;
}

void DhtperfMaster::handleMessage(int controllerNumber,
				  std::auto_ptr<dpm::Message> message)
{
    std::cout << "handling message from controller " << controllerNumber <<
		 ", opcode " << message->opcode() << std::endl;

    switch(message->opcode()) {
    case dp::opcode::Report:
	handleReportMessage(controllerNumber, (dpm::Report *)(message.get()));
	break;
    case dp::opcode::Finish:
	handleFinishMessage(controllerNumber);
	break;
    default:
	std::cerr << "Invalid opcode: " << message->opcode() << std::endl;
	exit(1);
    }
}

void DhtperfMaster::handleDisconnection(int controllerNumber)
{
    std::cout << "Controller " << controllerNumber << " disconnected"
	      << std::endl;

    failedControllers_++;
    controllerFinished();
    // Just don't wait for more messages and it's "okay".
}

void DhtperfMaster::handleReportMessage(int controllerNumber,
					dpm::Report *report)
{
    std::cout << "Storing report: controller=" << controllerNumber << " type="
	      << report->type() << std::endl;
    resultProcessor_.pushResult(report->type(), report->value());
    // Wait for other reports or a finish message:
    controllers_[controllerNumber]->asyncWaitForMessage();
}

void DhtperfMaster::handleFinishMessage(int controllerNumber)
{
    std::cout << "Node " << controllerNumber << " finished" << std::endl;
    controllerFinished();
    // Since we're not waiting for other messages, we're done.
}

void DhtperfMaster::handleWorkloadTimeout(const bs::error_code& error)
{
    if (error) {
	std::cerr << "Error at handleWorkloadTimeout:" << error.message()
		  << std::endl;
	exit(EXIT_FAILURE);
    }
    std::cout << "Timeout! Forcing finishing" << std::endl;
    ioService_.stop();
}

void DhtperfMaster::controllerFinished()
{
    finishedControllers_++;
    if (finishedControllers_ == workload_.nodes()) {
	std::cout << "All controllers finished!" << std::endl;
	// Even after all nodes are finished, we'll still have the
	// workloadTimeout waiting on the ioService_, so we need to stop it
	// manually
	ioService_.stop();
    }
}

void DhtperfMaster::closeConnections()
{
    // XXX FIXME!
    // Not sure something needs to be done here...
    //ioService_.reset();

    for(unsigned int i = 0; i < controllers_.size(); i++) {
	//std::cout << "Count: " << controllers_[i].use_count() << "\n";
	assert(controllers_[i].unique());
    }

    controllers_.clear();
}

void DhtperfMaster::pushNodesStatus()
{
    // Nodes that closed their connection:
    int failed = failedControllers_;
    // Nodes that closed successfully, reporting results:
    int finished = finishedControllers_ - failedControllers_;
    // Nodes that didn't report any results nor closed their connections:
    int unfinished = workload_.nodes() - finishedControllers_;

    std::stringstream value;
    value << finished << " " << failed << " " << unfinished;

    resultProcessor_.pushResult("NodesStatus", value.str());
}

void DhtperfMaster::processResults()
{
    resultProcessor_.process();
}
