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

#include <cstdio>
#include <iostream>

#include <boost/asio.hpp>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "Utils.h"

namespace ba = boost::asio;
namespace bs = boost::system;

const char *Utils::dictionary[] = {
    "refuse",
    "punch",
    "couple",
    "index",
    "banknote",              // 5
    "silverberry",
    "Manchu",
    "open",
    "overcome",
    "warfighter",            // 10
    "round",
    "angle",
    "German",
    "disturb",
    "Schmidt",               // 15
    "selfless",
    "bioptic",
    "rally",
    "osteoarthropathy",
    "August",                // 20
    "desist",
    "dessjatines",
    "ads",
    "catodi",
    "gutsing",               // 25
    "hinder",
    "furrow",
    "saponify",
    "relief",
    "complicated",           // 30
    "blizzarded",
    "endure",
    "wonder",
    "marvel",
    "beget",                 // 35
    "lay",
    "delirium",
    "Palmiro",
    "I",
    "royalty",               // 40
    "spilths",
    "conferencegoers",
    "reservoirs",
    "transient",
    "taqwa",                 // 45
    "giant",
    "clicky",
    "static_line",
    "coexistences",
    "transducer",            // 50
    "ghits",
    "rattletrap",
    "comedogenic",
    "propelling_pencil",
    "neutrality",            // 55
    "calendrers",
    "glossed_over",
    "coexisted",
    "chafed",
    "dyspareunia",           // 60
    "fictionalizes",
    "high-water_mark",
    "beach_head",
    "potsies",
    "bummer",                // 65
    "scraps",
    "aird-guitared",
    "preordered",
    "quadrantanopia",
    "Sacramentarians",       // 70
    "kinesthesiologists",
    "flounced",
    "timber_wolf",
    "loblolly",
    "way_back_when",         // 75
    "agricultural_liens",
    "in_the_nick_of_time",
    "Baldwin's_phosphorus",
    "Eichmann",
    "papillote",             // 80
    "hackleback",
    "Magha_Puja",
    "sectored",
    "bell_ringers",
    "eyeopening",            // 85
    "omphalomancy",
    "noncitrus",
    "uncovers",
    "chopins",
    "microlenders",          // 90
    "marginal_benefits",
    "err_on_the_side_of_caution",
    "loveliest",
    "unanchor",
    "Trenchardian",          // 95
    "kangaroo_paw",
    "commissural",
    "diminished_radix_complement",
    "casters",
    "ET743",                 // 100
    "firebases",
    "unpunished",
    "replayable",
    "hotshot",
    "eigendecomposition",    // 105
    "Fennicism",
    "prima_ballerinas",
    "subdural",
    "scaggning",
    "audio_cassete",         // 110
    "athletic",
    "giaour",
    "buccal_cavity",
    "nonchromatically",
    "booting_up",            // 115
    "dosed",
    "cat's_pajamas",
    "epiphenomenalism",
    "glass_transition_temperature",
    "honing",                // 120
};

const unsigned int Utils::dictionarySize =
    sizeof(dictionary) / sizeof(dictionary[0]);

// Random device

Utils::RandomDevice::RandomDevice()
{
    file_.open("/dev/urandom", std::ios::binary);
    if (!file_.is_open()) {
	std::cerr << "Error: can't open /dev/urandom";
	exit(EXIT_FAILURE);
    }
}

Utils::RandomDevice::~RandomDevice()
{
    file_.close();
}

unsigned int Utils::RandomDevice::operator()()
{
    unsigned int ret;
    file_.read((char *)&ret, sizeof(ret));
    return ret;
}

// Random dictionary

Utils::RandomDictionary::RandomDictionary(unsigned int size)
    : engine_(randomDev_()),
      distribution_(0, size),
      generator_(engine_, distribution_)
{
    if (size >= Utils::dictionarySize) {
	std::cerr << "Error: RandomDictionary is too big. Max size: "
		  << Utils::dictionarySize << std::endl;
	exit(EXIT_FAILURE);
    }
}

