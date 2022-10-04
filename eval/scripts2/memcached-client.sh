cd /usr/src/ycsb-0.17.0
mkdir -p results

./bin/ycsb load memcached -s -P "workloads/w100m" -p "memcached.hosts=127.0.0.1" 2>&1

ENABLER_NAME="memcached-server"
ENABLE_FILE="/tmp/enablement/$ENABLER_NAME"
echo 1 > "$ENABLE_FILE"

./bin/ycsb run memcached -s -P "workloads/w100m" -p "memcached.hosts=127.0.0.1" 2>&1
