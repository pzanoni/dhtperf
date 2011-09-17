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

#set -x
#set -e

if [ -z "$1" ] ; then
    echo "Usage: $0 workload-file"
    exit 1
fi

WORKLOAD_FILE=$1
if [ ! -f "$WORKLOAD_FILE" ]; then
    echo "$1 is not a regular file"
    exit 1
fi

if [ -z "$OW_HOME" ]; then
    echo "\$OW_HOME is empty. It should point to Overlay Weaver's top-level"
    echo "directory"
    exit 1
fi

#export PATH="$PATH:$(pwd)"
BINARIES="kaddemo_static openchord-console owdhtshell"
BINARIES="$BINARIES dhtperf-master dhtperf-controller"
for binary in $BINARIES; do
    if ! which $binary > /dev/null; then
	echo "$binary not found"
	exit 1
    fi
done

NODES=$(egrep '^nodes[[:space:]][[:digit:]]+$' $1 | cut -d' ' -f2)
if [ -z "$NODES" ]; then
    echo "Can't find the \"nodes\" entry inside $1"
    exit 1
fi
DURATION=$(egrep '^duration[[:space:]][[:digit:]]+$' $1 | cut -d' ' -f2)
if [ -z "$DURATION" ]; then
    echo "Can't find the \"duration\" entry inside $1"
    exit 1
fi

rm -f *.csv

echo "Starting"

if [ -z "$DHTS" ]; then
    DHTS="dummy kaddemo openchord owshellchord owshellkad"
fi
for dht in $DHTS; do
    echo "== $dht"

    LOG_DIR="${dht}.logs"
    rm -rf $LOG_DIR
    mkdir -p $LOG_DIR

    dhtperf-master -d $dht -w $WORKLOAD_FILE > ${LOG_DIR}/master.log 2>&1 &
    MASTER_PID=$!

    # wait for master to start properly
    sleep 5

    for ((i = 0; i < $NODES; i++)); do
	dhtperf-controller > ${LOG_DIR}/controller.${i}.log 2>&1 &
	CONTROLLER_PIDS[$i]=$!
    done

    echo "Sleeping $DURATION seconds"
    sleep $DURATION
    echo "Master should be finishing soon..."

    wait $MASTER_PID
    RET=$?
    if [ "$RET" != "0" ]; then
	echo "Master returned $RET"
	exit 1
    fi

    for ((i = 0; i < $NODES; i++)); do
	wait ${CONTROLLER_PIDS[$i]}
	RET=$?
	if [ "$RET" != "0" ]; then
	    echo "Controller returned $RET"
	    exit 1
	fi
    done

    # give machine a little time to breath
    echo "== done"
    sleep 60
done

echo "Success"
exit 0
