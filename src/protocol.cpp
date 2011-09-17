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

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "protocol.h"

namespace dp = dhtperfproto;
namespace dpm = dp::message;

// This virtual fuction is needed because we have a constructor, so if someone
// deletes a Message*, the derived destructor needs to be called
dpm::Message::~Message()
{}

std::auto_ptr<dpm::Message> dpm::Message::fromBits(
    const MessageHeader& header,
    const std::vector<char>& body)
{
    switch(header.opcode) {
    case opcode::DhtType:
	return std::auto_ptr<dpm::Message>(new DhtType(header, body));
    case opcode::Profile:
	return std::auto_ptr<dpm::Message>(new Profile(header, body));
    case opcode::Start:
	return std::auto_ptr<dpm::Message>(new Start(header, body));
    case opcode::Report:
	return std::auto_ptr<dpm::Message>(new Report(header, body));
    case opcode::Finish:
	return std::auto_ptr<dpm::Message>(new Finish(header, body));
    default:
	std::cout << "Protocol error: invalid opcode" << std::endl;
	exit(1);
    }
}

dpm::Message::Message(opcodeType opcode)
    : opcode_(opcode)
{}

dp::opcodeType dpm::Message::opcode()
{
    return opcode_;
}


// DhtType
// This message contains the header and the string containing the type.
dpm::DhtType::DhtType(const std::string& type)
    : Message(opcode::DhtType),
      type_(type)
{}

dpm::DhtType::DhtType(const MessageHeader& header,
		      const std::vector<char>& body)
    : Message(opcode::DhtType)
{
    type_ = std::string(&body[0], header.bodySize);
}

std::vector<char> dpm::DhtType::toBits()
{
    size_t size;
    MessageHeader header;

    header.opcode = opcode_;
    header.bodySize = type_.size();

    size = headerSize + header.bodySize;

    std::vector<char> bits(size, 0);
    memcpy(&bits[0], &header, headerSize);
    memcpy(&bits[headerSize], type_.c_str(), type_.size());
    return bits;
}

std::string dpm::DhtType::getType()
{
    return type_;
}

// Profile
// This message is formed by:
// - the header
// - a ProfileSizes struct containing the size of each profile string
// - each of the profile strings, in this order: join, leave, put, get
dpm::Profile::Profile(const std::string& joinProfile,
		      const std::string& leaveProfile,
		      const std::string& putProfile,
		      const std::string& getProfile)
    : Message(opcode::Profile),
      joinProfile_(joinProfile),
      leaveProfile_(leaveProfile),
      putProfile_(putProfile),
      getProfile_(getProfile)
{}

dpm::Profile::Profile(const MessageHeader& header,
		      const std::vector<char>& body)
    : Message(opcode::Profile)
{
    unsigned int copied = 0;

    ProfileSizes *profileSizes = (ProfileSizes*)&body[0];
    copied += sizeof(ProfileSizes);

    joinProfile_ = std::string(&body[copied], profileSizes->join);
    copied += profileSizes->join;
    leaveProfile_ = std::string(&body[copied], profileSizes->leave);
    copied += profileSizes->leave;
    putProfile_ = std::string(&body[copied], profileSizes->put);
    copied += profileSizes->put;
    getProfile_ = std::string(&body[copied], profileSizes->get);
    copied += profileSizes->get;

    assert(copied == header.bodySize);
}

std::vector<char> dpm::Profile::toBits()
{
    MessageHeader header;
    ProfileSizes profileSizes;
    size_t copied;
    size_t size;

    header.opcode = opcode_;
    header.bodySize = sizeof(ProfileSizes) +
		      joinProfile_.size() + leaveProfile_.size() +
		      putProfile_.size() + getProfile_.size();

    profileSizes.join = joinProfile_.size();
    profileSizes.leave = leaveProfile_.size();
    profileSizes.put = putProfile_.size();
    profileSizes.get = getProfile_.size();

    size = headerSize + header.bodySize;
    std::vector<char> bits(size, 0);

    memcpy(&bits[0], &header, headerSize);
    copied = headerSize;
    memcpy(&bits[copied], &profileSizes, sizeof(ProfileSizes));
    copied += sizeof(ProfileSizes);
    memcpy(&bits[copied], joinProfile_.c_str(), joinProfile_.size());
    copied += joinProfile_.size();
    memcpy(&bits[copied], leaveProfile_.c_str(), leaveProfile_.size());
    copied += leaveProfile_.size();
    memcpy(&bits[copied], putProfile_.c_str(), putProfile_.size());
    copied += putProfile_.size();
    memcpy(&bits[copied], getProfile_.c_str(), getProfile_.size());
    copied += getProfile_.size();
    assert(copied == size);
    return bits;
}

void dpm::Profile::getProfiles(std::string& join, std::string& leave,
			       std::string& put, std::string& get)
{
    join = joinProfile_;
    leave = leaveProfile_;
    put = putProfile_;
    get = getProfile_;
}

