#!/bin/bash
cd ../sources
make clean 
make
cd ../run
sshpass -p "analog" scp ../sources/tx_rx root@192.168.1.36:/proyecto_radar/
sshpass -p "analog" ssh -t root@192.168.1.36  "cd /proyecto_radar/; exec \$SHELL --login"