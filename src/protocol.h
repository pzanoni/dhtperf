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

#ifndef DHTPERF_PROTOCOL
#define DHTPERF_PROTOCOL

#include <memory>
#include <string>
#include <vector>

#include <boost/cstdint.hpp>

namespace dhtperfproto
{
    // I know I could have used char, but int makes memory alignment easier
    // Btw, I'm assigning numbers because someone might want to implement the
    // controller in another language...

    typedef int32_t opcodeType;
    typedef uint32_t messageSizeType;

    typedef uint32_t timeUnit;

    const int DefaultMasterPort = 1810;

    typedef struct {
	opcodeType opcode;
	messageSizeType bodySize;
    } MessageHeader;

    const size_t headerSize = sizeof(MessageHeader);

    namespace opcode
    {
	const opcodeType DhtType = 1;
	const opcodeType Profile = 2;
	const opcodeType Start   = 3;
	const opcodeType Report  = 4;
	const opcodeType Finish  = 5;
    }

    // Amount of time the node has to report its results and finish, after
    // the execution of the workload
    const int ReportAndFinishTimeout = 90;

    // XXX: we assume a char has 8 bits everywhere

    namespace message
    {
	class Message {
	    public:
		// Factory:
		static std::auto_ptr<Message> fromBits(
		    const MessageHeader& header,
		    const std::vector<char>& body);
		virtual ~Message();
		// XXX: is there a way to force derived classes to implement a
		// constructor?
		virtual std::vector<char> toBits() = 0;
		opcodeType opcode();
	    protected:
		Message(opcodeType opcode);
		const opcodeType opcode_;
	};

	class DhtType : public Message {
	    public:
		DhtType(const std::string& type);
		DhtType(const MessageHeader& header,
		        const std::vector<char>& body);
		std::vector<char> toBits();

		std::string getType();
	    private:
		std::string type_;
	};

	class Profile : public Message {
	    public:
		Profile(const std::string& joinProfile,
			const std::string& leaveProfile,
			const std::string& putProfile,
			const std::string& getProfile);
		Profile(const MessageHeader& header,
			const std::vector<char>& body);
		std::vector<char> toBits();

		void getProfiles(std::string& join, std::string& leave,
				 std::string& put, std::string& get);

	    private:
		typedef struct {
		    int32_t join;
		    int32_t leave;
		    int32_t put;
		    int32_t get;
		} ProfileSizes;

		std::string joinProfile_;
		std::string leaveProfile_;
		std::string putProfile_;
		std::string getProfile_;
	};

	class Start : public Message {
	    public:
		Start(timeUnit duration, const std::string& entryPointHost,
		      uint16_t entryPointPortOffset, uint16_t selfPortOffset);
		Start(const MessageHeader& header,
		      const std::vector<char>& body);
		std::vector<char> toBits();
		timeUnit duration();
		std::string entryPointHost();
		uint16_t entryPointPortOffset();
		uint16_t selfPortOffset();
	    private:
		timeUnit duration_;
		std::string entryPointHost_;
		uint16_t entryPointPortOffset_;
		uint16_t selfPortOffset_;
	};

	class Report : public Message {
	    public:
		Report(const std::string& type, const std::string& value);
		Report(const MessageHeader& header,
		       const std::vector<char>& body);
		std::vector<char> toBits();
		std::string type();
		std::string value();
	    private:
		std::string type_;
		std::string value_;
	};

	class Finish : public Message {
	    public:
		Finish();
		Finish(const MessageHeader& header,
		       const std::vector<char>& body);
		std::vector<char> toBits();
	};
    }
}

#endif
