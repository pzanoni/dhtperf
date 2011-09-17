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
#include <string>

#include <boost/program_options.hpp>

#include "config.h"
#include "../protocol.h"
#include "DhtperfController.h"

namespace po = boost::program_options;
namespace dp = dhtperfproto;

int main(int argc, char *argv[])
{
    int masterPort;
    std::string masterHost;

    std::string usageString = "Usage: " + std::string(argv[0]) +
			      " [options] host";
    po::options_description opts(usageString);
    opts.add_options()
	("help,h", "prints this help")
	("version,v", "prints program version")
	("host,o",
	    po::value<std::string>(&masterHost)->default_value("localhost"),
	    "(positional) specifies the host")
	("port,p",
	    po::value<int>(&masterPort)->default_value(dp::DefaultMasterPort),
	    "specifies the port");
    po::positional_options_description positional;
    positional.add("host", 1);
    po::variables_map vm;

    try {
	po::store(po::command_line_parser(argc, argv)
		    .options(opts).positional(positional).run(),
		  vm);
    } catch (std::exception &e) {
	std::cerr << "Error: " << e.what() << "\n" << opts << std::endl;
	return 1;
    }
    po::notify(vm);

    if (vm.count("help")) {
	std::cout << opts << std::endl;
	return 0;
    }

    if (vm.count("version")) {
	std::cout << "dhtperf version " << DHTPERF_VERSION << std::endl;
	return 0;
    }

    {
	DhtperfController controller(masterHost, masterPort);
	controller.run();
    }
    std::cout << "Done" << std::endl;
    return 0;
}
