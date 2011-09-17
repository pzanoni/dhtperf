#!/bin/bash
#
# dhtperf: DHT performance evaluation tool
# Copyright (C) 2011 Paulo Zanoni <przanoni@gmail.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

# vim:textwidth=0

if [ -z "$TEST_NAME" ]; then
    TEST_NAME=$(date +%Y-%m-%d-%H%M)
    echo "Test name not set. Using default value: $TEST_NAME"
fi

echo "Changing dir to $TEST_NAME"
mkdir -p $TEST_NAME
cd $TEST_NAME

if [ -z "$WORKLOAD" ]; then
    WORKLOAD=successrate
    echo "Workload not set. Using default value: $WORKLOAD"
fi
if [ -z "$DHTS" ]; then
    DHTS="openchord owshellchord owshellkad owshellpastry pastconsole"
    echo "DHTs not set. Using default value: $DHTS"
fi
if [ ! -f "$NODES_FILE" ]; then
    if [ -f "../today.nodes" ]; then
	NODES_FILE="../today.nodes"
	echo "Nodes file not set, using default value: $NODES_FILE"
    else
	echo 'Nodes file not set. Specify with $NODES_FILE'
	exit 1
    fi
fi
if [ ! -f "$DHTPERFD_CLIENT" ]; then
    if [ -f "../dhtperfd-client" ]; then
	DHTPERFD_CLIENT="../dhtperfd-client"
	echo "dhtperfd-client not set, but found: $DHTPERFD_CLIENT"
    else
	echo 'dhtperfd-client not set. Specify with $DHTPERFD_CLIENT'
	exit 1
    fi
fi

touch $WORKLOAD

SLICE_NAME=ufpr_dht
WORKLOAD_DIR=/usr/share/dhtperf/workloads

NODES=$(grep "^y:" ${NODES_FILE} | cut -d':' -f2)
NODES_COUNT=$(grep "^y:" ${NODES_FILE} | cut -d':' -f2 | wc -l)
CONTROLLERS=$((NODES_COUNT - 1))
USED_CONTROLLERS=$((CONTROLLERS * 9 / 10))
#USED_CONTROLLERS=$((CONTROLLERS - 0))

for dht in $DHTS; do

    echo "Running test for dht ${dht}"

    MASTER_STARTED=0
    for node in $NODES; do
	if [ "$MASTER_STARTED" = "0" ]; then
	    echo "Starting master: ${node}"
	    master=${node}
	    ssh ${SLICE_NAME}@${node} "mkdir -p ${TEST_NAME}; cd ${TEST_NAME}; dhtperf-master -w ${WORKLOAD_DIR}/${WORKLOAD} -n ${USED_CONTROLLERS} -d ${dht} > master.${dht}.log 2>&1" &
	    MASTER_PID=$!
	    MASTER_STARTED=1
	    echo $master > master
	else
	    echo "Starting controller: ${node}"
	    $DHTPERFD_CLIENT $node start $master &
	    sleep 0.4
	fi
    done

    echo "Waiting master $MASTER_PID"
    wait $MASTER_PID
    #wait

    echo "All nodes finished"

    sleep 5
    scp ${SLICE_NAME}@${master}:${TEST_NAME}/*.csv .

    echo "Waiting 30 seconds"
    sleep 30

    echo "Killing processes on all nodes"
    for node in $NODES; do
	echo "Stopping controller: ${node}"
	$DHTPERFD_CLIENT $node stop &
	sleep 0.4
    done

    echo "Waiting 1 minute for the next test"
    sleep 60

done

echo "Generating plots"
set -e
for file in latency.gnuplot cdf.gnuplot; do
    cp  ../../plots/$file .
    gnuplot $file
done

echo "Done"
