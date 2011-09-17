#!/bin/bash

set -x

ulimit -c unlimited

WORKLOADS="filesharing no-churn high-churn key-spreading successrate"
DHTS="owshellchord owshellkad owshellpastry pastconsole openchord"

cd /home/ppginf/paulo/mestrado/dhtperf-clone/src/build

export PATH="$PATH:$(pwd)"
# Be sure to modify the "owdhtshell" binary!!
#export OW_HOME="$HOME/mestrado/overlayweaver/overlayweaver"

WORKLOAD_DIR="$HOME/mestrado/dhtperf-clone/src/workloads"

NODES=300

#MACHINE_IP="200.17.202.37" # pos1
MACHINE_IP="200.17.202.38" # pos2

TEST_NAME=$(date +%Y-%m-%d-%H%M)
mkdir $TEST_NAME
cd $TEST_NAME

for workload in $WORKLOADS; do
    mkdir $workload
    cd $workload

    for dht in $DHTS; do
	dhtperf-master -l -d $dht -w ${WORKLOAD_DIR}/$workload -n $NODES | tee ${dht}.master.log &
	MASTER_PID=$!

	for i in $(seq $NODES); do
	    dhtperf-controller -o $MACHINE_IP > ${dht}.${i}.log 2>&1 &
	    sleep 0.1
	done

	sleep 20
	echo "($dht $workload)"

	wait $MASTER_PID
	sleep 10
	ps ux > psux-${dht}-${workload}.log
	pkill java
	sleep 50
    done

    #for file in latency.gnuplot cdf.gnuplot; do
	#cp $HOME/mestrado/dhtperf-clone/$file .
	#gnuplot $file
    #done

    cd ..
done
