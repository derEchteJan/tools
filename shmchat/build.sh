#!/bin/bash
echo "compiling ShmChatClient.cpp" &&\
g++ ./ShmChat/ShmChatClient.cpp -o shmchat &&\

(cd ./Piper && bash build.sh) &&\
cp ./Piper/piper . &&\
cp ./Piper/pipertest .
