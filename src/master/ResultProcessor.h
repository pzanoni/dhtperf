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

#ifndef DHTPERF_MASTER_RESULT_PROCESSOR_H
#define DHTPERF_MASTER_RESULT_PROCESSOR_H

#include <string>

#include "../common.h"
#include "../metrics/GeneralLatency.h"
#include "../metrics/GeneralSuccessRate.h"
#include "../metrics/NodesStatus.h"

class ResultProcessor
{
    public:
	ResultProcessor(const std::string& dhtType, bool lowLatency);
	void pushResult(const std::string& type, const std::string& value);
	void process();
    private:
	std::string dhtType_;
	GeneralLatency joinLatency_;
	GeneralLatency leaveLatency_;
	GeneralLatency putLatency_;
	GeneralLatency getLatency_;
	GeneralSuccessRate joinSuccessRate_;
	GeneralSuccessRate getSuccessRate_;
	NodesStatus nodesStatus_;

	void pushLatency(DhtOperation operation, const std::string& value);
	void pushSuccessRate(DhtOperation operation, const std::string& value);
	void pushNodesStatus(const std::string& value);

	void processLatency(const std::string& outputFilePrefix,
			    const GeneralLatency& latency);
	void processSuccessRate(const std::string& outputFilePrefix,
				const GeneralSuccessRate& successRate);
	void processNodesStatus();
};

#endif
