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

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include "../protocol.h"
#include "DhtperfController.h"

namespace ba = boost::asio;
namespace bs = boost::system;
namespace dp = dhtperfproto;
namespace dpm = dp::message;

DhtperfController::DhtperfController(std::string masterHost,
				     int masterPort)
    : masterHost_(masterHost),
      masterPort_(masterPort),
      socket_(ioService_),
      dhtNode_(NULL)
{}

DhtperfController::~DhtperfController()
{
    cleanup();
    timeoutThread_.interrupt();
    timeoutThread_.join();
}

void DhtperfController::run()
{
    connect();
    getDhtType();
    getProfile();
    waitForStartMessage();
    start();
    reportResults();
    sendFinishMessage();
}

void DhtperfController::connect()
{
    std::string masterPortString =
		boost::lexical_cast<std::string>(masterPort_);

    ba::ip::tcp::resolver resolver(ioService_);
    ba::ip::tcp::resolver::query query(masterHost_, masterPortString);
    ba::ip::tcp::endpoint endpoint;
    ba::ip::tcp::resolver::iterator endpointIterator = resolver.resolve(query);
    ba::ip::tcp::resolver::iterator endpointIteratorEnd;
    bs::error_code error;

    while (endpointIterator != endpointIteratorEnd) {
	endpoint = *endpointIterator;
	std::cerr << "Endpoint: " << endpoint << std::endl;
	socket_.connect(endpoint, error);
	if (!error)
	    break;
	else
	    std::cerr << "error connecting to endpoint: " << error.message()
		      << std::endl;

	endpointIterator++;
    }

    if (error) {
	std::cerr << "Error connecting: " << error.message() << "\n";
	exit(1);
    }

    std::cout << "Connected!" << std::endl;
}

void DhtperfController::getDhtType()
{
    std::cout << "Getting DHT type" << std::endl;

    std::auto_ptr<dpm::Message> message = getNextMessage(dp::opcode::DhtType);
    std::string dhtType = ((dpm::DhtType*)message.get())->getType();
    std::cout << "type: " << dhtType << std::endl;
    dhtNode_ = DhtNode::make(dhtType);
}

void DhtperfController::getProfile()
{
    std::cout << "Getting profile." << std::endl;

    std::auto_ptr<dpm::Message> message = getNextMessage(dp::opcode::Profile);

    std::string joinProfile, leaveProfile, putProfile, getProfile;
    ((dpm::Profile*)message.get())->getProfiles(joinProfile, leaveProfile,
						putProfile, getProfile);

    profile_.setProfile(Join, joinProfile);
    profile_.setProfile(Leave, leaveProfile);
    profile_.setProfile(Put, putProfile);
    profile_.setProfile(Get, getProfile);
}

void DhtperfController::waitForStartMessage()
{
    std::cout << "Waiting for start message..." << std::endl;

    std::auto_ptr<dpm::Message> message = getNextMessage(dp::opcode::Start);
    dpm::Start* startMessage = (dpm::Start*)message.get();
    profile_.setDuration(startMessage->duration());
    dhtNode_->setEntryPoint(startMessage->entryPointHost(),
			    startMessage->entryPointPortOffset());
    dhtNode_->setSelfPortOffset(startMessage->selfPortOffset());
}

// XXX Move this function to dp:: ?
// XXX Pass opcode as argument and do some checking?
std::auto_ptr<dpm::Message>
DhtperfController::getNextMessage(dp::opcodeType opcode)
{
    bs::error_code error;
    dp::MessageHeader header;
    ba::read(socket_, ba::buffer(&header, dp::headerSize),
	     ba::transfer_all(), error);
    if (error) {
	std::cerr << "Error: " << error.message() << std::endl;
	exit(1);
    }

    if (header.opcode != opcode) {
	std::cerr << "Error: unexpected message opcode: received "
		  << header.opcode << ", expected " << opcode << std::endl;
	exit(1);
    }

    std::vector<char> body(header.bodySize, 0);

    if (header.bodySize > 0)
	ba::read(socket_, ba::buffer(body, body.size()), ba::transfer_all(),
		 error);

    return dpm::Message::fromBits(header, body);
}

