#!/usr/bin/bash

FREQUNCY="2.4GHz"
AFFINITY=3
TURBOSTAT_INTERVAL=0.5
TARGET=build/mandelbrot.out
OUT_DIR=stats

mkdir -p $OUT_DIR

sudo cpupower frequency-set -d $FREQUENCY -u $FREQUENCY
sudo cpupower frequency-set -g performance

benchmark() {
  ./build.sh $1 -bench
  sudo turbostat --quiet --cpu $AFFINITY --show CPU,frequency,CoreThr,CoreTmp,Busy% --interval $TURBOSTAT_INTERVAL --Summary --out $OUT_DIR/bench_$2.stat & disown
  TURBOSTAT_PID=$!
  taskset --cpu-list $AFFINITY ./$TARGET "$OUT_DIR/bench_$2.clk"
  kill $TURBOSTAT_PID
}

benchmark "-naive" "naive"
# benchmark "-arrs" "arrs"
# benchmark "" "intrinsic"

sudo cpupower frequency-set -d 500MHz
sudo cpupower frequency-set -u 3GHz
sudo cpupower frequency-set -g performance
