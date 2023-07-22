#!/bin/bash
ip_address=$(head -n 1 ../../test/ip.txt)

echo "====================================================="
echo "                | CARGANDO FILTROS "$ip_address"|    "
echo "                ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯    "
echo "====================================================="

# CARPETA DE TRABAJO /proyecto_radar ssh -t root@"$ip_address"

ping -c 1 "$ip_address" >/dev/null 2>&1
if [ $? -ne 0 ]; then
    echo "Ping fallido. La conexión está siendo rechazada. Saliendo del script."
    exit 1
fi

sshpass -p "analog" scp ../filters/filter_1.txt root@"$ip_address":/proyecto_radar/
sshpass -p "analog" scp ../filters/filter_2.txt root@"$ip_address":/proyecto_radar/
sshpass -p "analog" scp ../filters/filter_3.txt root@"$ip_address":/proyecto_radar/
sshpass -p "analog" scp ../filters/filter_4.txt root@"$ip_address":/proyecto_radar/
