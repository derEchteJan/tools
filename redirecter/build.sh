#!/bin/bash
# install.sh
# builds the docker container for this application

./stop.sh

sudo docker rm redirecter
sudo docker build -t redirecter .
