#!/bin/bash
ip_address=$(cat ../../test/ip.txt)
ssh -t root@"$ip_address"
# Crear carpeta /jonathan
# mkdir
# Borrar el contenido de  /root/.ssh/known_hosts 
# Si se abrio con vim precionando dd se puede ir borrando el contenido.