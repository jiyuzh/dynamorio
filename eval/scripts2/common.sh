# this is not a full script, use it with source

TRACER_DIR="/usr/src/dynamorio"
PERF_DIR="/usr/src/vanilla-5.10.123/tools/perf"

cd "$SCRIPT_DIR"

kill_descendant_processes() {
	local pid="$1"
	local and_self="${2:-false}"
	if children="$(pgrep -P "$pid")"; then
		for child in $children; do
			kill_descendant_processes "$child" true
		done
	fi
	if [[ "$and_self" == true ]]; then
		echo "Killing $pid"
		kill -INT "$pid"
	fi
}

sigint()
{
	echo "signal INT received, script ending"
	kill_descendant_processes $$
	exit 0
}
trap sigint SIGINT



cd "$TRACER_DIR"

sudo ./prepare_system.sh

"$TRACER_DIR/generate_trace.sh" "--output_dir=$OUTPUT_DIR" "--enabler_file=$ENABLE_FILE" $COMMAND 2>&1 & pid=$!



cd "$SCRIPT_DIR"

echo "Started, PID = $pid"

count=0

while true; do

		checker=`cat /proc/$pid/comm 2>/dev/null`
		if [ "$checker" != "generate_trace." ]; then
			break
		fi

		pid2=`cat $ENABLE_FILE`
		if [ "$pid2" != "1" ] && [ "$pid2" != "0" ]; then
			date -Iseconds >> "$OUTPUT_DIR/perf.log"
			"$PERF_DIR/perf" stat -e "$PERF_ITEMS" -p "$pid2" sleep "$PERF_INTERVAL" >> "$OUTPUT_DIR/perf.log" 2>&1

			if [ $count -ge $PT_MULTIPLIER ]; then
				echo "$pid2" | sudo tee /proc/page_tables
				sudo cat /proc/page_tables > "$OUTPUT_DIR/pt_dump_raw"
				count=0
			fi
		else
			sleep "$PERF_INTERVAL"
		fi

		count=`expr $count + 1`
done

sync

echo "Done"
