#!/bin/bash
sudo cp ./build_linux/shmchat /usr/bin &&\
sudo chmod 777 /usr/bin/shmchat &&\
echo "installed shmchat to /usr/bin" &&\

sudo cp ./build_linux/piper /usr/bin &&\
sudo chmod 777 /usr/bin/piper &&\
echo "installed piper to /usr/bin"
