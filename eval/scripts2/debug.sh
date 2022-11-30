#!/usr/bin/env bash

ENABLER_NAME="debug" # a easy to remember name

SUFFIX="-debug"
if [ "$#" -ge 1 ]; then
	SUFFIX="-$1"
fi

source ./init.sh

COMMAND="wget https://speed.hetzner.de/1GB.bin -O /dev/null"

PERF_ITEMS="dtlb_load_misses.walk_completed,dtlb_load_misses.walk_pending,dtlb_store_misses.walk_completed,dtlb_store_misses.walk_pending,itlb_misses.walk_completed,itlb_misses.walk_pending,ept.walk_pending,cycles,cpu-cycles"
PERF_INTERVAL=5
PT_MULTIPLIER=1

enable_trace

source ./common.sh
