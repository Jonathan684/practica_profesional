#touch salida.txt
#iio_info >> salida.txt
#Buscar el archivo.
#touch salida.txt
#iio_info >> salida.txt
ip_address=$(cat ../../app/ip.txt)
sshpass -p "analog" ssh -t root@"$ip_address" "cd /proyecto_radar/;rm iio_info.txt; touch iio_info.txt ; iio_info >> iio_info.txt"
echo -n > iio_info.txt
sshpass -p "analog" scp  root@"$ip_address":/proyecto_radar/iio_info.txt ../iio_data