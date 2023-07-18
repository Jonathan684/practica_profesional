#include <stdio.h>
#include <stdlib.h>
#include <iio.h>
#include <math.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
//#include <errno>


#define FILTRO_1 "filter_1.txt"
#define FILTRO_2 "filter_2.txt"
#define FILTRO_3 "filter_3.txt"
#define FILTRO_4 "filter_4.txt"

bool stop;

struct iio_context *ctx = NULL;
struct iio_device *main_dev;
struct iio_device *dev_tx;
struct iio_device *dev_rx;
struct iio_channel *tx0_i, *tx0_q;
struct iio_buffer *txbuf;

struct iio_channel *rx0_i, *rx0_q;
struct iio_buffer *rxbuf;

struct iio_channel *chnn_altvoltage1_output;
struct iio_channel *chnn_altvoltage0_output;
struct iio_channel *chnn_device_output;
struct iio_channel *chnn_device_input;

size_t TxBufferSize     = 0;// (pow(2, 12)-1);//150000;// (pow(2, 17)-1);//(pow(2, 14)-1);
size_t RxBufferSize     = 0;//(pow(2, 18)-1);//(pow(2, 18)-1);

/* PULS0*/
int Longitud_del_pulso  = 1;
int PRI  = 0;//(pow(2, 12)-1);  //150000;//(pow(2, 17)-1);   // TxBufferSize   //16383;//4; //Minimo 4 
int16_t amplitud = 0;//0x0FFF ; //4080;  // 0x0FFF;  // Jupyter
int N_rx = 0;//(pow(2, 18)-1);  //RxBufferSize
int rx  = 0;
char value_loopback[3] = "1";




/*
char value_loopback[3] = "0"; // Loopback desactivado
char value_loopback[3] = "1"; // Loopback digital (Necesario para probar la señal)
char value_loopback[3] = "2"; // Loopback RF
*/
void handle_sig(int sig)
{
	printf("Waiting for process to finish... Got signal %d\n", sig);
    iio_buffer_destroy(txbuf);
    iio_channel_disable(tx0_i);
    iio_channel_disable(tx0_q);

    if(rx == true)iio_buffer_destroy(rxbuf);
    if(rx == true)iio_channel_disable(rx0_i);
    if(rx == true)iio_channel_disable(rx0_q);
    iio_context_destroy(ctx);
	//stop = true;
    exit(1);
}

int create_context();
int config_filter();

int config_tx();
int num_lines_tx();
int read_config_tx(char *config[], char *values[]);

int config_rx();
int num_lines_rx();
int read_config_rx(char *config[], char *values[]);
int read_attr_txt();

