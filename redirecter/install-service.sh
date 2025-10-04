#!/bin/bash
# installs redirecter as systemd service

sfile="/etc/systemd/system/redirecter13.service"

sudo touch $sfile
sudo chmod 666 $sfile
sudo cat ./systemd-service.txt > $sfile
echo installed:
cat $sfile
echo ""

sudo systemctl daemon-reload
sudo systemctl restart redirecter13
sleep 3
sudo systemctl status redirecter13