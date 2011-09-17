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

#ifndef DHTPERF_CONTROLLER_DHTNODES_KADDEMOIDWRAPPER_H
#define DHTPERF_CONTROLLER_DHTNODES_KADDEMOIDWRAPPER_H

// For some reason, when you start kaddemo_static, you need to know the node ID
// of your node's entry point. Since we only have the entry point host and ip
// address, this class impelemnts a "server" that can be contacted through
// ip/port and answers the node ID.

#include <string>
#include <boost/asio.hpp>

class KaddemoIdClientConnection;

class KaddemoIdServer
{
    public:
        KaddemoIdServer(const std::string& id, int port);
        void run(const bool *pStop);

    private:
        std::string id_;
        boost::asio::io_service ioService_;
        boost::asio::ip::tcp::acceptor acceptor_;

        void asyncAccept();
        void handleAccept(KaddemoIdClientConnection *client,
                          const boost::system::error_code& error);
};

class KaddemoIdClient
{
    public:
        KaddemoIdClient(const std::string& serverHost, int serverPort);
        std::string getId();

    private:
        std::string serverHost_;
        int serverPort_;

        boost::asio::io_service ioService_;
        boost::asio::ip::tcp::socket socket_;

        void connect();
};

#endif
