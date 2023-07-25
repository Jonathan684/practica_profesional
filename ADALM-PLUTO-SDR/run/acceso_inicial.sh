#!/bin/bash
ip_address=$(head -n 1 ../../app/ip.txt)
ssh -t root@"$ip_address"
# Crear carpeta /jonathan
# mkdir
# Borrar el contenido de  /root/.ssh/known_hosts 
# Si se abrio con vim precionando dd se puede ir borrando el contenido.