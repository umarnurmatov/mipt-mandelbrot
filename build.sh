TARGET=naive
SRC_DIR=src
BUILD_DIR=build

mkdir -p $BUILD_DIR
gcc -DNDEBUG -march=native -O3 -o $BUILD_DIR/$TARGET.out $SRC_DIR/$TARGET.c -lSDL3
./$BUILD_DIR/$TARGET.out
