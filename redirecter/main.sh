#!/bin/bash
# main.sh

echo "starting main.sh ($1)"


# $1: command name
# $2: package name
install_command()
{
    if ! command -v $1 > /dev/null 2>&1
    then
        echo "missing command '$1'"
        echo "installing package '$2'"
        echo "apt update"
        apt update
        echo "apt install $2 -y"
        apt install $2 -y
        echo "$1 --help"
        $1 --help
    fi
}

# $1: interface name
get_ip6()
{
    ip -6 address show dev $1 | grep "inet6" | head -1 | awk '/inet/ {print $2}' | awk -F'[\\\\/]' ' { print $1 }'
}

# $1: ssh host
# $2: ip6 addr
# $3: port
upload_ip6()
{
    ssh_host=$1

    touch /tmp/server.conf
    echo "IP6=\"$2\"" > /tmp/server.conf
    echo "PORT=\"$3\"" >> /tmp/server.conf

    get_pid='$( sudo docker ps -a -q --filter="name=redirecter" )' # shell subcommand to get pid of container

    ssh -o 'PasswordAuthentication=no' $ssh_host echo "connected"
    if [[ $? != 0 ]]; then echo "ssh: unable to connect to '$ssh_host'"; return 1; fi

    # copy /tmp/server.conf to remote container
    echo "uploading server.conf to $ssh_host:/tmp"
    scp /tmp/server.conf $ssh_host:/tmp/server.conf
    if [[ $? != 0 ]]; then echo "ssh: unable upload server.conf to '$ssh_host'"; return 1; fi

    # copy file into remote container
    container_pid=$( ssh $ssh_host "$get_pid" )
    if [[ $container_pid != "" ]]
    then
        echo "copying uploaded file to '$container_pid'"
        ssh $ssh_host "sudo docker cp /tmp/server.conf redirecter:/app/server.conf && rm /tmp/server.conf"
    else
        echo "no remote container running"
    fi
}


echo "installing dependencies"
install_command ip iproute2


if [[ $1 == "server" ]]
then
    echo "running in server mode"

    while [[ 1 ]]
    do
        source server.conf # load config
        echo "loaded server.conf"
        cat server.conf
        echo ""

        echo "redirecting traffic to: '$IP6'"
        echo "running socat UDP4-LISTEN:$PORT,fork,su=nobody,reuseaddr \"UDP6:[$IP6]:$PORT\""
        socat UDP4-LISTEN:$PORT,fork,su=nobody,reuseaddr "UDP6:[$IP6]:$PORT"
        sleep 5
    done

elif [[ $1 == "client" ]]
then
    echo "running in client mode"
    source client.conf # load config
    echo "loaded client.conf"

    set_addr=""
    addr=$( get_ip6 $IF_NAME )

    while [[ 1 ]]
    do

        if [[ $addr != $set_addr ]]
        then
            upload_ip6 $SSH_HOST $addr
            set_addr=$addr
        fi

        sleep 5

        addr=$( get_ip6 $IF_NAME )

    done

else
    echo "error: specify 'server' or 'client' as launch parameter"
    exit 1
fi
