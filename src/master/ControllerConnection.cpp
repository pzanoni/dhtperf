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

#include "ControllerConnection.h"

namespace ba = boost::asio;
namespace bs = boost::system;
namespace dp = dhtperfproto;
namespace dpm = dp::message;

ControllerConnection::ControllerConnection(ba::io_service& ioService)
    : socket_(ioService),
      messageHandler_(NULL),
      disconnectionHandler_(NULL) // XXX: maybe we should properly init it here?
{}

ControllerConnection::~ControllerConnection()
{
    std::cout << "~ControllerConnection()\n";
}

void ControllerConnection::setHandlers(
    messageCallbackFunction messageHandler,
    disconnectionCallbackFunction disconnectionHandler)
{
    messageHandler_ = messageHandler;
    disconnectionHandler_ = disconnectionHandler;
}

ba::ip::tcp::socket& ControllerConnection::socket()
{
    return socket_;
}

void ControllerConnection::asyncWaitForMessage()
{
    ba::async_read(socket_,
		   ba::buffer(&headerBuffer_, dp::headerSize),
		   boost::bind(&ControllerConnection::handleHeaderRead, this,
			       ba::placeholders::error,
			       ba::placeholders::bytes_transferred));
}

void ControllerConnection::handleHeaderRead(const bs::error_code& error,
					    std::size_t bytesTransferred)
{
    if (! error) {
	assert(bytesTransferred == dp::headerSize);
	std::cout << "Message received: opcode=" << headerBuffer_.opcode
		  << " bodySize=" << headerBuffer_.bodySize << " from "
		  << socket_.remote_endpoint() << std::endl;

	if (headerBuffer_.bodySize > 0) {
	    bodyBuffer_.resize(headerBuffer_.bodySize);
	    ba::async_read(socket_,
			   ba::buffer(bodyBuffer_, headerBuffer_.bodySize),
			   boost::bind(&ControllerConnection::handleBodyRead,
				       this, ba::placeholders::error,
				       ba::placeholders::bytes_transferred));
	} else {
	    handleMessage();
	}

    } else {
	std::cerr << "Error reading message from "
		  << socket_.remote_endpoint() << ": " << error.message()
		  << " (" << error.value() << ")" << std::endl;
	disconnectionHandler_();
    }
}

void ControllerConnection::handleBodyRead(const bs::error_code& error,
					  std::size_t bytesTransferred)
{
    if (!error) {
	assert(bytesTransferred == headerBuffer_.bodySize);
	handleMessage();
    } else {
	std::cerr << "Error reading message from "
		  << socket_.remote_endpoint() << ": " << error.message()
		  << " (" << error.value() << ")" << std::endl;
	disconnectionHandler_();
    }
}

void ControllerConnection::handleWrite(const bs::error_code& error)
{
    if (error) {
	std::cerr << "Error writing message to" << socket_.remote_endpoint()
		  << ": " << error.message() << std::endl;
	exit(1);
    }
}

void ControllerConnection::handleMessage()
{
    std::auto_ptr<dpm::Message> message;
    message = dpm::Message::fromBits(headerBuffer_, bodyBuffer_);
    messageHandler_(message);
}

void ControllerConnection::sendMessage(dpm::Message *message)
{
    writeBuffer_ = message->toBits();
    ba::async_write(socket_, ba::buffer(writeBuffer_, writeBuffer_.size()),
		    boost::bind(&ControllerConnection::handleWrite, this,
				ba::placeholders::error));
}

