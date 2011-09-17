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

// boost::accumulators is too old for PlanetLab =(

#ifndef DHTPERF_METRICS_STATISTICS_H
#define DHTPERF_METRICS_STATISTICS_H

#include <map>
#include <boost/cstdint.hpp>

// This class provides statistics about the data you push to it.
// How to use: you use "push" to insert elements, then anytime you can use the
// other functions to query the results
template <class T>
class Statistics
{
    public:
	Statistics(T frequencyMapInterval = 0);

	void push(T element);

	// Mean of all element values
	double mean() const;
	// Biggest element seen
	T max() const;
	// Smallest element seen
	T min() const;
	// Number of elements seen
	uintmax_t count() const;
	// Map of the elements seen, grouped by frequencyMapInterval_ interval
	std::map<T, uintmax_t> getFrequencyMap(T& interval) const;

    private:
	double mean_;
	T max_;
	T min_;
	uintmax_t count_;
	T frequencyMapInterval_;
	std::map<T, uintmax_t> frequencyMap_;
};

template <class T>
Statistics<T>::Statistics(T frequencyMapInterval)
    : mean_(0.0),
      count_(0),
      frequencyMapInterval_(frequencyMapInterval)
{}

template <class T>
void Statistics<T>::push(T element)
{
    if (count_ == 0) {
	max_ = element;
	min_ = element;
    } else {
	if (max_ < element)
	    max_ = element;
	if (min_ > element)
	    min_ = element;
    }
    count_++;

    mean_ += (element - mean_) / (double) count_;

    if (frequencyMapInterval_)
	frequencyMap_[element / frequencyMapInterval_]++;
}

template <class T>
double Statistics<T>::mean() const
{
    return mean_;
}

template <class T>
T Statistics<T>::max() const
{
    return max_;
}

template <class T>
T Statistics<T>::min() const
{
    return min_;
}

template <class T>
uintmax_t Statistics<T>::count() const
{
    return count_;
}

template<class T>
std::map<T, uintmax_t> Statistics<T>::getFrequencyMap(T& interval) const
{
    interval = frequencyMapInterval_;
    return frequencyMap_;
}

#endif
