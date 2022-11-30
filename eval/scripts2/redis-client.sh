cd /usr/src/ycsb-0.17.0
mkdir -p results

./bin/ycsb load redis -s -P "workloads/w100m" -p "redis.host=127.0.0.1" -p "redis.port=6379" 2>&1

ENABLER_NAME="redis-server"
ENABLE_FILE="/tmp/enablement/$ENABLER_NAME"
echo 1 > "$ENABLE_FILE"

./bin/ycsb run redis -s -P "workloads/w100m" -p "redis.host=127.0.0.1" -p "redis.port=6379" 2>&1