int main(){

    stop = false;
    signal(SIGINT, handle_sig);
    ssize_t nbytes_tx;
	char *p_dat, *p_end;
	ptrdiff_t p_inc;
    if (create_context()<0)return 0;
    read_attr_txt();
    //printf("N_rx %d\n",N_rx);
    //printf("RxBufferSize %d\n",RxBufferSize);
    //printf("TxBufferSize %d\n",TxBufferSize);
    //printf("PRI %d\n",PRI);
    // printf("rx %d\n",rx);
    
    // printf("amplitud %d\n",amplitud);
    if(config_filter()<0){
       iio_context_destroy(ctx);
       return 0;
    }
    config_tx();
    if(rx == true)config_rx();
    
    /*TRANSMITIR*/
    nbytes_tx = iio_buffer_push(txbuf);// solo es valida para buffer de salida.
    if (nbytes_tx < 0) printf("Error pushing buf %d\n", (int) nbytes_tx); 
    
    p_inc = iio_buffer_step(txbuf);
	p_end = iio_buffer_end(txbuf);
    int count = 0;
    
    for (p_dat = (char *)iio_buffer_first(txbuf, tx0_i); p_dat < p_end; p_dat += p_inc) {
       if ( (count >= 0) && (count < Longitud_del_pulso)  ) {
            ((int16_t*)p_dat)[0] = amplitud;//(1*pow(2, 14));//signal[increm];
            ((int16_t*)p_dat)[1] = amplitud;//(1*pow(2, 14));//signal[increm];
        }
        else if ( (count >= Longitud_del_pulso ) && (count < PRI ) ) {
            ((int16_t*)p_dat)[0] =((int16_t)0x0000);
            ((int16_t*)p_dat)[1] =((int16_t)0x0000);
        }
        count ++;
        if (count == PRI) count = 0;
    }
    /*RECIBIR*/
    if(rx == 1){
            
            int I_rx = 0;
            double signal_i[N_rx];
            double signal_q[N_rx];
            iio_buffer_refill(rxbuf);
            p_inc = iio_buffer_step(rxbuf);
            p_end = iio_buffer_end(rxbuf);
          
            for (p_dat = (char *)iio_buffer_first(rxbuf, rx0_i); p_dat < p_end; p_dat += p_inc) {     
                
                if(I_rx < N_rx){
                    signal_q[I_rx] = (double) (((int16_t*)p_dat)[0]>>4);// / amplitud); //i real
                    signal_i[I_rx] = (double) (((int16_t*)p_dat)[1]>>4);/// amplitud); //q ima
                }
                I_rx ++;
                if(I_rx == N_rx)I_rx = 0;
            }
            
            FILE* archivo = fopen("datos.txt", "w"); // Abrir el archivo en modo binario
            if (archivo == NULL) {
                printf("Error al abrir el archivo.\n");
                return 0;
            }
            fprintf(archivo,"[");
            char output[100];
            for(int j=0;j<N_rx;j++){
                    if(signal_i[j]>=0)sprintf(output, " %f +%fj,",(signal_q[j]),(signal_i[j]));   
                    else sprintf(output, " %f %fj,", (signal_q[j]), (signal_i[j]));

                    fseek(archivo, 0, SEEK_END);
                    fprintf(archivo,output);
                    memset(output, 0x00, 100);
            }
            fprintf(archivo,"]");
            fclose(archivo);
    }
   
    //printf(" FIN \n");
    //while (!stop);
    //sleep(7);
    /*TRANSMITIR 0*/
    iio_buffer_push(txbuf);
	p_inc = iio_buffer_step(txbuf);
	p_end = iio_buffer_end(txbuf);
    for (p_dat = (char *)iio_buffer_first(txbuf, tx0_i); p_dat < p_end; p_dat += p_inc) {
        ((int16_t*)p_dat)[0] = 0;//(1*pow(2, 14));//signal[increm];
        ((int16_t*)p_dat)[1] = 0;//(1*pow(2, 14));//signal[increm];
    }


    int ret_output;
    ret_output = iio_channel_attr_write(chnn_altvoltage1_output, "frequency", "710000000");//# Frecuencia de la portadora 710000000
    if (ret_output < 0) { 
        perror("Error setting frecuency rate TX: [Transmisor] frecuencia portadora"); 
        return ret_output;
    }
     ret_output = iio_channel_attr_write(chnn_device_output,"hardwaregain", "-89");//hardwaregain -10
     if (ret_output < 0) {
         perror("Error setting hardwaregain rate TX: "); 
         return ret_output;
    }
 
    iio_buffer_destroy(txbuf);
    iio_channel_disable(tx0_i);
    iio_channel_disable(tx0_q);

    if(rx == true)iio_buffer_destroy(rxbuf);
    if(rx == true)iio_channel_disable(rx0_i);
    if(rx == true)iio_channel_disable(rx0_q);
    
    iio_context_destroy(ctx);
    return 0;
}

int config_filter(){

    FILE *filtro; // Modo lectura
    //char buffer[LONGITUD_MAXIMA_CADENA];         // Aquí vamos a ir almacenando cada línea
    char buffer[4096];
    char name_filter[40];
    ssize_t read_bytes;
    
    int num_linea = num_lines_rx();
    
    char *config[num_linea];
    char *values[num_linea];

    read_config_rx(config,values);
    
    if(!strcmp(config[0],"sampling_frequency")){
        int rate = atoi(values[0]); 
        if(rate<521000 ){
            printf("Error frecuencia muy baja\n");
            return -1;
        }
        
        else if(rate<=20000000){
            //printf("2-->%d\n",atoi(values[0]));
            strcpy(name_filter,FILTRO_1);
            //filtro = fopen(FILTRO_1, "r");
        }
        else if(rate<=40000000){
            //printf("3-->%d\n",atoi(values[0]));
            strcpy(name_filter,FILTRO_2);
            //filtro = fopen(FILTRO_2, "r");
        }
        else if(rate<=53333333){
            //printf("4-->%d\n",atoi(values[0]));
            strcpy(name_filter,FILTRO_3);
            //filtro = fopen(FILTRO_3, "r");
        }             //60380000
        else if(rate <= 61440000){
            //printf("5-->%d\n",atoi(values[0]));
            strcpy(name_filter,FILTRO_4);
            //filtro = fopen(FILTRO_4, "r");
        }
        else{
            printf("Error frecuencia muy alta\n");
            return -1;
        }
        //printf("name_filter %s \n",name_filter);
        filtro = fopen(name_filter, "r");
    }
    
    while ((read_bytes = fread(buffer, sizeof(char), sizeof(buffer), filtro)) > 0) 
     {
         if (iio_device_attr_write_raw(main_dev, "filter_fir_config", buffer, read_bytes) < 0) {
             printf("Failed to write filter configuration to device\n");
             fclose(filtro);
             iio_context_destroy(ctx);
             return -1;
         }
     }
     int ret_rx;
     struct iio_channel *chnn_out_input = iio_device_find_channel(main_dev, "out", false);//input 
     ret_rx = iio_channel_attr_write(chnn_out_input, "voltage_filter_fir_en", "1");//3999999
     if (ret_rx < 0) {
        printf("Error setting voltage_filter_fir_en out: "); 
        return ret_rx;
    }
    fclose(filtro);
 return 0;

}

