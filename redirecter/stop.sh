#!/bin/bash
# stop.sh
# stops the docker container service

echo "stopping all 'redirecter' containers"
pids=$( sudo docker ps | grep "redirecter" | cut -d' ' -f1 )
for pid in $pids
do
    echo "stopping container '$pid'"
    sudo docker stop $pid
done
