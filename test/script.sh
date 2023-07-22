#!/bin/bash
# Leer la dirección IP pasada como argumento
ip=$1
test=$2
loop=$3

# Hacer algo con la dirección IP
echo "Dirección IP: $ip"
echo "Test: $test"
echo "Loopback: $loop"
echo $ip > values.txt
# Escribir la dirección IP y el test en un archivo de texto
echo $ip > ip.txt
echo $test >> ip.txt
# Ruta del archivo
archivo="../ADALM-PLUTO-SDR/configuration/debug_attr.txt"

# Reemplaza "loop" con el valor de la variable $loop en el archivo
sed -Ei 's/(loopback )[0-9]+/\1'"$loop"'/' "$archivo"

# Resto del script...
cd ../ADALM-PLUTO-SDR/run
./TX_RX.sh
cd ../../test
# #pwd

if [ "$test" = "1" ]; then
    #echo "sdr_test1"
    /usr/bin/python3 sdr_test.py
elif [ "$test" = "2" ]; then
    #echo "sdr_test2"
    /usr/bin/python3 sdr_test2.py
else
    echo "ERROR script.SH"
    exit 1
fi
echo "Recargar la pagina"
exit 1