#!/bin/bash
# installs bedrock_serve with piper as systemd service

sfile="/etc/systemd/system/bedrock.service"

sudo touch $sfile
sudo chmod 666 $sfile
sudo cat ./systemd-service.txt > $sfile
echo installed:
cat $sfile
echo ""

sudo systemctl daemon-reload
sleep 1
sudo systemctl enable bedrock
sleep 1
sudo systemctl restart bedrock
sleep 1
sudo systemctl status bedrock