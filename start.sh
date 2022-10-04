TRACE_DIR=/usr/src/dynamorio-trace/eval/results/2022-10-04-1664899431-debug

OUTPUT_FILE="$TRACE_DIR/sim.log"

TRACE=$(find $TRACE_DIR -maxdepth 1 -name "drmemtrace*" -type d)
/usr/src/dynamorio/build/bin64/drrun -t drcachesim \
                    -indir $TRACE \
                    -pt_dump_file $TRACE_DIR/pt_dump \
					-vt_pt_dump_file $TRACE_DIR/../host.pt_dump  \
                    -warmup_refs     300000000                   \
                    -TLB_L1I_entries 64                          \
                    -TLB_L1I_assoc   4                           \
                    -TLB_L1D_entries 64                          \
                    -TLB_L1D_assoc   8                           \
                    -TLB_L2_entries  16384                       \
                    -TLB_L2_assoc   4                            \
                    -L1I_size  $(( 512 * 1024 ))                 \
                    -L1I_assoc 8                                 \
                    -L1D_size  $(( 512 * 1024 ))                 \
                    -L1D_assoc 8                                 \
                    -L2_size   $(( 16 * 1024 * 1024 ))           \
                    -L2_assoc  16                                \
                    -LL_size   $(( 32 * 1024 * 1024 ))           \
                    -LL_assoc  16                                #\
                    #> $OUTPUT_FILE 2>&1 & pid=$! &