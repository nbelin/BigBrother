#!/bin/bash

function start_triangulator {
    cd ../triangulator/source
    echo "Start triangulator"
    ./server.py -w > output_${RANDOM} 2>&1 &
}

function start_detector {
    cd ../detector/source
    echo "Start detector"
    ./wrapper.py > output_${RANDOM} 2>&1 &
}

########
# main #
########

sleep 10

#sudo route add -net 234.0.0.0 netmask 255.0.0.0 eth0
#sleep 5

# start (if true) the triangulator (only for Rasp 0)
if true; then
    (start_triangulator)
fi

(start_detector)