// This function became quite a mess over the time... FIXME!
// Pitfalls:
// - Comparision of signed/unsigned (dp::timeUnit is unsigned!)
// - Profiles can return events whose timestamps are bigger than "duration"
// - Don't return from this function before "duration" has passed
// - DhtNode operations might never end. That's why we have the timeout thread.
void DhtperfController::start()
{
    dp::timeUnit duration = profile_.duration();
    dp::timeUnit elapsed, nextEvent, toWait;
    ba::deadline_timer timer(ioService_);
    ba::deadline_timer aux_timer(ioService_);
    boost::posix_time::time_duration remaining;

    // The master is only going to wait for duration + ReportAndFinishTimeout
    // for us, so we can just kill the process after that time...
    timeoutThread_ = boost::thread(boost::bind(
			&DhtperfController::timeoutThreadFunction, this));

    std::cout << "Starting! Duration: " << duration << std::endl;

    timer.expires_from_now(boost::posix_time::seconds(duration));

// Use this macro carefully!
#define DHTPERF_CHECK_DONE					 \
    remaining = timer.expires_from_now();                        \
    elapsed = duration - remaining.total_seconds();              \
    if (remaining.total_seconds() < 0 || elapsed >= duration) {  \
	std::cout << "Time's up! Finishing..." << std::endl;     \
	return;                                                  \
    }                                                            \

    while (1) {
	DHTPERF_CHECK_DONE

	nextEvent = profile_.getNextEventTime(elapsed, dhtNode_->connected());

	// Profile might give us an event bigger than the duration
	if (nextEvent > duration)
	    nextEvent = 0;

	if (elapsed >= nextEvent && nextEvent != 0) {
	    std::cout << "Processing event at time " << elapsed << std::endl;

	    if (dhtNode_->connected()) {
		std::list<DhtPutEvent> putEvents =
		    profile_.getPutEvents(elapsed);
		std::list<DhtGetEvent> getEvents =
		    profile_.getGetEvents(elapsed);
		std::list<DhtLeaveEvent> leaveEvents =
		    profile_.getLeaveEvents(elapsed);

		BOOST_FOREACH(DhtPutEvent event, putEvents) {
		    dhtNode_->processPutEvent(event);
		    DHTPERF_CHECK_DONE
		}
		BOOST_FOREACH(DhtGetEvent event, getEvents) {
		    dhtNode_->processGetEvent(event);
		    DHTPERF_CHECK_DONE
		}
		BOOST_FOREACH(DhtLeaveEvent event, leaveEvents) {
		    dhtNode_->processLeaveEvent(event);
		    DHTPERF_CHECK_DONE
		}
	    } else {
		std::list<DhtJoinEvent> joinEvents =
		    profile_.getJoinEvents(elapsed);
		BOOST_FOREACH(DhtJoinEvent event, joinEvents) {
		    dhtNode_->processJoinEvent(event);
		    DHTPERF_CHECK_DONE
		}
	    }
	}
	if (nextEvent != 0) {
	    if (nextEvent > elapsed) {
		toWait = nextEvent - elapsed;
	    } else {
		continue;
	    }
	} else {
	    std::cout << "No more events! Sleeping until the end..."
		      << std::endl;
	    toWait = remaining.total_seconds();
	}

	std::cout << "Waiting " << toWait << " time units" << std::endl;
	aux_timer.expires_from_now(boost::posix_time::seconds(toWait));
	aux_timer.wait();
    }
#undef DHTPERF_CHECK_DONE
}

void DhtperfController::timeoutThreadFunction()
{
    // Wait 10 seconds more than the Master, so that it will detect us as
    // "Unfinished". If we end up closing the conection before that, it might
    // detect us as "Failed", which will be wrong.
    int toWait = profile_.duration() + dp::ReportAndFinishTimeout + 10;

    boost::this_thread::sleep(boost::posix_time::seconds(toWait));
    std::cerr << "=== Timeout! ===" << std::endl;

    cleanup();
    exit(EXIT_FAILURE);
}

void DhtperfController::reportResults()
{
    bs::error_code error;
    std::string joinLatencyString, leaveLatencyString, putLatencyString,
		getLatencyString, joinSuccessRateString, getSuccessRateString;
    std::vector<char> bits;

    std::cout << "Reporting results" << std::endl;

    dhtNode_->getResults(joinLatencyString, leaveLatencyString,
			 putLatencyString, getLatencyString,
			 joinSuccessRateString, getSuccessRateString);

    // JoinLatency
    dpm::Report joinLatencyReport("JoinLatency", joinLatencyString);
    bits = joinLatencyReport.toBits();
    ba::write(socket_, ba::buffer(bits, bits.size()), ba::transfer_all(),
	      error);
    if (error) {
	std::cerr << "Error at reportResults: " << error.message() << std::endl;
	exit(1);
    }

    // LeaveLatency
    dpm::Report leaveLatencyReport("LeaveLatency", leaveLatencyString);
    bits = leaveLatencyReport.toBits();
    ba::write(socket_, ba::buffer(bits, bits.size()), ba::transfer_all(),
	      error);
    if (error) {
	std::cerr << "Error at reportResults: " << error.message() << std::endl;
	exit(1);
    }

    // GetLatency
    dpm::Report getLatencyReport("GetLatency", getLatencyString);
    bits = getLatencyReport.toBits();
    ba::write(socket_, ba::buffer(bits, bits.size()), ba::transfer_all(),
	      error);
    if (error) {
	std::cerr << "Error at reportResults: " << error.message() << std::endl;
	exit(1);
    }

    // PutLatency
    dpm::Report putLatencyReport("PutLatency", putLatencyString);
    bits = putLatencyReport.toBits();
    ba::write(socket_, ba::buffer(bits, bits.size()), ba::transfer_all(),
	      error);
    if (error) {
	std::cerr << "Error at reportResults: " << error.message() << std::endl;
	exit(1);
    }

    // JoinSuccessRate
    dpm::Report joinSuccessRateReport("JoinSuccessRate", joinSuccessRateString);
    bits = joinSuccessRateReport.toBits();
    ba::write(socket_, ba::buffer(bits, bits.size()), ba::transfer_all(),
	      error);
    if (error) {
	std::cerr << "Error at reportResults: " << error.message() << std::endl;
	exit(1);
    }

    // GetSuccessRate
    dpm::Report getSuccessRateReport("GetSuccessRate", getSuccessRateString);
    bits = getSuccessRateReport.toBits();
    ba::write(socket_, ba::buffer(bits, bits.size()), ba::transfer_all(),
	      error);
    if (error) {
	std::cerr << "Error at reportResults: " << error.message() << std::endl;
	exit(1);
    }
}

void DhtperfController::sendFinishMessage()
{
    bs::error_code error;

    std::cout << "Sending finish message." << std::endl;

    dpm::Finish finishMessage;
    std::vector<char> bits = finishMessage.toBits();
    ba::write(socket_, ba::buffer(bits, bits.size()), ba::transfer_all(),
	      error);
    if (error) {
	std::cerr << "Error at finish: " << error.message() << std::endl;
	exit(1);
    }
}

void DhtperfController::cleanup()
{
    // This can be called either from the "timeout" thread or the main thread
    boost::mutex::scoped_lock lock(cleanupMutex_);
    dhtNode_.reset();
}
