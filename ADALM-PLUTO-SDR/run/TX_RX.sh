#!/bin/bash
ip_address=$(head -n 1 ../../test/ip.txt)
test=$(sed -n '2p' ../../test/ip.txt)
echo "====================================================="
echo "                | MAKEFILE TX RX "$ip_address"|      "
echo "                ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯       "

cd ../sources
make clean
if ! make; then
    echo "Make failed. Exiting the script."
    exit 1
fi

# CARPETA DE TRABAJO /proyecto_radar ssh -t root@"$ip_address"

# Realiza un ping a la dirección IP
ping -c 1 "$ip_address" >/dev/null 2>&1
if [ $? -ne 0 ]; then
    echo "Ping fallido. La conexión está siendo rechazada. Saliendo del script. Verificar VPN!"
    exit 1
fi

# sshpass -p "analog" scp ../filters/filter_1.txt root@"$ip_address":/proyecto_radar/
# sshpass -p "analog" scp ../filters/filter_2.txt root@"$ip_address":/proyecto_radar/
# sshpass -p "analog" scp ../filters/filter_3.txt root@"$ip_address":/proyecto_radar/
# sshpass -p "analog" scp ../filters/filter_4.txt root@"$ip_address":/proyecto_radar/


sshpass -p "analog" scp ../configuration/debug_attr.txt root@"$ip_address":/proyecto_radar/
sshpass -p "analog" scp ../configuration/tx_config.txt root@"$ip_address":/proyecto_radar/
sshpass -p "analog" scp ../configuration/rx_config.txt root@"$ip_address":/proyecto_radar/


#sshpass -p "analog" scp ../output/datos.txt root@"$ip_address":/proyecto_radar/

#echo "FIN ENVIO"
# Check the value of second_line and perform actions accordingly
if [ "$test" = "1" ]; then
    echo "Test 1"
    echo -n > ../output/datos.txt
    sshpass -p "analog" scp ../sources/tx_rx root@"$ip_address":/proyecto_radar/
    echo "FINALIZADO DE LA CARGA DEL PROGRAMA"
    echo "EJECUTANDO PROGRAMA..."
    sshpass -p "analog" ssh -t root@"$ip_address" "cd /proyecto_radar/; exec ./tx_rx"
    echo "OBTENCION DE DATOS..."
    sshpass -p "analog" scp  root@"$ip_address":/proyecto_radar/datos.txt ../output
    echo "FIN SIMULACION"
elif [ "$test" = "2" ]; then
    echo "Test 2"
    echo -n > ../output/datos_0.txt
    echo -n > ../output/datos_1.txt
    echo -n > ../output/datos_2.txt
    sshpass -p "analog" scp ../sources/tx_rx_v2 root@"$ip_address":/proyecto_radar/
    echo "FINALIZADO DE LA CARGA DEL PROGRAMA"
    echo "EJECUTANDO PROGRAMA..."
    sshpass -p "analog" ssh -t root@"$ip_address" "cd /proyecto_radar/; exec ./tx_rx_v2"
    echo "OBTENCION DE DATOS. DESCARGANDO ARCHIVOS..."
    sshpass -p "analog" scp  root@"$ip_address":/proyecto_radar/datos_0.txt ../output
    sshpass -p "analog" scp  root@"$ip_address":/proyecto_radar/datos_1.txt ../output
    sshpass -p "analog" scp  root@"$ip_address":/proyecto_radar/datos_2.txt ../output
    
    echo "FIN SIMULACION"
    #sshpass -p "analog" scp  root@"$ip_address":/proyecto_radar/datos.txt ../output
else
    echo "ERROR .SH"
fi





#sshpass -p "analog" ssh -t root@"$ip_address" "cd /proyecto_radar/; exec ./tx_rx"

#chmod +x TX_RX.sh
#ejecutar
#sshpass -p "analog" ssh -t root@192.168.1.31 "cd /proyecto_radar/; exec ./2_receive_pulse"
#buscar
#sshpass -p "analog" scp ../sources/tx_rx root@192.168.1.36:/proyecto_radar/
#sshpass -p "analog" ssh -t root@192.168.1.36  "cd /proyecto_radar/; exec \$SHELL --login"
#sshpass -p "analog" scp  root@192.168.1.31:/proyecto_radar/output_pulse.txt ../new_idea
#sshpass -p "analog" scp ../configuration/debug_attr.txt root@192.168.1.36:/proyecto_radar/