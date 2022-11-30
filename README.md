This is a trace generator for the simulator used in the "Prefetched Address Translation" paper. The trace generator records (1) an instruction trace, and (2) a page table dump at the end of the trace generation. Both the trace and the page table dump are later consumed by the simulator. Main changes are in ./clients/drcachesim/tracer/tracer.cpp. 

1. Set up environment variables
```bash
source source.sh
```

2. Install/Build the tracer
```bash
./install.sh
```

__FOR SIMULATOR__

3. Run the simulator 
Example:
```bash
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

# SETTTINGS
TRACE_DIR=/disk/local/traces/mcf/
OUTPUT_FILE=./mcf.res

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

TRACE=$(find $TRACE_DIR -maxdepth 1 -name "drmemtrace*" -type d)
$SIMULATOR_DIR/build/bin64/drrun -t drcachesim \
                    -indir $TRACE \
                    -pt_dump_file $TRACE_DIR/pt_dump \
                    -warmup_refs     300000000                   \
                    -TLB_L1I_entries 64                          \
                    -TLB_L1I_assoc   8                           \
                    -TLB_L1D_entries 64                          \
                    -TLB_L1D_assoc   8                           \
                    -TLB_L2_entries  1024                        \
                    -TLB_L1D_assoc   8                           \
                    -L1I_size  $(( 32 * 1024 ))                  \
                    -L1I_assoc 8                                 \
                    -L1D_size  $(( 32 * 1024 ))                  \
                    -L1D_assoc 8                                 \
                    -L2_size   $(( 256 * 1024 ))                 \
                    -L2_assoc  8                                 \
                    -LL_size   $(( 16 * 1024 * 1024 ))           \
                    -LL_assoc  16                                \
                    > $OUTPUT_FILE 2>&1 & pid=$! &
```

__FOR TRACER__

3. Prepare the system: disable THP, enable page table dump module
```bash
sudo ./prepare_system.sh
```

4. Generate trace
```bash
  # MAIN SETTINGS SECTION
  # Example:
  OUTPUT_DIR=/disk/local/traces/<studied_application.tracedir>
  APPLICATION="<application> <application_params>"
  
  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#
  
  ENABLE_FILE=/tmp/$RANDOM
  echo 0 > $ENABLE_FILE #trace generation will start recording a trace when there will be 1 in this file
  
  $TRACER_DIR/generate_trace.sh --output_dir=$OUTPUT_DIR --enabler_file=$ENABLE_FILE $APPLICATION 2>&1 & pid=$!
  # wait for application to warm up
  sleep 20 
  
  #start trace generation once the application is warmed up 
  echo 1 > $ENABLE_FILE 
  wait $pid
  
  # postprocess page table dump:
  echo -n "Postprocess page table dump file to prepare it for the simulator..."
  cat $OUTPUT_DIR/pt_dump_raw | $TRACER_DIR/parse_va_from_dump.py > $OUTPUT_DIR/pt_dump
  # add number of lines on top of the file to simlify reading
  sed -i "1i $(wc -l $OUTPUT_DIR/pt_dump | awk '{print $1}')" $OUTPUT_DIR/pt_dump
  echo "Done"
```

