#!/bin/bash
ip_address=$(head -n 1 ../../test/ip.txt)
sshpass -p "analog" ssh -t root@"$ip_address"  "cd /proyecto_radar/; exec \$SHELL --login"
# mkdir proyecto_radar