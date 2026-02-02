#!/bin/bash
# run.sh
# starts the docker container for this application

source client.conf

# MARK JAN: port conflict on client: port is not needed by container, reserverd by bedrock_server already.. fix

./stop.sh

echo "starting redirecter.."
existing_id=$( sudo docker ps -a -q --filter="name=redirecter" )
if [[ $existing_id == "" ]]
then
    # configure as service
    echo "-> configuring new service"

    existing_network=$( docker network list --filter="name=ip6net" | grep ip6net )
    if [[ $existing_network == "" ]]
    then
        echo "setting up ip6net network"
        docker network create --ipv6 ip6net
    fi

    sudo docker run -d -p $PORT:$PORT/udp --network ip6net --name 'redirecter' --restart unless-stopped 'redirecter'
else
    # regular restart
    echo "-> restarting container"
    sudo docker start 'redirecter'
fi

sleep 1
sudo docker ps -a
