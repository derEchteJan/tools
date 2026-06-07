### Redirecter

uses socat to forward UDP v4 traffic to UDP v6 port

enter addres and port into client.conf and server.conf

On the server (reverse proxy) run 'build.sh server'
 -> the socat service is installed and started as a docker image

On the client (target server) run 'build.sh client'
 -> the systemd service is created that updates the ip address on the reverse proxy machine
 - client.conf SSH_HOST needs to point to the reverse proxy machine