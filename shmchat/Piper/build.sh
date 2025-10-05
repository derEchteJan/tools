#!/bin/bash

echo compiling piper.cpp &&\
g++ piper.cpp -o piper &&\

echo compiling pipertest.cpp &&\
g++ pipertest.cpp -o pipertest

chmod 777 piper
chmod 777 pipertest