int create_context(){
    ctx = iio_create_default_context();
    if (!ctx) {
        printf("Unable to create IIO context\n");
        return -1;
    }
    main_dev = iio_context_find_device(ctx, "ad9361-phy");
    iio_device_debug_attr_write(main_dev,"loopback",value_loopback);//0-1-2
    
    //const char * attr = iio_device_find_debug_attr(main_dev, "loopback");
    //char buffer[50];
    //iio_device_debug_attr_read(main_dev,"loopback",buffer,sizeof(buffer));
    //printf("value :%s\n",buffer);
    //printf("attr name %s\n",attr);
    //printf("value %s\n",buffer);
   
    return 0;
}

int read_config_tx(char *config[], char *values[]) {
    FILE *archivo;
    // Abre el archivo en modo de lectura ("r")
    archivo = fopen("tx_config.txt", "r");
    // Comprueba si el archivo se abrió correctamente
    if (archivo == NULL) {
        printf("No se pudo abrir el archivo read_config_tx.\n");
        return 1;
    }
    int num_linea = num_lines_tx();
    //printf("num_linea : %d\n",num_linea);
    char *lines[num_linea];

    char linea[100];
    // Lee el archivo línea por línea y guarda cada línea en el arreglo
    for (int i = 0; i < num_linea; i++) {
        if (fgets(linea, sizeof(linea), archivo) != NULL) {
            size_t linea_length = strlen(linea);
            if (linea[linea_length - 1] == '\n') {
                linea[linea_length - 1] = '\0';
                linea_length--;
            }
            // Copia la línea al arreglo
            lines[i] = malloc(linea_length + 1);
           strcpy(lines[i], linea);
        } else {
            printf("El archivo no tiene suficientes líneas.\n");
            break;
        }
    }

    for (int i = 0; i < num_linea; i++) {
        //printf("%s\n", lines[i]);  // Imprime la línea leída
        char *key, *value;
        key = strtok(lines[i], " ");
        value = strtok(NULL, " ");

        if (key != NULL && value != NULL) {
            //printf("%s %s \n", key,value);
            config[i] = key;
            values[i] = value;
        } else {
            printf("Error al separar la línea.\n");
        }
    }
    rewind(archivo); // Vuelve al inicio del archivo
    fclose(archivo);
    return 0;
}

int read_config_rx(char *config[], char *values[]) {
    FILE *archivo;
    // Abre el archivo en modo de lectura ("r")
    archivo = fopen("rx_config.txt", "r");
    // Comprueba si el archivo se abrió correctamente
    if (archivo == NULL) {
        printf("No se pudo abrir el archivo read_config_rx.\n");
        return 1;
    }
    int num_linea = num_lines_rx();
    //printf("num_linea : %d\n",num_linea);
    char *lines[num_linea];

    char linea[100];
    // Lee el archivo línea por línea y guarda cada línea en el arreglo
    for (int i = 0; i < num_linea; i++) {
        if (fgets(linea, sizeof(linea), archivo) != NULL) {
            size_t linea_length = strlen(linea);
            if (linea[linea_length - 1] == '\n') {
                linea[linea_length - 1] = '\0';
                linea_length--;
            }
            // Copia la línea al arreglo
            lines[i] = malloc(linea_length + 1);
           strcpy(lines[i], linea);
        } else {
            printf("El archivo no tiene suficientes líneas.\n");
            break;
        }
    }

    for (int i = 0; i < num_linea; i++) {
        //printf("%s\n", lines[i]);  // Imprime la línea leída
        char *key, *value;
        key = strtok(lines[i], " ");
        value = strtok(NULL, " ");

        if (key != NULL && value != NULL) {
            //printf("%s %s \n", key,value);
            config[i] = key;
            values[i] = value;
        } else {
            printf("Error al separar la línea.\n");
        }
    }
    rewind(archivo); // Vuelve al inicio del archivo
    fclose(archivo);
    return 0;
}

