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
#include <iostream>
#include <string>
#include <vector>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>

#include "KaddemoIdWrapper.h"

namespace ba = boost::asio;
namespace bs = boost::system;


// KaddemoIdClientConnection

class KaddemoIdClientConnection
{
    public:
	KaddemoIdClientConnection(ba::io_service& ioService);
	void sendId(const std::string& id);
	ba::ip::tcp::socket& socket();

    private:
	ba::ip::tcp::socket socket_;
	void handleWrite(const bs::error_code& error);
};

KaddemoIdClientConnection::KaddemoIdClientConnection(ba::io_service& ioService)
    : socket_(ioService)
{}

void KaddemoIdClientConnection::sendId(const std::string& id)
{
    ba::async_write(socket_, ba::buffer(id, id.size()),
		    boost::bind(&KaddemoIdClientConnection::handleWrite, this,
				ba::placeholders::error));
}

void KaddemoIdClientConnection::handleWrite(const bs::error_code& error)
{
    if (error) {
	std::cerr << "Error writing message to " << socket_.remote_endpoint()
		  << ": " << error.message() << "\n";
	exit(EXIT_FAILURE);
    }
    delete this;
}

ba::ip::tcp::socket& KaddemoIdClientConnection::socket()
{
    return socket_;
}


// KaddemoIdServer

KaddemoIdServer::KaddemoIdServer(const std::string& id, int port)
    : id_(id),
      acceptor_(ioService_, ba::ip::tcp::endpoint(ba::ip::tcp::v4(), port))
{}

void KaddemoIdServer::run(const bool *pStop)
{
    asyncAccept();

    while (!*pStop) {
	ioService_.poll_one();
    }
    ioService_.stop();
}

void KaddemoIdServer::asyncAccept()
{
    KaddemoIdClientConnection *client = new
	KaddemoIdClientConnection(ioService_);
    acceptor_.async_accept(client->socket(),
			   boost::bind(&KaddemoIdServer::handleAccept, this,
				       client, ba::placeholders::error));
}

void KaddemoIdServer::handleAccept(KaddemoIdClientConnection *client,
				   const bs::error_code& error)
{
    if (error) {
	std::cout << "Accept error: " << error.message() << "\n";
	exit(EXIT_FAILURE);
    }

    std::string idstring(id_ + "\n");
    client->sendId(idstring);

    asyncAccept();
}


// KaddemoIdClient

KaddemoIdClient::KaddemoIdClient(const std::string& serverHost, int serverPort)
    : serverHost_(serverHost),
      serverPort_(serverPort),
      socket_(ioService_)
{}

void KaddemoIdClient::connect()
{
    std::string serverPortString =
	boost::lexical_cast<std::string>(serverPort_);

    ba::ip::tcp::resolver resolver(ioService_);
    ba::ip::tcp::resolver::query query(serverHost_, serverPortString);
    ba::ip::tcp::endpoint endpoint;
    ba::ip::tcp::resolver::iterator endpointIterator = resolver.resolve(query);
    ba::ip::tcp::resolver::iterator endpointIteratorEnd;
    bs::error_code error;

    while (endpointIterator != endpointIteratorEnd) {
	endpoint = *endpointIterator;
	std::cout << "Endpoint: " << endpoint << "\n";
	socket_.connect(endpoint, error);
	if (!error)
	    break;
	else
	    std::cerr << "Error connecting to endpoint: " << error.message()
		      << "\n";

	++endpointIterator;
    }

    if (error) {
	std::cerr << "Error connecting: " << error.message() << "\n";
	exit(EXIT_FAILURE);
    }
}

std::string KaddemoIdClient::getId()
{
    connect();

    ba::streambuf buffer;
    bs::error_code error;

    ba::read_until(socket_, buffer, '\n', error);
    if (error) {
	std::cout << "Error getting id: " << error.message() << "\n";
	exit(EXIT_FAILURE);
    }

    std::istream is(&buffer);
    std::string id;
    std::getline(is, id);

    return id;
}
