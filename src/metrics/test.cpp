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
#include <iostream>
#include <string>

#include "NodeLatency.h"
#include "GeneralLatency.h"
#include "NodeSuccessRate.h"
#include "GeneralSuccessRate.h"

namespace bpt = boost::posix_time;
namespace bacc = boost::accumulators;

long int latencyGeneralMax = -1;
long int latencyGeneralMin = -1;

void makeNodeLatency(NodeLatency& nl)
{
    std::cout << "Making node latency\n";
    bpt::ptime pt1(bpt::microsec_clock::universal_time());
    std::cout << "  Time now: " << pt1 << "\n";
    bpt::ptime pt2(bpt::microsec_clock::universal_time());
    std::cout << "  Time now: " << pt2 << "\n";
    bpt::ptime pt3(bpt::microsec_clock::universal_time());
    std::cout << "  Time now: " << pt3 << "\n";

    bpt::time_duration td1(pt2 - pt1);
    std::cout << "  Duration 1: " << td1.total_microseconds() << "\n";
    bpt::time_duration td2(pt3 - pt1);
    std::cout << "  Duration 2: " << td2.total_microseconds() << "\n";

    if (td1.total_microseconds() > latencyGeneralMax || latencyGeneralMax == -1)
	latencyGeneralMax = td1.total_microseconds();
    if (td2.total_microseconds() > latencyGeneralMax || latencyGeneralMax == -1)
	latencyGeneralMax = td2.total_microseconds();
    if (td1.total_microseconds() < latencyGeneralMin || latencyGeneralMin == -1)
	latencyGeneralMin = td1.total_microseconds();
    if (td2.total_microseconds() < latencyGeneralMin || latencyGeneralMin == -1)
	latencyGeneralMin = td2.total_microseconds();

    nl.push(td1);
    assert(nl.meanInMicroseconds() == td1.total_microseconds());
    assert(nl.minInMicroseconds()  == td1.total_microseconds());
    assert(nl.maxInMicroseconds()  == td1.total_microseconds());
    nl.push(td2);
    assert(nl.meanInMicroseconds() >= td1.total_microseconds());
    assert(nl.meanInMicroseconds() <= td2.total_microseconds());
    assert(nl.minInMicroseconds()  == td1.total_microseconds());
    assert(nl.maxInMicroseconds()  == td2.total_microseconds());

    std::cout << "  Mean: " << nl.meanInMicroseconds() << "\n";
    std::cout << "  Count: " << nl.count() << "\n";
    assert(nl.count() == 2);

    std::string str = nl.toString();
    NodeLatency nl2(str);
    assert(nl.toString() == nl2.toString());

    std::cout << "  String: " << str << "\n";
}

void testLatency()
{
    NodeLatency nl1;
    NodeLatency nl2;

    makeNodeLatency(nl1);
    makeNodeLatency(nl2);

    GeneralLatency gl;

    gl.push(nl1);
    gl.push(nl2);

    std::cout << "Mean of means: " << gl.meanOfMeansInMicroseconds() << "\n";
    std::cout << "Min of means: " << gl.minMeanInMicroseconds() << "\n";
    std::cout << "Max of means: " << gl.maxMeanInMicroseconds() << "\n";
    std::cout << "General max: " << gl.generalMaxInMicroseconds() << "\n";
    std::cout << "General min: " << gl.generalMinInMicroseconds() << "\n";
    std::cout << "Count: " << gl.count() << "\n";

    assert(latencyGeneralMin == gl.generalMinInMicroseconds());
    assert(latencyGeneralMax == gl.generalMaxInMicroseconds());
    assert(gl.count() == 4);

    // Test empty stuff:
    NodeLatency emptyNl;
    NodeLatency emptyNl2(emptyNl.toString());
    GeneralLatency emptyGl;
    emptyGl.push(emptyNl2);

    assert(emptyNl.maxInMicroseconds() == 0);
    assert(emptyNl.minInMicroseconds() == 0);

    //assert(emptyGl.meanOfMeansInMicroseconds() == 0.0);
    //assert(emptyGl.minMeanInMicroseconds() == 0.0);
    //assert(emptyGl.maxMeanInMicroseconds() == 0.0);
    assert(emptyGl.generalMaxInMicroseconds() == 0);
    assert(emptyGl.generalMinInMicroseconds() == 0);
}

void testSuccessRate()
{
    int i;

    NodeSuccessRate nsr;

    for(i = 0; i < 10; i++)
	nsr.hit();
    for(i = 0; i < 9; i++)
	nsr.miss();

    std::cout << "Success rate: " << nsr.successRate() << "\n";
    std::cout << "toString: " << nsr.toString() << "\n";

    NodeSuccessRate nsr2(nsr.toString());
    std::cout << "Success rate: " << nsr2.successRate() << "\n";

    assert(nsr.toString() == nsr2.toString());

    GeneralSuccessRate gsr;
    gsr.push(nsr);
    gsr.push(nsr2);

    std::cout << "General min: " << gsr.min() << "\n";
    std::cout << "General max: " << gsr.max() << "\n";
    std::cout << "General mean: " << gsr.mean() << "\n";

    // Test empty stuff:
    NodeSuccessRate emptyNsr;
    NodeSuccessRate emptyNsr2(emptyNsr.toString());
    GeneralSuccessRate emptyGsr;
    emptyGsr.push(emptyNsr2);
    assert(emptyGsr.min() == 0);
    assert(emptyGsr.max() == 0);
    assert(emptyGsr.mean() == 0);
}

int main() {

    testLatency();
    testSuccessRate();

    std::cout << "Success!\n";

    return 0;
}
