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

#include <fstream>

#include "ResultProcessor.h"

ResultProcessor::ResultProcessor(const std::string& dhtType, bool lowLatency)
    : dhtType_(dhtType),
      joinLatency_( 500 * 1000),
      leaveLatency_(500 * 1000),
      putLatency_(   50 * 1000),
      getLatency_(   50 * 1000)
{
    // These magic numbers were obtained after a series of experiments
    if (lowLatency) {
	joinLatency_ = GeneralLatency( 250 * 1000);
	leaveLatency_ = GeneralLatency(250 * 1000);
	putLatency_ = GeneralLatency(   10 * 1000);
	getLatency_ = GeneralLatency(   10 * 1000);
    }

    // Maximum values for the plot:
    // -   5 * 1000 =  0.150s
    // -  10 * 1000 =  0.300s
    // -  50 * 1000 =  1.500s
    // -  75 * 1000 =  2.250s
    // - 100 * 1000 =  3.000s
    // - 250 * 1000 =  7.500s
    // - 500 * 1000 = 15.000s

    // The ideal solution would be to implement some function like
    // "GeneralLatency.saveToFile", then *after* the tests, you would run some
    // tool to load this file and print the CDF graph with any desired range.
}

void ResultProcessor::pushResult(const std::string& type,
				 const std::string& value)
{
    if (type == "JoinLatency") {
	pushLatency(Join, value);
    } else if (type == "LeaveLatency") {
	pushLatency(Leave, value);
    } else if (type == "PutLatency") {
	pushLatency(Put, value);
    } else if (type == "GetLatency") {
	pushLatency(Get, value);
    } else if (type == "JoinSuccessRate") {
	pushSuccessRate(Join, value);
    } else if (type == "GetSuccessRate") {
	pushSuccessRate(Get, value);
    } else if (type == "NodesStatus") {
	pushNodesStatus(value);
    } else {
	std::cerr << "Invalid result type: " << type << std::endl;
	exit(1);
    }
}

void ResultProcessor::pushLatency(DhtOperation operation,
				  const std::string& value)
{
    NodeLatency node(value);
    switch (operation) {
	case Join:
	    joinLatency_.push(node);
	    break;
	case Leave:
	    leaveLatency_.push(node);
	    break;
	case Put:
	    putLatency_.push(node);
	    break;
	case Get:
	    getLatency_.push(node);
	    break;
	default:
	    std::cerr << "Error: invalid DhtOperation for Latency metric: "
		      << operation << std::endl;
	    exit(EXIT_FAILURE);
    }
}

void ResultProcessor::pushSuccessRate(DhtOperation operation,
				      const std::string& value)
{
    NodeSuccessRate node(value);
    switch (operation) {
	case Join:
	    joinSuccessRate_.push(node);
	    break;
	case Get:
	    getSuccessRate_.push(node);
	    break;
	default:
	    std::cerr << "Error: invalid DhtOperation for SuccessRate metric: "
		      << operation << std::endl;
	    exit(EXIT_FAILURE);
    }
}

void ResultProcessor::pushNodesStatus(const std::string& value)
{
    nodesStatus_.set(value);
}

void ResultProcessor::processLatency(const std::string& outputFilePrefix,
				     const GeneralLatency& latency)
{
    std::cout << "- mean (us):          " << latency.meanInMicroseconds()
	      << std::endl;
    std::cout << "- mean of means (us): " << latency.meanOfMeansInMicroseconds()
	      << std::endl;
    std::cout << "- maximum mean (us):  " << latency.maxMeanInMicroseconds()
	      << std::endl;
    std::cout << "- minimum mean (us):  " << latency.minMeanInMicroseconds()
	      << std::endl;
    std::cout << "- maximum latency (us): "
	      << latency.generalMaxInMicroseconds() << std::endl;
    std::cout << "- minimum latency (us): "
	      << latency.generalMinInMicroseconds() << std::endl;
    std::cout << "- count: " << latency.count() << std::endl;

    std::string outputFile;

    // latency.csv:
    int colSize = 18;
    std::string colContent;

    outputFile = outputFilePrefix + "-latency.csv";
    std::ofstream dataFile(outputFile.c_str(), std::ios::app);

    if (!dataFile.is_open()) {
	std::cerr << "Error: can't open " << outputFile << std::endl;
	exit(EXIT_FAILURE);
    }

    // pos seems needed in Fedora 8 (gcc 4.1.2) to remove ambiguity in
    // "operator==" for the "dataFile.tellp() == 0" statement
    long pos = dataFile.tellp();
    if (pos == 0) {
	dataFile << std::left
		 << std::setw(colSize) << "# DHT type" << " "
		 << std::right
		 << std::setw(colSize) << "Min" << " "
		 << std::setw(colSize) << "Min mean"  << " "
		 << std::setw(colSize) << "Mean" << " "
		 << std::setw(colSize) << "Mean of means" << " "
		 << std::setw(colSize) << "Max mean" << " "
		 << std::setw(colSize) << "Max" << "\n";
    }

    dataFile << std::setiosflags(std::ios::fixed) << std::setprecision(6);

    colContent = '"' + dhtType_ + '"';
    dataFile << std::left << std::setw(colSize) << colContent;

    dataFile << std::right
	     << " " << std::setw(colSize) << latency.generalMinInMicroseconds()
	     << " " << std::setw(colSize) << latency.minMeanInMicroseconds()
	     << " " << std::setw(colSize) << latency.meanInMicroseconds()
	     << " " << std::setw(colSize) << latency.meanOfMeansInMicroseconds()
	     << " " << std::setw(colSize) << latency.maxMeanInMicroseconds()
	     << " " << std::setw(colSize) << latency.generalMaxInMicroseconds()
	     << "\n";
    dataFile.close();

    // latency-cdf.csv
    outputFile = outputFilePrefix + "-latency-cdf.csv";
    std::ofstream frequencyFile(outputFile.c_str(), std::ios::app);

    if (!frequencyFile.is_open()) {
	std::cerr << "Error: can't open " << outputFile << std::endl;
	exit(EXIT_FAILURE);
    }

    pos = frequencyFile.tellp();
    if (pos == 0) {
	frequencyFile << std::left
		      << std::setw(colSize) << "# Range " << " "
		      << std::right
		      << std::setw(colSize) << "CDF" << "\n";
    } else {
	frequencyFile << "\n\n";
    }

    frequencyFile << std::setiosflags(std::ios::fixed) << std::setprecision(6);
    frequencyFile << "# [" << dhtType_ << "]\n";

    long int interval;
    std::map<long int, uintmax_t> frequencyMap =
					latency.getFrequencyMap(interval);

    double total = (double) latency.count();
    long int accumulated = 0;

    frequencyFile << std::left << std::setw(colSize) << 0
		  << " " << std::right << std::setw(colSize) << 0 << "\n";
    for(int i = 0; i < 30; i++) {
	accumulated += frequencyMap[i];
	frequencyFile << std::left << std::setw(colSize)
		      << ((i+1) * interval) / 1000 /* miliseconds */
		      << " " << std::right << std::setw(colSize)
		      << accumulated / total << "\n";
    }

    frequencyFile.close();
}

