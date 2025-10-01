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
    sudo docker run -d -p 127.0.0.1:$PORT:$PORT --name 'redirecter' --restart unless-stopped 'redirecter'
else
    # regular restart
    echo "-> restarting container"
    sudo docker start 'redirecter'
fi

sleep 1
sudo docker ps -a
