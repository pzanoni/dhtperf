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

#ifndef DHTPERF_CONTROLLER_DHTNODE_H
#define DHTPERF_CONTROLLER_DHTNODE_H

#include <list>
#include <memory>
#include <string>

#include "../events.h"
#include "../metrics/NodeLatency.h"
#include "../metrics/NodeSuccessRate.h"

class DhtNode {
    public:
	static std::auto_ptr<DhtNode> make(std::string type);

	DhtNode();
	virtual ~DhtNode();

	// This is the ugly part:
	void setEntryPoint(std::string host, uint16_t portOffset);
	void setSelfPortOffset(uint16_t portOffset);

	void processJoinEvent(const DhtJoinEvent& event);
	void processLeaveEvent(const DhtLeaveEvent& event);
	void processPutEvent(const DhtPutEvent& event);
	void processGetEvent(const DhtGetEvent& event);

	void getResults(std::string& joinLatencyString,
			std::string& leaveLatencyString,
			std::string& putLatencyString,
			std::string& getLatencyString,
			std::string& joinSuccessRateString,
			std::string& getSuccessRateString);

	bool connected();
    protected:
	bool connected_;

	std::string entryPointHost_;
	uint16_t entryPointPort_;
	uint16_t selfPort_;

	bool nodeStartsNewDht();

    private:
	NodeLatency joinLatency_;
	NodeLatency leaveLatency_;
	NodeLatency putLatency_;
	NodeLatency getLatency_;
	NodeSuccessRate joinSuccessRate_;
	NodeSuccessRate getSuccessRate_;

	virtual bool doJoin() = 0;
	virtual void doLeave() = 0;
	virtual void doPut(const DhtPutEvent& event) = 0;
	virtual bool doGet(const DhtGetEvent& event) = 0;

	virtual uint16_t portOffsetToPort(uint16_t offset) = 0;
};

#endif