int num_lines_tx(){
     FILE *archivo;
    // Abre el archivo en modo de lectura ("r")
    archivo = fopen("tx_config.txt", "r");
    // Comprueba si el archivo se abrió correctamente
    if (archivo == NULL) {
        printf("No se pudo abrir el archivo num_lines_tx.\n");
        return 1;
    }
    // Lee el archivo línea por línea
    char linea[256];
    int num_lines_tx = 0;
    while (fgets(linea, sizeof(linea), archivo) != NULL) {
         num_lines_tx++;
    }
    rewind(archivo); // Vuelve al inicio del archivo
    return num_lines_tx;
}

int num_lines_rx(){
     FILE *archivo;
    // Abre el archivo en modo de lectura ("r")
    archivo = fopen("rx_config.txt", "r");
    // Comprueba si el archivo se abrió correctamente
    if (archivo == NULL) {
        printf("No se pudo abrir el archivo rx.txt num_lines_rx.\n");
        return 1;
    }
    // Lee el archivo línea por línea
    char linea[256];
    int num_lines_tx = 0;
    while (fgets(linea, sizeof(linea), archivo) != NULL) {
         num_lines_tx++;
    }
    rewind(archivo); // Vuelve al inicio del archivo
    return num_lines_tx;
}

int config_tx(){
    
    int num_linea = num_lines_tx();
    char *config[num_linea];
    char *values[num_linea];
    read_config_tx(config,values);
    int ret_output;
    /// configurando la salida
    chnn_device_output = iio_device_find_channel(main_dev, "voltage0", true);//output 
    
    ret_output = iio_channel_attr_write(chnn_device_output, config[0], values[0]);//sampling_frequency 700000
    if (ret_output < 0) {
       perror("Error setting sampling_frequency rate TX : "); 
        return ret_output;
    }

    ret_output = iio_channel_attr_write(chnn_device_output,config[1], values[1]);
    if (ret_output < 0) {
       perror("Error setting rf_bandwidth rate TX: "); 
        return ret_output;
    }
    //else printf("Set correct rf_bandwidth TX  \n");

    ret_output = iio_channel_attr_write(chnn_device_output,config[2], values[2]);//hardwaregain -10
    if (ret_output < 0) {
       perror("Error setting hardwaregain rate TX: "); 
        return ret_output;
    }
    
    chnn_altvoltage1_output = iio_device_find_channel(main_dev, "altvoltage1", true);//output 
    ret_output = iio_channel_attr_write(chnn_altvoltage1_output, config[3], values[3]);//# Frecuencia de la portadora 710000000
    if (ret_output < 0) { 
       perror("Error setting frecuency rate TX: [Transmisor] frecuencia portadora"); 
        return ret_output;
    }
    dev_tx = iio_context_find_device(ctx, "cf-ad9361-dds-core-lpc"); //dispositivo para enviar
    tx0_i = iio_device_find_channel(dev_tx, "voltage0", true);
    tx0_q = iio_device_find_channel(dev_tx, "voltage1", true);
    iio_channel_enable(tx0_i);
    iio_channel_enable(tx0_q);
    //size_t TxBufferSize     = 1048574;
    
    
    txbuf = iio_device_create_buffer(dev_tx, TxBufferSize, true);//Paso :0 Fin :-1225617408  Paso :-1225617408 Fin :-1225617408
    if (!txbuf) {
    perror("Could not create TX buffer");
        iio_context_destroy(ctx);
    }
    return 0;
}

