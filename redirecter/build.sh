#!/bin/bash
# install.sh
# builds the docker container for this application

if [[ $1 != "server" && $1 != "client" ]]
then
    echo "build.sh - error: specify 'server' or 'client' as launch parameter"
    exit 1
fi

# stop existing instances
./stop.sh

echo "uninstalling container redirecter"
sudo docker rm redirecter

if [[ $1 == "server" ]]
then
    source client.conf

    if [[ $PORT == "" ]]; then echo "error: specify 'PORT' in client.conf"; exit 1; fi

    sudo docker build -f Dockerfile_server -t redirecter .

elif [[ $1 == "client" ]]
then

    #sudo docker build -t redirecter .
    ./install-service.sh

else
    echo "build.sh - error: specify 'server' or 'client' as launch parameter"
    exit 1
fi

