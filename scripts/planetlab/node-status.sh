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

#if [ "`id -u`" != 0 ]; then
#    echo "You're not root"
#    exit 1
#fi

if [ ! -f "$NODES_FILE" ]; then
    if [ -f nodes ]; then
        NODES_FILE=nodes
    else
        echo 'Nodes file not set. Specify with $NODES_FILE'
        exit 1
    fi
fi

flag=y

NODES=$(grep "^${flag}:" ${NODES_FILE} | cut -d':' -f2)

OUTPUT=today.nodes
echo -n > $OUTPUT

SLICE_NAME=ufpr_dht

SSH="ssh -o ConnectTimeout=3"

for node in $NODES; do
    if sudo ping -q -c 20 -w 3 -f $node > /dev/null; then
	VERSION=`$SSH ${SLICE_NAME}@${node} "rpm -q dhtperf"`
	if [[ "$VERSION" =~ dhtperf.*fc8 ]]; then
	    echo "y:${node}:${VERSION}" >> $OUTPUT
	    echo "y:${node}:${VERSION}"
	else
	    echo " :${node}:wrong-version" >> $OUTPUT
	    echo " :${node}:wrong-version"
	fi
    else
	echo " :${node}:ping-fail" >> $OUTPUT
	echo " :${node}:ping-fail"
    fi
done
