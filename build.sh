#!/bin/sh
cd "$(dirname "$0")"
cd lib
ld -r -b binary -o all_res.o all.res
cd ..
g++ main.cpp -o testris -lGL -lGLEW -lglfw lib/all_res.o
g++ -g main.cpp -o testris_dbg -lGL -lGLEW -lglfw lib/all_res.o
rm lib/all_res.o
