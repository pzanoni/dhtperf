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

#ifndef DHTPERF_CONTROLLER_UTILS_H
#define DHTPERF_CONTROLLER_UTILS_H

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <boost/tr1/random.hpp>
#include <boost/math/distributions/weibull.hpp>

class Utils
{
    public:
	// This dictionary is meant to be used by Profiles that rely on random
	// sets of words. This way, different nodes can use the same set of
	// "random" words in their puts and gets.
	static const char *dictionary[];
	static const unsigned int dictionarySize;

	// Boost's std::tr1::random_device is not shipped in distros
	class RandomDevice
	{
	    public:
		RandomDevice();
		~RandomDevice();

		unsigned int operator()();
	    private:
		std::ifstream file_;
	};

	// Gets random words from dictionary_ from a set of size "size"
	class RandomDictionary
	{
	    public:
		RandomDictionary(unsigned int size);
		const char *getRandomWord();
	    private:
		RandomDevice randomDev_;
		std::tr1::mt19937 engine_;
		std::tr1::uniform_int<int> distribution_;
		std::tr1::variate_generator<std::tr1::mt19937&,
		    std::tr1::uniform_int<int> > generator_;
	};

	class UniformRandomNumber
	{
	    public:
		UniformRandomNumber(int min, int max);
		int get();
	    private:
		RandomDevice randomDev_;
		std::tr1::mt19937 engine_;
		std::tr1::uniform_int<> distribution_;
		std::tr1::variate_generator<std::tr1::mt19937&,
		    std::tr1::uniform_int<> > generator_;
	};

	// Get a random number using the Poisson distribution with the specified
	// lambda
	class PoissonRandomNumber
	{
	    public:
		PoissonRandomNumber(double lambda);
		int get();
	    private:
		RandomDevice randomDev_;
		std::tr1::mt19937 engine_;
		std::tr1::poisson_distribution<> distribution_;
		std::tr1::variate_generator<std::tr1::mt19937&,
		    std::tr1::poisson_distribution<> > generator_;
	};

	// Get a random number using the Weibull distribution with the specified
	// shape and scale
	class WeibullRandomNumber
	{
	    public:
		WeibullRandomNumber(double shape, double scale);
		double get();
	    private:
		// We generate integers and then transform them into the weibul
		// distribution using boost::math
		RandomDevice randomDev_;
		std::tr1::mt19937 engine_;
		std::tr1::uniform_real<double> uniformDistribution_;
		std::tr1::variate_generator<std::tr1::mt19937&,
		    std::tr1::uniform_real<double> > generator_;
		boost::math::weibull_distribution<double> weibullDistribution_;
	};

	static void removeEmptyStringsFromVector(std::vector<std::string>& v);
	static int getTimeFromString(const std::string& timeString);

	static void mkdir_p(const char *dirName);
	static std::string getSelfIp();
	static std::string getHostIp(const std::string& host);
};

#endif