const char *Utils::RandomDictionary::getRandomWord()
{
    return dictionary[generator_()];
}


// UniformRandomNumber

Utils::UniformRandomNumber::UniformRandomNumber(int min, int max)
    : engine_(randomDev_()),
      distribution_(min, max),
      generator_(engine_, distribution_)
{}

int Utils::UniformRandomNumber::get()
{
    return generator_();
}

// PoissonRandomNumber

Utils::PoissonRandomNumber::PoissonRandomNumber(double lambda)
    : engine_(randomDev_()),
      distribution_(lambda),
      generator_(engine_, distribution_)
{}

int Utils::PoissonRandomNumber::get()
{
    return generator_();
}

// WeibullRandomNumer

Utils::WeibullRandomNumber::WeibullRandomNumber(double shape, double scale)
    : engine_(randomDev_()),
      generator_(engine_, uniformDistribution_),
      weibullDistribution_(shape, scale)
{}

double Utils::WeibullRandomNumber::get()
{
    return quantile(weibullDistribution_, generator_());
}

// Others

void Utils::removeEmptyStringsFromVector(std::vector<std::string>& v)
{
    unsigned int i;
    // This loop is slooooow (removing elements from vectors).
    // If we were using lists, we could do something like:
    // paramsList.remove_if(boost::bind(std::equal_to<std::string>(), _1, ""));
    for(i = v.size(); i > 0; i--) {
        if (v[i-1].empty()) {
            v.erase(v.begin()+i-1);
        }
    }
}

int Utils::getTimeFromString(const std::string& timeString)
{
    // This function accepts two "time" formats:
    // - "X", which means an exact time
    // - "X-Y", which means an interval. In this case, we select a random
    //   number inside this interval to return
    int rc, min, max;
    rc = sscanf(timeString.c_str(), "%d-%d", &min, &max);

    if (rc == 0 || (rc == 2 && min >= max)) {
	std::cerr << "Error: " << timeString << " is not an acceptable time "
		  << "string\n";
	exit(EXIT_FAILURE);
    }

    if (rc == 1)
	return min;

    // FIXME: instantiating this every single function call is BAD
    UniformRandomNumber r(min, max);
    return r.get();
}

void Utils::mkdir_p(const char *dirName)
{
    struct stat statBuffer;

    if (stat(dirName, &statBuffer) != 0) {

        if (errno == ENOENT) {
            if (mkdir(dirName, 0755) != 0) {
                std::cerr << "Error: can't create " << dirName << std::endl;
                exit(EXIT_FAILURE);
            }
        } else {
            std::cerr << "Error: can't stat " << dirName << std::endl;
            exit(EXIT_FAILURE);
        }
    } else if (!S_ISDIR(statBuffer.st_mode)) {
        std::cout << "Error: " << dirName << " is not a directory" << std::endl;
        exit(EXIT_FAILURE);
    }
}

std::string Utils::getSelfIp()
{
    return getHostIp(ba::ip::host_name());
}

std::string Utils::getHostIp(const std::string& host)
{
    ba::io_service ioService;

    bs::error_code error;

    ba::ip::tcp::resolver resolver(ioService);
    ba::ip::tcp::resolver::query query(host, "");
    ba::ip::tcp::resolver::iterator endpointIterator = resolver.resolve(query,
									error);
    ba::ip::tcp::resolver::iterator endpointIteratorEnd;
    ba::ip::tcp::endpoint endpoint;


    if (error) {
        std::cerr << "Error getting host \"" << host << "\"'s ip: "
		  << error.message() << std::endl;
        exit(EXIT_FAILURE);
    }

    if (endpointIterator != endpointIteratorEnd) {
	endpoint = *endpointIterator;
	return endpoint.address().to_v4().to_string();
    }

    exit(EXIT_FAILURE);
}

