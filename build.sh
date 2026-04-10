#!/bin/bash

make clean

BENCH_MODE=false
VERSION=""

for arg in "$@"; do
    case "$arg" in
        -bench)
            BENCH_MODE=true
            ;;
        -naive)
            VERSION="VERSION_NAIVE"
            ;;
        -arrs)
            VERSION="VERSION_ARRAYS"
            ;;
    esac
done

if [[ "$BENCH_MODE" == true ]]; then
    if [[ -n "$VERSION" ]]; then
        DEFINE="${VERSION} MODE_BENCHMARK"
    else
        DEFINE="MODE_BENCHMARK"
    fi
else
    if [[ -n "$VERSION" ]]; then
        DEFINE="$VERSION"
    else
        DEFINE=""
    fi
fi

if [[ -n "$DEFINE" ]]; then
    make TARGET=Release DEFINE="$DEFINE"
else
    make TARGET=Release
fi
