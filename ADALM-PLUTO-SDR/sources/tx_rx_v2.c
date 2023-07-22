#include <stdio.h>
#include <stdlib.h>
#include <iio.h>
#include <math.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>


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
int pulse_length  = 1;
int PRI  = 0;//(pow(2, 12)-1);  //150000;//(pow(2, 17)-1);   // TxBufferSize   //16383;//4; //Minimo 4 
int16_t amplitude = 0;//0x0FFF ; //4080;  // 0x0FFF;  // Jupyter
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

int read_attr_txt();

int attr_debug();
int settings_tx();
int settings_rx();

int main(){
    stop = false;
    signal(SIGINT, handle_sig);
    ssize_t nbytes_tx,nbytes_rx;
	char *p_dat, *p_end;
	ptrdiff_t p_inc;

    if (create_context()<0)return 0;
    
    read_attr_txt();
    if(config_filter()<0){
       iio_context_destroy(ctx);
       return 0;
    }
    settings_tx();
    if(rx == 1)settings_rx();
    
    attr_debug();
    //printf("RxBufferSize %d \n",(int)RxBufferSize);
    /*TRANSMITIR*/
    nbytes_tx = iio_buffer_push(txbuf);// solo es valida para buffer de salida.
    if (nbytes_tx < 0) printf("Error pushing buf %d\n", (int) nbytes_tx); 
    
    p_inc = iio_buffer_step(txbuf);
	p_end = iio_buffer_end(txbuf);
    int sample = 0;
    
    for (p_dat = (char *)iio_buffer_first(txbuf, tx0_i); p_dat < p_end; p_dat += p_inc) {
       if ( (sample >= 0) && (sample < pulse_length)  ) {
            ((int16_t*)p_dat)[0] = amplitude;
            ((int16_t*)p_dat)[1] = amplitude;
        }
        else if ( (sample >= pulse_length ) && (sample < PRI ) ) {
            ((int16_t*)p_dat)[0] =((int16_t)0x0000);
            ((int16_t*)p_dat)[1] =((int16_t)0x0000);
        }
        sample ++;
        if (sample == PRI) sample = 0;
    }

    /*RECIBIR*/
    if(rx == 1){
            
            int I_rx = 0;

            float signal_i_0[(int)RxBufferSize];
            float signal_q_0[(int)RxBufferSize];
            float signal_i_1[(int)RxBufferSize];
            float signal_q_1[(int)RxBufferSize];
            float signal_i_2[(int)RxBufferSize];
            float signal_q_2[(int)RxBufferSize];

            //1 muestra del buffer
            iio_buffer_refill(rxbuf);
            p_inc = iio_buffer_step(rxbuf);
            p_end = iio_buffer_end(rxbuf);
          
            for (p_dat = (char *)iio_buffer_first(rxbuf, rx0_i); p_dat < p_end; p_dat += p_inc) {     
                    signal_i_0[I_rx] = (float) (((int16_t*)p_dat)[0]);//REAL I
                    signal_q_0[I_rx] = (float) (((int16_t*)p_dat)[1]);//IMAG Q
                    I_rx ++;
            }
            //2 muestra del buffer
            nbytes_rx=iio_buffer_refill(rxbuf);
            if (nbytes_rx < 0)printf("Error refilling buf\n");
            I_rx = 0;
            p_inc = iio_buffer_step(rxbuf);
            p_end = iio_buffer_end(rxbuf);
          
            for (p_dat = (char *)iio_buffer_first(rxbuf, rx0_i); p_dat < p_end; p_dat += p_inc) {     
                signal_i_1[I_rx] = (float) (((int16_t*)p_dat)[0]);
                signal_q_1[I_rx] = (float) (((int16_t*)p_dat)[1]);
                I_rx ++;
            } 
            //3 muestra del buffer
            nbytes_rx=iio_buffer_refill(rxbuf);
            if (nbytes_rx < 0)printf("Error refilling buf\n");
            I_rx = 0;
            p_inc = iio_buffer_step(rxbuf);
            p_end = iio_buffer_end(rxbuf);
          
            for (p_dat = (char *)iio_buffer_first(rxbuf, rx0_i); p_dat < p_end; p_dat += p_inc) {     
                signal_i_2[I_rx] = (float) (((int16_t*)p_dat)[0]);
                signal_q_2[I_rx] = (float) (((int16_t*)p_dat)[1]);
                I_rx ++;
            } 



            FILE* archivo = fopen("datos_0.txt", "w"); // Abrir el archivo en modo binario
            if (archivo == NULL) {
                printf("Error al abrir el archivo.\n");
                return 0;
            }
            fprintf(archivo,"[");
            char output[100];
            for(int j=0;j<(int)RxBufferSize;j++){
                    if(signal_i_0[j]>=0)sprintf(output, " %f +%fj,",(signal_q_0[j]),(signal_i_0[j]));   
                    else sprintf(output, " %f %fj,", (signal_q_0[j]), (signal_i_0[j]));

                    fseek(archivo, 0, SEEK_END);
                    fprintf(archivo,output);
                    memset(output, 0x00, 100);
            }
            fprintf(archivo,"]");
            fclose(archivo);

            //2
            FILE* datos_1 = fopen("datos_1.txt", "w"); // Abrir el archivo en modo binario
            if (datos_1 == NULL) {
                printf("Error al abrir el archivo.\n");
                return 0;
            }
            fprintf(datos_1,"[");
            for(int i=0;i<(int)RxBufferSize;i++){
                if(signal_i_1[i]>=0)sprintf(output, " %f +%fj,",(signal_q_1[i]),(signal_i_1[i]));   
                else sprintf(output, " %f %fj,", (signal_q_1[i]), (signal_i_1[i]));
                fseek(datos_1, 0, SEEK_END);
                fprintf(datos_1,output);
                memset(output, 0x00, 100);
            }
            fprintf(datos_1,"]");
            fclose(datos_1);
            //3
            FILE* datos_2 = fopen("datos_2.txt", "w"); // Abrir el archivo en modo binario
            if (datos_2 == NULL) {
                printf("Error al abrir el archivo.\n");
                return 0;
            }
            fprintf(datos_2,"[");
            for(int j=0;j<(int)RxBufferSize;j++){
                if(signal_i_2[j]>=0)sprintf(output, " %f +%fj,",(signal_q_2[j]),(signal_i_2[j]));   
                else sprintf(output, " %f %fj,", (signal_q_2[j]), (signal_i_2[j]));
                fseek(datos_2, 0, SEEK_END);
                fprintf(datos_2,output);
                memset(output, 0x00, 100);
            }
            fprintf(datos_2,"]");
            fclose(datos_2);


    }
   
    //printf(" FINISH\n");
    //while (!stop);
    //sleep(7);
    /*TRANSMITIR 0*/
    iio_buffer_push(txbuf);
	p_inc = iio_buffer_step(txbuf);
	p_end = iio_buffer_end(txbuf);
    for (p_dat = (char *)iio_buffer_first(txbuf, tx0_i); p_dat < p_end; p_dat += p_inc) {
        ((int16_t*)p_dat)[0] = 0;
        ((int16_t*)p_dat)[1] = 0;
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

int settings_tx(){
    
    FILE *file_tx;
    file_tx = fopen("tx_config.txt", "r");
    if (file_tx == NULL) {
        printf("No se pudo abrir el archivo tx_config.txt.\n");
        return 1;
    }
    
    char linea[256];
    int indice = 0;
    chnn_device_output = iio_device_find_channel(main_dev, "voltage0", true);//output
    chnn_altvoltage1_output = iio_device_find_channel(main_dev, "altvoltage1", true);//output 

    while (fgets(linea, sizeof(linea), file_tx) != NULL) {
        //printf("-->%s \n",linea);
        char *parameter, *value;
        parameter = strtok(linea, " ");
        value = strtok(NULL, " ");

        if(indice == 0){
            //printf("0 %s %s\n",parameter,value);
            if ((iio_channel_attr_write(chnn_device_output, parameter,value)) < 0){
                printf("Error setting sampling_frequency rate TX : %s\n",strerror(errno)); 
                return -1;
            }
        }
        else if(indice == 1){
            //printf("1 %s %s\n",parameter,value);
            if ((iio_channel_attr_write(chnn_device_output, parameter,value)) < 0){
                printf("Error setting rf_bandwidth rate TX : %d\n",errno); 
                return -1;
            }
        }
        else if(indice == 2){
            //printf("2 %s %s\n",parameter,value);
            if ((iio_channel_attr_write(chnn_device_output, parameter,value)) < 0){
                printf("Error setting hardwaregain rate TX : %d\n",errno); 
                return -1;
            }
        }
        else if(indice == 3){
            //printf("3 %s %s\n",parameter,value);
            if ((iio_channel_attr_write(chnn_altvoltage1_output, parameter,value)) < 0){
                printf("Error setting frecuency rate TX : %s\n",strerror(errno)); 
                return -1;
            }
        }
        indice ++;
    }
    fclose(file_tx);
    dev_tx = iio_context_find_device(ctx, "cf-ad9361-dds-core-lpc"); //dispositivo para enviar
    tx0_i = iio_device_find_channel(dev_tx, "voltage0", true);
    tx0_q = iio_device_find_channel(dev_tx, "voltage1", true);
    iio_channel_enable(tx0_i);
    iio_channel_enable(tx0_q);
    
    //size_t TxBufferSize     = 1048574;
    txbuf = iio_device_create_buffer(dev_tx, TxBufferSize, true);//Paso :0 Fin :-1225617408  Paso :-1225617408 Fin :-1225617408
    if (!txbuf) {
        printf("Could not create TX buffer\n");
        iio_context_destroy(ctx);
        return -1;
    }
    return 0;
}

int settings_rx(){
    
    FILE *file_rx;
    file_rx = fopen("rx_config.txt", "r");
    if (file_rx == NULL) {
        printf("No se pudo abrir el archivo debug _attr.txt.\n");
        return -1;
    }
    
    char linea[256];
    char tmp[40];
    int indice = 0;

    chnn_device_input = iio_device_find_channel(main_dev, "voltage0", false);//input 
    chnn_altvoltage0_output = iio_device_find_channel(main_dev, "altvoltage0", true);//RX LO

    while (fgets(linea, sizeof(linea), file_rx) != NULL) {
        //printf("-->%s \n",linea);
        char *parameter, *value;
        parameter = strtok(linea, " ");
        value = strtok(NULL, " ");

        if(indice == 0){
            //printf("0 %s %s\n",parameter,value);
            if ((iio_channel_attr_write(chnn_device_input, parameter,value)) < 0){
                printf("Error setting sampling_frequency rate RX : %d\n",errno); 
                return -1;
            }
        }
        else if(indice == 1){
            //printf("1 %s %s\n",parameter,value);
            if ((iio_channel_attr_write(chnn_device_input, parameter,value)) < 0){
                printf("Error setting rf_bandwidth rate RX : %d\n",errno); 
                return -1;
            }
        }
        else if(indice == 2){
            //printf("2 %s %s\n",parameter,value);
            if ((iio_channel_attr_write(chnn_device_input, parameter,value)) < 0){
                printf("Error setting rf_bandwidth rate RX : %d\n",errno); 
                return -1;
            }
            strcpy(tmp, value);
            tmp[strlen(tmp)-1] = '\0';
        }
        else if(indice == 3){
            //printf("3 %s ->%s<-\n",parameter,tmp);
            if(!strcmp(tmp,"manual")){
                if ((iio_channel_attr_write(chnn_device_input, parameter,value)) < 0){
                    printf("Error setting hardwaregain rate RX: %s\n",strerror(errno)); 
                    return -1;
                }
            }  
        }
        else if(indice == 4){
            //printf("4 %s %s\n",parameter,value);
            if ((iio_channel_attr_write(chnn_altvoltage0_output, parameter,value)) < 0){
                printf("Error setting frecuency rate RX : %d\n",errno); 
                return -1;
            }
        }
        indice ++;
    }
    fclose(file_rx);
    
    dev_rx = iio_context_find_device(ctx, "cf-ad9361-lpc");     //dispositivo para enviar
    rx0_i = iio_device_find_channel(dev_rx, "voltage0", false); //(input)
    rx0_q = iio_device_find_channel(dev_rx, "voltage1", false); //(input)
    
    iio_channel_enable(rx0_i);
    iio_channel_enable(rx0_q);
    
    rxbuf = iio_device_create_buffer(dev_rx, RxBufferSize, false);//size_t BufferSize     = 1048500;  pow(2, 12) tamaño maximo permitido 409600
    if (!rxbuf) {
        printf("Could not create rxbuf buffer");
        iio_context_destroy(ctx);
        return -1;
    }
    return 0;
}

int config_filter(){

    FILE *filtro; // Modo lectura
    char buffer[4096];
    char name_filter[40];
    ssize_t read_bytes;
    
    
    char linea[256]; // Buffer para almacenar la línea leída
    const char* name = "rx_config.txt"; // Ruta del archivo de texto

    FILE* file_rx = fopen(name, "r");
    if (file_rx == NULL) {
        perror("Error al abrir el archivo");
        return 1;
    }
    fgets(linea, sizeof(linea), file_rx);
    fclose(file_rx);

    char *parameter, *value;
    parameter = strtok(linea, " ");
    value = strtok(NULL, " ");
   
    if(!strcmp(parameter,"sampling_frequency")){
        int rate = atoi(value); 
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

    return 0;
}

int attr_debug(){
    
    iio_device_debug_attr_write(main_dev,"loopback",value_loopback);//0-1-2
    // const char * attr = iio_device_find_debug_attr(main_dev, "loopback");
    // char buffer[50];
    // iio_device_debug_attr_read(main_dev,"loopback",buffer,sizeof(buffer));
    // printf("value :%s\n",buffer);
    // printf("attr name %s\n",attr);
    // printf("value %s\n",buffer);
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
            pulse_length = atoi(value);
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
            //printf("2 amplitude %s %s \n", key,value);
            //amplitude = atoi(value);
            //char *endPtr;
            int decimal = strtol(value, &value, 16);
            //printf("2 amplitude%d \n", decimal);
            amplitude = decimal;

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
            
        }
        else if(indice == 5){
            rx = atoi(value);
            //printf("4 rx %s %s \n", key,value);
        }
        else if(indice == 6){
            strcpy(value_loopback,value);
            //printf("loopback %s \n",value);
        }
        
        indice++;
    }
    fclose(file_attr);
    return 0;
}
