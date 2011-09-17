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

#ifndef DHTPERF_MASTER_CONTROLLER_CONNECTION
#define DHTPERF_MASTER_CONTROLLER_CONNECTION

#include <boost/asio.hpp>
#include <boost/function.hpp>

#include "../protocol.h"

class ControllerConnection {
    public:
	typedef	boost::function<
		void(std::auto_ptr<dhtperfproto::message::Message>)>
		messageCallbackFunction;
	typedef boost::function<void()> disconnectionCallbackFunction;

	ControllerConnection(boost::asio::io_service& ioService);
	~ControllerConnection();

	boost::asio::ip::tcp::socket& socket();
	void asyncWaitForMessage();

	void setHandlers(messageCallbackFunction handler,
			 disconnectionCallbackFunction);
	void sendMessage(dhtperfproto::message::Message *message);

    private:

	// This function allocates memory for bodyBuffer_ when needed, but
	// doesn't free it: it's up to the caller
	void handleHeaderRead(const boost::system::error_code &error,
			      std::size_t bytesTransferred);
	void handleBodyRead(const boost::system::error_code &error,
			    std::size_t bytesTransferred);
	void handleWrite(const boost::system::error_code &error);
	void handleMessage();

	boost::asio::ip::tcp::socket socket_;

	// Buffers for reading messages
	dhtperfproto::MessageHeader headerBuffer_;
	std::vector<char> bodyBuffer_;
	// Buffer for writing messages
	std::vector<char> writeBuffer_;

	messageCallbackFunction messageHandler_;
	disconnectionCallbackFunction disconnectionHandler_;
};

#endif
