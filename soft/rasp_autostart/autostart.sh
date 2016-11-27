#!/bin/bash

function start_triangulator {
    cd ../triangulator/source
    echo "Start triangulator"
    ./server.py -w > output_${RANDOM} &
}

function start_detector {
    cd ../detector/source
    echo "Start detector"
    ./wrapper.py > output_${RANDOM} &
}

########
# main #
########

sleep 10

# start (if true) the triangulator (only for Rasp 0)
if true; then
    (start_triangulator)
fi

(start_detector)

