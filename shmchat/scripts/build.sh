#!/bin/bash
mkdir -p build_linux &&\

echo "compiling ShmChatClient.cpp" &&\
g++ ./Client/ShmChatClient.cpp -o build_linux/shmchat &&\
chmod 777 build_linux/shmchat &&\

echo compiling piper.cpp &&\
g++ ./Piper/piper.cpp -o build_linux/piper &&\
chmod 777 build_linux/piper &&\

echo compiling pipertest.cpp &&\
g++ ./Piper/pipertest.cpp -o build_linux/pipertest &&\
chmod 777 build_linux/pipertest
