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
    echo 'Test name not set. Specify with $TEST_NAME'
    exit 1
fi
if [ -z "$WORKLOAD" ]; then
    echo 'Workload not set. Specify with $WORKLOAD'
    exit 1
fi
if [ -z "$DHTS" ]; then
    echo 'DHTs not set. Specify with $DHTS'
    exit 1
fi
if [ ! -f "$NODES_FILE" ]; then
    echo 'Nodes file not set. Specify with $NODES_FILE'
    exit 1
fi

SLICE_NAME=ufpr_dht
WORKLOAD_DIR=/usr/share/dhtperf/workloads

NODES=$(grep "^y:" ${NODES_FILE} | cut -d':' -f2)
NODES_COUNT=$(grep "^y:" ${NODES_FILE} | cut -d':' -f2 | wc -l)
CONTROLLERS=$((NODES_COUNT - 1))
#USED_CONTROLLERS=$((CONTROLLERS - 10))
USED_CONTROLLERS=$((CONTROLLERS - 0))

for dht in $DHTS; do

    echo "Running test for dht ${dht}"

    MASTER_STARTED=0
    for node in $NODES; do
	if [ "$MASTER_STARTED" = "0" ]; then
	    echo "Starting master: ${node}"
	    master=${node}
	    ssh ${SLICE_NAME}@${node} "mkdir -p ${TEST_NAME}; cd ${TEST_NAME}; dhtperf-master -w ${WORKLOAD_DIR}/${WORKLOAD} -n ${USED_CONTROLLERS} -d ${dht} > master.log 2>&1" &
	    #ssh ${SLICE_NAME}@${node} "mkdir -p ${TEST_NAME}; cd ${TEST_NAME}; dhtperf-master -w ${WORKLOAD_DIR}/${WORKLOAD} -n ${USED_CONTROLLERS} -d ${dht} &" > ${node}.${dht}.log 2>&1 &
	    #gnome-terminal -x ssh ${SLICE_NAME}@${node} "mkdir -p ${TEST_NAME}; cd ${TEST_NAME}; dhtperf-master -w ${WORKLOAD_DIR}/${WORKLOAD} -n ${CONTROLLERS} -d ${dht}" > ${node}.${dht}.log 2>&1 &
	    MASTER_PID=$!
	    MASTER_STARTED=1
	else
	    echo "Starting controller: ${node}"
	    #ssh ${SLICE_NAME}@${node} "sleep 60; nohup dhtperf-controller -o ${master} > controller.log 2>&1 &" > ${node}.${dht}.log 2>&1 &
	    #gnome-terminal -x ssh ${SLICE_NAME}@${node} "sleep 60; dhtperf-controller -o ${master}" > ${node}.${dht}.log 2>&1 &
	    #gnome-terminal -x ssh ${SLICE_NAME}@${node} "sleep 30; dhtperf-controller -o ${master}" \; echo "$node" \; sleep 24h &
	    guake -n $(pwd) -e "ssh ${SLICE_NAME}@${node} sleep 30\; dhtperf-controller -o ${master}" &
	    sleep 0.3
	fi
    done

    wait $MASTER_PID
    #wait

    echo "All nodes finished"

    sleep 5
    scp ${SLICE_NAME}@${master}:${TEST_NAME}/*.csv .
    echo "========="
    sleep 60

    echo "Killing processes on all nodes"
    for node in $NODES; do
	ssh ${SLICE_NAME}@${node} kill -9 -1 &
    done
    wait
    sleep 60

done
