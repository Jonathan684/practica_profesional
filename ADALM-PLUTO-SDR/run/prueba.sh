#!/bin/bash0
ip_address=$(head -n 1 ../../app/ip.txt)
#echo "IP Address: $ip_address"
# Read the second line into another variable
second_line=$(sed -n '2p' ../../app/ip.txt)
#echo "Second Line: $second_line"
