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

flag='y'

while (( $# >= 1 )); do
    case $1 in
    start)
	do_start=1
	master_node=$2
	shift 2
	;;
    stop)
	do_stop=1
	shift
	;;
    ping)
	do_ping=1
	shift
	;;
    update)
	do_update=1
	shift
	;;
    version)
	do_version=1
	shift
	;;
    panic)
	do_panic=1
	shift
	;;
    *)
	echo "Unrecognized option: $1"
	exit 1
	;;
    esac
done


if [ ! -f "$NODES_FILE" ]; then
    if [ -f nodes ]; then
	NODES_FILE=nodes
    else
	echo 'Nodes file not set. Specify with $NODES_FILE'
	exit 1
    fi
fi

NODES=$(grep "^${flag}:" ${NODES_FILE} | cut -d':' -f2)


for node in $NODES; do
    if (($do_start)); then
	echo "starting $node"
	./dhtperfd-client $node start $master_node
    elif (($do_stop)); then
	echo "stopping $node"
	./dhtperfd-client $node stop
    elif (($do_ping)); then
	echo "pinging $node"
	./dhtperfd-client $node ping
    elif (($do_update)); then
	echo "updating $node"
	./dhtperfd-client $node update
    elif (($do_version)); then
	echo "version $node"
	./dhtperfd-client $node version
    elif (($do_panic)); then
	./dhtperfd-client $node stop &
	sleep 0.3
    fi
done

wait
