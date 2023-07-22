#!/bin/bash
echo "#########################################"
echo "#    ACTIVAR LA VPN ANTES DE INICIAR    #"
echo "#########################################"
# Pedir al usuario que ingrese un valor
echo "Ingresar un IP:"
read ip_address
#ip_address=$(head -n 1 ../test/ip.txt)
username="root"
password="analog"
echo "El ip_address ingresado es: $ip_address"

ping -c 1 "$ip_address" >/dev/null 2>&1
if [ $? -ne 0 ]; then
    echo "Ping fallido. La conexión está siendo rechazada. Saliendo del script."
    exit 1
fi

echo $ip_address > ../test/ip.txt
#verificar si existe la carpeta
# Ruta al archivo known_hosts
# Guardar el directorio actual en una variable
current_dir=$(pwd)
cd /root/.ssh

# Verificar si el archivo known_hosts existe antes de eliminar su contenido
if [ -f "known_hosts" ]; then
    # Si el archivo existe, borrar su contenido
    echo "" > "known_hosts"
else
    # Si el archivo no existe, mostrar un mensaje de advertencia
    echo "El archivo known_hosts no existe. No se borrará el contenido."
fi
#echo "" > "known_hosts"

# Regresar al directorio original
cd "$current_dir"
# Primero responde "yes" para la solicitud de confirmación y luego ingresa la contraseña
sshpass -p "$password" ssh -t -o StrictHostKeyChecking=no "$username@$ip_address" 'echo "SSH connection successful"'
if [ $? -eq 0 ]; then
    echo "SSH connection successful. Exiting."
else
    echo "SSH connection failed. Exiting."
    exit 1
fi

# CARPETA DE TRABAJO /proyecto_radar ssh -t root@"$ip_address"
sshpass -p "analog" ssh -t root@"$ip_address" "cd /; mkdir proyecto_radar"


echo "#########################################"
echo "#   | CARGANDO FILTROS "$ip_address" |   #"
echo "#   ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯   #"
echo "#########################################"
sshpass -p "analog" scp ../ADALM-PLUTO-SDR/filters/filter_1.txt root@"$ip_address":/proyecto_radar/
sshpass -p "analog" scp ../ADALM-PLUTO-SDR/filters/filter_2.txt root@"$ip_address":/proyecto_radar/
sshpass -p "analog" scp ../ADALM-PLUTO-SDR/filters/filter_3.txt root@"$ip_address":/proyecto_radar/
sshpass -p "analog" scp ../ADALM-PLUTO-SDR/filters/filter_4.txt root@"$ip_address":/proyecto_radar/
