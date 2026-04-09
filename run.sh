#!/bin/bash

make clean

if [[ $1 == "-naive" ]] 
then
  make run TARGET=Release DEFINE=VERSION_NAIVE
elif [[ $1 == "-arrs" ]]
then
  make run TARGET=Release DEFINE=VERSION_ARRAYS
else 
  make run TARGET=Release
fi