int config_rx(){
    int ret_input;
    int num_linea = num_lines_rx();
    char *config[num_linea];
    char *values[num_linea];
    read_config_rx(config,values);

    // printf("num_linea %d \n",num_linea);
    // for (int i = 0; i < num_linea; i++) {
    //     printf("Attr rx: %s, Value: %s  Value: %d \n", config[i], values[i],i);
    // }
    //iio_channel_enable(chnn_device_input);
    chnn_device_input = iio_device_find_channel(main_dev, "voltage0", false);//input 
    int ret = iio_channel_attr_write(chnn_device_input, config[0], values[0]); //sampling_frequency /*RX Baseband rate 5000000=5 MSPS  */
    if (ret < 0) {
       printf("Error setting sampling_frequency rate RX: [%s]\n", values[0]); 
        return ret;
    }

    ret_input = iio_channel_attr_write(chnn_device_input,  config[1], values[1]);//rf_bandwidth 16000000
    if (ret_input < 0) {
       printf("Error setting rf_bandwidth rate RX: [%s]",values[1]); 
        return ret_input;
    }

    // gain_control_mode value: fast_attack
    // gain_control_mode_available value: manual fast_attack slow_attack hybrid
    ret_input = iio_channel_attr_write(chnn_device_input,  config[2], values[2]);
    if (ret_input < 0) {
       printf("Error setting rf_bandwidth rate RX: "); 
        return ret_input;
    }

    
    if(!strcmp(values[2],"manual")){
        ret_input = iio_channel_attr_write(chnn_device_input,  config[3], values[3]);//Ganancia hardwaregain max 73 # Gain applied to RX path. Only applicable when gain_control_mode is set to 'manual'
        if (ret_input < 0) {
            printf("Error setting hardwaregain rate RX: "); 
            return ret_input;
        }
        //else printf("Set correct hardwaregain RX  \n");
    }

    chnn_altvoltage0_output = iio_device_find_channel(main_dev, "altvoltage0", true);//RX LO
    //printf(" %s %s \n",config[4], values[4]);
    ret_input = iio_channel_attr_write(chnn_altvoltage0_output,  config[4], values[4]); //frequency 710000000 # RX LO frequency 710 Mhz
    if (ret_input < 0) { 
       printf("Error setting frecuency rate RX: "); 
        return ret_input;
    }

    dev_rx = iio_context_find_device(ctx, "cf-ad9361-lpc"); //dispositivo para enviar
    rx0_i = iio_device_find_channel(dev_rx, "voltage0", false);//(input)
    rx0_q = iio_device_find_channel(dev_rx, "voltage1", false);//(input)
    

    iio_channel_enable(rx0_i);
    iio_channel_enable(rx0_q);


    //size_t BufferSize     = 1048500;
    // size_t BufferSize     = pow(2, 12) ;//1048500 ;//4096;// * 171;//tamaño maximo permitido 409600
    
    rxbuf = iio_device_create_buffer(dev_rx, RxBufferSize, false);//Paso :0 Fin :-1225617408  Paso :-1225617408 Fin :-1225617408
    
    if (!rxbuf) {
        printf("Could not create rxbuf buffer");
        iio_context_destroy(ctx);
    }
    return 0;
}

int read_attr_txt() {
    FILE *file_attr;
    // Abre el archivo en modo de lectura ("r")
    
    file_attr = fopen("debug_attr.txt", "r");
    // Comprueba si el archivo se abrió correctamente
    if (file_attr == NULL) {
        printf("No se pudo abrir el archivo debug _attr.txt.\n");
        return 1;
    }


    char linea[256];

    int indice = 0;
   

    while (fgets(linea, sizeof(linea), file_attr) != NULL) {
        
        char *key, *value;
        key = strtok(linea, " ");
        value = strtok(NULL, " ");
        //printf("indice %d \n",indice);
        if(indice == 0){
            Longitud_del_pulso = atoi(value);
            //printf("0 Longitud_del_pulso %d \n", Longitud_del_pulso);
        }
        else if(indice == 1){
            //printf("1 %s %s \n", key,value);
            key = strtok(value, "_");
            value = strtok(NULL, " ");
            PRI  = (pow(atoi(key),atoi(value))-1); 
            //printf("1 PRI %d\n", PRI);
        }
        else if(indice == 2){
            //printf("2 amplitud %s %s \n", key,value);
            //amplitud = atoi(value);
            //char *endPtr;
            int decimal = strtol(value, &value, 16);
            //printf("2 amplitud%d \n", decimal);
            amplitud = decimal;

        }
        else if(indice == 3){
            key = strtok(value, "_");
            value = strtok(NULL, " ");
            TxBufferSize = (pow(atoi(key),atoi(value))-1); 
            //printf("3 TxBufferSize %d \n", TxBufferSize);
        }
        else if(indice == 4){
           
            key = strtok(value, "_");
            value = strtok(NULL, " ");
            
            RxBufferSize = (pow(atoi(key),atoi(value))-1); 
            //printf("4 RxBufferSize %d \n", RxBufferSize);
            N_rx = (int)RxBufferSize;
        }
        else if(indice == 5){
            rx = atoi(value);
            //printf("4 rx %s %s \n", key,value);
        }
        else if(indice == 6){
            strcpy(value_loopback,value);
            //printf("5 loopback %s \n",value);
            
        }
        
        indice++;
    }
    fclose(file_attr);
    return 0;
}
