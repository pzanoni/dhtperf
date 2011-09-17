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

# Opens multiple terminals with SSHs to the selected machines
# Parameters:
#    -b: runs bootstrap-node.sh for each node
#    -c: checks dhtperf version, not launching terminals
#    -f: changes the "flag" used to select nodes on the nodes file
#    -i: ping all nodes, not launching terminal
#    -k: makes all nodes run "kill -9 -1", not launching terminals
#    -p: runs "ps aux" on all nodes, not launching terminals
#    -s: starts dhtperfd on all nodes, not launching terminals
#    -t: runs "true" on each node, not launching terminals
#    -u: does "yum update"
#    -x: bash's "set -x"

flag='y'

while (( $# >= 1 )); do
    case $1 in
    -b)
	do_bootstrap=1
	if [ ! -x ./bootstrap-node.sh ]; then
	    echo "Error: can't run ./bootstrap-node.sh"
	    exit 1
	fi
	shift
	;;
    -c)
	do_check=1
	shift
	;;
    -f)
	flag=$2
	shift 2
	;;
    -i)
	do_ping=1
	shift
	;;
    -k)
	do_kill=1
	shift
	;;
    -p)
	do_psaux=1
	shift
	;;
    -s)
	do_start=1
	shift
	;;
    -stop)
	do_stop=1
	shift
	;;
    -t)
	do_true=1
	shift
	;;
    -u)
	do_update=1
	shift
	;;
    -x)
	set -x
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

SLICE_NAME=ufpr_dht

SSH="ssh -o ConnectTimeout=3"

for node in $NODES; do
    if (($do_check)); then
	set +x
	(v=$($SSH ${SLICE_NAME}@${node} rpm -q dhtperf); echo "${node}: $v") &
    elif (($do_kill)); then
	$SSH ${SLICE_NAME}@${node} sudo kill -9 -1 &
	sleep 0.5
    elif (($do_bootstrap)); then
	#gnome-terminal -x ./bootstrap-node.sh ${node} \; sleep 24h &
	guake -n $(pwd) -e "./bootstrap-node.sh ${node}; sleep 24h" &
	sleep 0.3
    elif (($do_update)); then
	#gnome-terminal -x ssh ${SLICE_NAME}@${node} sudo yum update -y \; echo "${node} update done" \; sleep 24h &
	guake -n $(pwd) -e "ssh ${SLICE_NAME}@${node} sudo yum update -y" &
	sleep 0.5
    elif (($do_psaux)); then
	#(p=$($SSH ${SLICE_NAME}@${node} ps aux); echo "${node}:"; echo $v) &
	echo "${node}:"
	$SSH ${SLICE_NAME}@${node} ps aux
    elif (($do_ping)); then
	if ping -q -c 20 -w 3 -f $node > /dev/null; then
	    echo "$node ok"
	else
	    echo "$node bad"
	fi
    elif (($do_start)); then
	ssh ${SLICE_NAME}@${node} sudo /etc/init.d/dhtperfd start &
	sleep 0.5
    elif (($do_true)); then
	$SSH ${SLICE_NAME}@${node} true
    else
	#gnome-terminal -x ssh ${SLICE_NAME}@${node} &
	guake -n $(pwd) -e "ssh ${SLICE_NAME}@${node}" &
	sleep 0.3
    fi
done

wait