void ResultProcessor::processSuccessRate(const std::string& outputFilePrefix,
					 const GeneralSuccessRate& successRate)
{
    std::cout << "- minimum: " << successRate.min() << std::endl;
    std::cout << "- maximum: " << successRate.max() << std::endl;
    std::cout << "- mean:    " << successRate.mean() << std::endl;

    int colSize = 18;
    std::string colContent;
    std::string outputFile = outputFilePrefix + "-success-rate.csv";
    std::ofstream dataFile(outputFile.c_str(), std::ios::app);

    if (!dataFile.is_open()) {
	std::cerr << "Error: can't open " << outputFile << std::endl;
	exit(EXIT_FAILURE);
    }

    // pos seems needed in Fedora 8 (gcc 4.1.2) to remove ambiguity in
    // "operator==" for the "dataFile.tellp() == 0" statement
    long pos = dataFile.tellp();
    if (pos == 0) {
	dataFile << std::left
		 << std::setw(colSize) << "# DHT type" << " "
		 << std::right
		 << std::setw(colSize) << "Min" << " "
		 << std::setw(colSize) << "Mean" << " "
		 << std::setw(colSize) << "Max" << "\n";
    }

    dataFile << std::setiosflags(std::ios::fixed) << std::setprecision(6);

    colContent = '"' + dhtType_ + '"';
    dataFile << std::left << std::setw(colSize) << colContent;

    dataFile << std::right
	     << " " << std::setw(colSize) << successRate.min()
	     << " " << std::setw(colSize) << successRate.mean()
	     << " " << std::setw(colSize) << successRate.max()
	     << "\n";
    dataFile.close();
}

void ResultProcessor::processNodesStatus()
{
    std::cout << "- finished:   " << nodesStatus_.finished() << std::endl;
    std::cout << "- failed:     " << nodesStatus_.failed() << std::endl;
    std::cout << "- unfinished: " << nodesStatus_.unfinished() << std::endl;

    std::ofstream dataFile("nodes-status.csv", std::ios::app);

    if (!dataFile.is_open()) {
	std::cerr << "Error: can't open nodes-status.csv" << std::endl;
	exit(EXIT_FAILURE);
    }

    int colSize = 18;
    std::string colContent;

    long pos = dataFile.tellp();
    if (pos == 0) {
	dataFile << std::left
		 << std::setw(colSize) << "# DHT type " << " "
		 << std::right
		 << std::setw(colSize) << "Finished" << " "
		 << std::setw(colSize) << "Failed" << " "
		 << std::setw(colSize) << "Unfinished" << "\n";
    }

    colContent = '"' + dhtType_ + '"';
    dataFile << std::left << std::setw(colSize) << colContent;

    dataFile << std::right
	     << " " << std::setw(colSize) << nodesStatus_.finished()
	     << " " << std::setw(colSize) << nodesStatus_.failed()
	     << " " << std::setw(colSize) << nodesStatus_.unfinished()
	     << "\n";
    dataFile.close();
}

void ResultProcessor::process()
{
    std::cout << "Results for JOIN latency:" << std::endl;
    processLatency("join", joinLatency_);
    std::cout << "Results for LEAVE latency:" << std::endl;
    processLatency("leave", leaveLatency_);
    std::cout << "Results for PUT latency:" << std::endl;
    processLatency("put", putLatency_);
    std::cout << "Results for GET latency:" << std::endl;
    processLatency("get", getLatency_);
    std::cout << "Results for JOIN success rate:" << std::endl;
    processSuccessRate("join", joinSuccessRate_);
    std::cout << "Results for GET success rate:" << std::endl;
    processSuccessRate("get", getSuccessRate_);
    std::cout << "Results for Nodes Status:" << std::endl;
    processNodesStatus();
}