// Start
// This message is formed by:
// - the header
// - a timeUnit field containing the duration of the workload
// - an uint16_t containing the "port offset" to connect to
// - an uint16_t containing the "port offset" the node should open
// - a string containing the host to connect to (or empty string, in case the
//   node should open his own dht instance)
//
// Explaining the "port offset":
// - If you have more than one DHT client in the same machine, each client will
//   have to provide a different port for its DHT services. So for each DHT we
//   set a "port number" for the DHT, and the offset is just an increment to the
//   number. For example: if for some DHT the default is to provide port 1234, a
//   node with "offset 0" will provide port 1234. Then the next node on the same
//   machine will have "offset 1", so it will provide port 1235, and so on.
// - On our implementation, the Master counts the number of nodes for each IP
//   and then assigns offsets based on this.
// - Q: Why not just use different port numbers for each node on the DHT (and
//      not per-host?
//   A: Because it would limit the number of DHT clients to less than 65k,
//      which is not even close the the maximum we expect for real DHTs.
dpm::Start::Start(dp::timeUnit duration, const std::string& entryPointHost,
		  uint16_t entryPointPortOffset, uint16_t selfPortOffset)
    : Message(opcode::Start),
      duration_(duration),
      entryPointHost_(entryPointHost),
      entryPointPortOffset_(entryPointPortOffset),
      selfPortOffset_(selfPortOffset)
{}

dpm::Start::Start(const MessageHeader& header,
		  const std::vector<char>& body)
    : Message(opcode::Start)
{
    size_t copied = 0;
    memcpy(&duration_, &body[0], sizeof(duration_));
    copied += sizeof(duration_);
    memcpy(&entryPointPortOffset_, &body[copied],
	   sizeof(entryPointPortOffset_));
    copied += sizeof(entryPointPortOffset_);
    memcpy(&selfPortOffset_, &body[copied], sizeof(selfPortOffset_));
    copied += sizeof(selfPortOffset_);
    entryPointHost_ = std::string(&body[copied], header.bodySize - copied);

    std::cout << "start message: duration " << duration_ << ", entry point "
	      << entryPointHost_ << ":+" << entryPointPortOffset_
	      << ", self port: +" << selfPortOffset_ << "\n";
}

std::vector<char> dpm::Start::toBits()
{
    size_t size;
    size_t copied = 0;
    MessageHeader header;

    header.opcode = opcode_;
    header.bodySize = sizeof(duration_) +
		      sizeof(entryPointPortOffset_) +
		      sizeof(selfPortOffset_) +
		      entryPointHost_.size();
    size = headerSize + header.bodySize;

    std::vector<char> bits(size, 0);

    memcpy(&bits[0], &header, headerSize);
    copied += headerSize;
    memcpy(&bits[copied], &duration_, sizeof(duration_));
    copied += sizeof(duration_);
    memcpy(&bits[copied], &entryPointPortOffset_,
	   sizeof(entryPointPortOffset_));
    copied += sizeof(entryPointPortOffset_);
    memcpy(&bits[copied], &selfPortOffset_, sizeof(selfPortOffset_));
    copied += sizeof(selfPortOffset_);
    memcpy(&bits[copied], entryPointHost_.c_str(), entryPointHost_.size());
    copied += entryPointHost_.size();
    assert(copied == size);
    return bits;
}

dp::timeUnit dpm::Start::duration()
{
    return duration_;
}

std::string dpm::Start::entryPointHost()
{
    return entryPointHost_;
}

uint16_t dpm::Start::entryPointPortOffset()
{
    return entryPointPortOffset_;
}

uint16_t dpm::Start::selfPortOffset()
{
    return selfPortOffset_;
}

// Report
// The idea of this message is to be as generic and future-proof as possible.
// You can send this message multiple times, one for each report. Usually,
// one "Report" message is is sent for each metric obtained in the performance
// evaluation.
// This message is formed by:
// - A string containing the "type", which is usually the name of a metric. This
//   string cannot contain white spaces.
// - A white space.
// - Another string containing the report. This report should be passed to
//   some class constructor.

dpm::Report::Report(const std::string& type, const std::string& value)
    : Message(opcode::Report),
      type_(type),
      value_(value)
{}

dpm::Report::Report(const MessageHeader& header,
		    const std::vector<char>& body)
    : Message(opcode::Report)
{
    std::string bodyString(&body[0], body.size());
    size_t separatorPos = bodyString.find(' ');
    if (separatorPos == std::string::npos) {
	std::cerr << "Invalid Report message!\n";
	exit(1);
    }

    type_ = bodyString.substr(0, separatorPos);
    value_ = bodyString.substr(separatorPos+1);

    assert(header.bodySize == body.size());
}

std::vector<char> dpm::Report::toBits()
{
    size_t size;
    size_t copied = 0;
    MessageHeader header;

    header.opcode = opcode_;
    header.bodySize = type_.size() + value_.size() + sizeof(char);
    size = headerSize + header.bodySize;

    std::vector<char> bits(size, 0);
    memcpy(&bits[0], &header, headerSize);
    copied += headerSize;
    memcpy(&bits[copied], type_.c_str(), type_.size());
    copied += type_.size();
    bits[copied] = ' ';
    copied++;
    memcpy(&bits[copied], value_.c_str(), value_.size());
    copied += value_.size();
    assert(copied == size);

    std::cout << "Report::toBits. size=" << size << " header.bodySize="
	      << header.bodySize << "\n";
    return bits;
}

std::string dpm::Report::type()
{
    return type_;
}

std::string dpm::Report::value()
{
    return value_;
}

// Finish
// This message contains only the header, no more data.
dpm::Finish::Finish()
    : Message(opcode::Finish)
{}

dpm::Finish::Finish(const MessageHeader& header,
		    const std::vector<char>& /*body*/)
    : Message(opcode::Finish)
{
    assert(header.bodySize == 0);
}

std::vector<char> dpm::Finish::toBits()
{
    size_t size;
    MessageHeader header;

    header.opcode = opcode_;
    header.bodySize = 0;
    size = headerSize + header.bodySize;
    std::vector<char> bits(size, 0);
    memcpy(&bits[0], &header, size);
    return bits;
}

