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
#include "DhtperfMaster.h"

namespace po = boost::program_options;
namespace dp = dhtperfproto;

int main(int argc, char *argv[])
{
    int port, nodes;
    bool lowLatency = false;
    std::string workloadFile;
    std::string dhtType;

    std::string usageString = "Usage: " + std::string(argv[0]) + " [options]";

    po::options_description opts(usageString);
    opts.add_options()
	("help,h", "prints this help")
	("version,v", "prints program version")
	("port,p", po::value<int>(&port)->default_value(dp::DefaultMasterPort),
	 "specifies the port")
	("workload-file,w",
	 po::value<std::string>(&workloadFile)->
	    default_value("example-workload"),
	 "specifies the workload file")
	("dht,d", po::value<std::string>(&dhtType), "specifies the DHT type")
	("nodes,n", po::value<int>(&nodes), "number of nodes")
	("low-latency,l",
	 "adjusts metrics to low latency networks (for CDF plots)");

    po::variables_map vm;

    try {
	po::store(po::parse_command_line(argc, argv, opts), vm);
    } catch (std::exception& e) {
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

    if (dhtType == "") {
	std::cerr << "Error: DHT not specified" << std::endl;
	return 1;
    } else {
	if (dhtType != "dummy"        && dhtType != "kaddemo" &&
	    dhtType != "openchord"    && dhtType != "owshellchord" &&
	    dhtType != "owshellkad"   && dhtType != "owshellpastry" &&
	    dhtType != "pastconsole") {
	    std::cerr << "Error: invalid DHT \"" << dhtType << "\"\n"
		      << "Valid DHTs: dummy kaddemo openchord owshellchord "
		      << "owshellkad owshellpastry pastconsole" << std::endl;
	    return 1;
	}
    }

    if (vm.count("low-latency")) {
	lowLatency = true;
    }

    DhtperfMaster master(port, dhtType, lowLatency);

    if (vm.count("nodes")) {
	master.loadWorkloadFile(workloadFile, nodes);
    } else {
	master.loadWorkloadFile(workloadFile);
    }

    master.run();
    return 0;
}
