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

set -e
set -x

if grep -q "Ubuntu 10.10" /etc/issue.net; then
    # basesystem
    #apt-get install language-{pack,support}-{en,pt} vim
    # dhtperf
    apt-get install build-essential cmake libboost-all-dev git
    # maidsafe
    apt-get install libgtest-dev libprotobuf-dev protobuf-compiler subversion
    # openchord/overlayweaver
    apt-get install openjdk-6-jdk ant unzip wget
fi

# dhtperf
git clone https://git.gitorious.org/dhtperf/dhtperf.git
mkdir dhtperf/src/build
cd dhtperf/src/build
cmake ..
make
cd ../../..

# maidsafe
svn checkout http://maidsafe-dht.googlecode.com/svn/trunk/ maidsafe-dht-read-only
cd maidsafe-dht-read-only/
patch -p0 < ../dhtperf/patches/maidsafe-dht.patch
cd build/Linux/Release
cmake ../../.. -G"CodeBlocks - Unix Makefiles"
make
cd ../../../../

# openchord
mkdir openchord
cd openchord
wget 'http://ufpr.dl.sourceforge.net/project/open-chord/Open%20Chord%201.0/1.0.5/open-chord_1.0.5.zip'
unzip open-chord_1.0.5.zip
patch -p1 < ../dhtperf/patches/openchord.patch
ant
ant compile
cd ..

# overlayweaver
mkdir overlayweaver
cd overlayweaver
wget 'http://tenet.dl.sourceforge.net/project/overlayweaver/overlayweaver/0.10.2/overlayweaver-0.10.2.tar.gz'
tar xvzf overlayweaver-0.10.2.tar.gz
cd overlayweaver
ant
ant compile
cd ../..
