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

set -x
set -e

NODE=$1

SLICE_NAME=ufpr_dht
REPO_FILE=dhtperf.repo

if [ ! -f "$REPO_FILE" ]; then
    echo "$REPO_FILE not found"
    exit 1
fi

scp $REPO_FILE ${SLICE_NAME}@${NODE}:
ssh ${SLICE_NAME}@${NODE} sudo cp ${REPO_FILE} /etc/yum.repos.d
ssh ${SLICE_NAME}@${NODE} sudo yum update -y --nogpgcheck
ssh ${SLICE_NAME}@${NODE} sudo yum install -y dhtperf --nogpgcheck
echo "${NODE} bootstrap finished"
sleep 24h
