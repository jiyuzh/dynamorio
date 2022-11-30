export SIMULATOR_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )" 
alias build_simulator='. $SIMULATOR_DIR/build.sh'
alias clean_simulator='. $SIMULATOR_DIR/clean.sh'
alias generate_trace='. $SIMULATOR_DIR/generate_trace.sh'
function run_simulator() { $SIMULATOR_DIR/build/bin64/drrun "$@"; }
export TRACER_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )" 

function build_tracer()                         { $TRACER_DIR/build.sh; }
function clean_tracer()                         { $TRACER_DIR/clean.sh; }
function generate_trace()                       { $TRACER_DIR/generate_trace.sh; }
function convert_raw_pt_to_simulator_readable() { $TRACER_DIR/parse_va_from_dump.py; }
function run_tracer()                           { $TRACER_DIR/build/bin64/drrun -t drcachesim "$@"; }

