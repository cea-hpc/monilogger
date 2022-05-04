#!/bin/sh

mkdir build
cmake -Bbuild -Ssrc
cmake --build build
