#!/bin/sh
ld -r -b binary -o lib/all.res.o lib/all.res
g++ main.cpp -o testris -lGL -lGLEW -lglfw all.res.o
g++ -g main.cpp -o testris_dbg -lGL -lGLEW -lglfw all.res.o

