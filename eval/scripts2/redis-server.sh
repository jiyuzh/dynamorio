#!/usr/bin/env bash

ENABLER_NAME="redis-server" # a easy to remember name

SUFFIX="-redis"
if [ "$#" -ge 1 ]; then
	SUFFIX="-$1"
fi

sudo service redis stop
sudo killall redis-server

source ./init.sh

COMMAND="/usr/bin/redis-server /etc/redis/redis.conf"

PERF_ITEMS="dtlb_load_misses.walk_completed,dtlb_load_misses.walk_pending,dtlb_store_misses.walk_completed,dtlb_store_misses.walk_pending,itlb_misses.walk_completed,itlb_misses.walk_pending,ept.walk_pending,cycles,cpu-cycles"
PERF_INTERVAL=5
PT_MULTIPLIER=1

source ./common.sh
