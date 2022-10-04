#!/usr/bin/env bash

# file locator
SOURCE="${BASH_SOURCE[0]:-$0}";
while [ -L "$SOURCE" ]; do # resolve $SOURCE until the file is no longer a symlink
        DIR="$( cd -P "$( dirname -- "$SOURCE"; )" &> /dev/null && pwd 2> /dev/null; )";
        SOURCE="$( readlink -- "$SOURCE"; )";
        [[ $SOURCE != /* ]] && SOURCE="${DIR}/${SOURCE}"; # if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
done
SCRIPT_DIR="$( cd -P "$( dirname -- "$SOURCE"; )" &> /dev/null && pwd 2> /dev/null; )";

cd "$SCRIPT_DIR"

SUFFIX=""

if [ "$#" -ge 1 ]; then
	SUFFIX="-$1"
fi

# 2022-12-31-512345678 (POSIX Timestamp)
TS=`date '+%F-%s'`
dir="results/$TS$SUFFIX/"

mkdir -p "$dir"

mv perf.log "$dir"
mv run.log "$dir"
mv /usr/src/ycsb-0.17.0/results/* "$dir"

sync

echo "Result saved to: $dir"
ls "$dir"
