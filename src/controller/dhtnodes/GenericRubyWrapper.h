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

#ifndef DHTPERF_CONTROLLER_GENERIC_RUBY_WRAPPER_H
#define DHTPERF_CONTROLLER_GENERIC_RUBY_WRAPPER_H

#include <string>

#include "../DhtNode.h"
#include "RWPopen.h"

class GenericRubyWrapper : public DhtNode {
    public:
	GenericRubyWrapper(const std::string& dhtType);
	~GenericRubyWrapper();
    private:
	bool doJoin();
	void doLeave();
	void doPut(const DhtPutEvent& event);
	bool doGet(const DhtGetEvent& event);

	uint16_t portOffsetToPort(uint16_t offset);

	std::auto_ptr<RWPopen> wrapperPipe_;
	std::string dhtType_;

	void waitPrompt();
	std::string getNonCommentLine();
};

#endif
