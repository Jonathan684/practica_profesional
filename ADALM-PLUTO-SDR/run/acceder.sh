#!/bin/bash
ip_address=$(cat ../../test/ip.txt)
sshpass -p "analog" ssh -t root@"$ip_address"  "cd /proyecto_radar/; exec \$SHELL --login"
# mkdir proyecto_radar