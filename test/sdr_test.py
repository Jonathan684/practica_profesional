import matplotlib.pyplot as plt
import mpld3

from os import remove
import numpy as np
import matplotlib.pyplot as plt
import matplotlib
import ast
import sys
# Read the file pps_docker/ADALM-PLUTO-SDR/output/datos.txt
with open("../ADALM-PLUTO-SDR/output/datos.txt", "r") as file:
    content = file.read()

#print(content)
archivo = open('../ADALM-PLUTO-SDR/configuration/debug_attr.txt', 'r')

# Lee el contenido del archivo línea por línea
lineas = archivo.readlines()

# Cierra el archivo
archivo.close()


#PRI = 2**15-1
#amplitud = 4080# 2 ** 14
#N = 2**18-1  # Replace N with the desired value of your signal length buffer rx



# Imprime el contenido línea por línea
for i, linea in enumerate(lineas):
    
    elementos = linea.split()
    if i == 0:
        Longitud_del_pulso = int(elementos[1])
        #print("Longitud_del_pulso ",Longitud_del_pulso)
    if i == 1:
        #cadena = elementos[0]
        #valor = elementos[1]
        vec=elementos[1].split("_")
        PRI = int(vec[0])**int(vec[1])-1
        #print("PRI",PRI)
    if i == 2:
        amplitud = int(elementos[1], 16)
        #print("amplitud",amplitud)    
    if i == 3:
        TxBufferSize_str = elementos[1]
        TxBufferSize_str=TxBufferSize_str.replace("_", "^")
        vec=elementos[1].split("_")
        TxBufferSize = int(vec[0])**int(vec[1])-1
    if i == 4:
        RxBufferSize_str = elementos[1]
        RxBufferSize_str=RxBufferSize_str.replace("_", "^")
        vec=elementos[1].split("_")
        RxBufferSize = int(vec[0])**int(vec[1])-1
        #print("RxBufferSize",RxBufferSize)
        break    
    contador = i + 1

#print("*"*20)
#print("Longitud_del_pulso",Longitud_del_pulso)
# print("PRI",PRI)
# print("amplitud",amplitud)
# print("RxBufferSize",RxBufferSize)
# print("TxBufferSize",TxBufferSize)
# print("*"*20)
N = RxBufferSize
count = 0
signal = np.zeros(N)
signal_tx = np.zeros((1, N), dtype=complex)  # Inicializar el array de pulsos

try:
    # Tu código que genera el error
    signal_rx = ast.literal_eval(content)
    
    # Resto de tu código
    # ...
    
except (SyntaxError, ValueError) as e:
    # Manejo del error
    print("Ocurrió un error al evaluar la expresión literal datos.txt dañado")
    sys.exit()
#signal_rx = ast.literal_eval(content)

max = np.real(signal_rx).argmax()
val_max = np.real(signal_rx)[max]
#print("val_max..",val_max)
amplitud = val_max
for n in range(N):
    if 0 <= count < Longitud_del_pulso:
        signal[n] = 1 / amplitud
        signal_tx[0,n] = amplitud+amplitud*1j
       
    elif Longitud_del_pulso <= count < PRI:
        signal[n] = 0.0
        signal_tx[0,n] = 0+0j
    count += 1
    if count == PRI:
        count = 0

signal_tx = signal_tx[0]


matplotlib.rcParams['axes.edgecolor'] = 'blue'

def plotSignal(signal_tx,signal_rx,max):
    
    #------------------------------- Transmitted I component -------------------------------
    # Plot time domain
    fig = plt.figure(figsize=(20, 8), dpi=80, facecolor='w', edgecolor='k')
    
    # Subplot 1 - Transmitted I component
    ax1 = fig.add_subplot(2, 2, 1)
    ax1.plot(np.real(signal_rx),color = "blue", label = "recibida RX")
    ax1.plot(np.real(signal_tx), color="red", label = "transmitida")
    matplotlib.rc('axes',edgecolor='blue')
    num_samples = len(signal_rx)
    x_ticks = np.linspace(0, num_samples-1, 11, dtype=int)
    ax1.set_xticks(x_ticks)
    ax1.legend()
    
    ax1.set_ylabel('Amplitude')
    ax1.set_xlabel('samples')
    ax1.set_title('In-phase Component of the Signal_rx and tx     [size buffer_rx :{}({}) size buffer_tx :{}({}) PRI:{} LG:{}]'.format(RxBufferSize, RxBufferSize_str,TxBufferSize,TxBufferSize_str,PRI,Longitud_del_pulso))
    ax1.grid()
    
    # Subplot 2 - Transmitted Q component
    ax2 = fig.add_subplot(2, 2, 2)
    ax2.plot(np.imag(signal_rx), color = "blue", label = "recibida")
    ax2.plot(np.real(signal_tx), color="red", label = "transmitida")
    ax2.set_ylabel('Amplitude')
    ax2.set_xlabel('samples')
    ax2.set_title('Quadrature Component of the Signal and tx')
    ax2.legend()
    ax2.grid()
    
    #ZOOM
    medio = N /2 #max# int((2**18)/2) 
    #medio = int((2**18)/2) 
    #medio = PRI
    mini = medio - 20000
    maxim = medio + 20000

    # Subplot 3 - Component in quadrature (first 1000 samples)
    ax3 = fig.add_subplot(2, 2, 3)
    ax3.set_xlim(mini,maxim)
    ax3.plot(np.real(signal_rx),color="blue", label = "recibida Rx")
    ax3.plot(np.real(signal_tx),color="red", label = "transmitida TX")
    ax3.set_ylabel('Amplitude max[{}]'.format(np.real(signal_rx)[max]))
    
    ax3.set_xlabel('samples')
    ax3.set_title('In-phase Component of the Signal_rx and tx (zoom centro)')
    ax3.grid()
    ax3.legend()
    #x_min = 0
    #x_max = 1000
    
    # Subplot 4 - Component in quadrature (first 1000 samples)
    ax4 = fig.add_subplot(2, 2, 4)
    ax4.set_xlim(mini,maxim)
    ax4.plot(np.imag(signal_rx),color="blue", label = "recibida")
    ax4.plot(np.imag(signal_tx),color="red", label = "transmitida")
    ax4.set_ylabel('Amplitude')
    ax4.set_xlabel('samples')
    ax4.set_title('Quadrature Component of the Signal and tx (zoom centro)')
    ax4.grid()
    ax4.legend()
    plt.tight_layout()
    plt.savefig('test.png')
    #plt.style.use('dark_background')
    #plt.show()


min = np.real(signal_rx).argmin()
val_max = np.real(signal_rx)[max]
val_min = np.real(signal_rx)[min]

# print("Maximo :",np.real(signal_rx)[max])
# print("Min :",np.real(signal_rx)[min])
#print("Posicion max:",max)
# print("Posicion min:",min)
# print("signal_rx:",len(signal_rx))

#print("signal_rx:",len(signal_rx))
plotSignal(signal_tx,signal_rx,max)


#plotSignal(txSignal)
min = 0
max = 200000

#print(np.real(txSignal[10000:10000]))
flag = 0
for i in np.real(signal_rx[min:max]):
    if(i  > 0.20):
        #print(i)
        flag += 1
        break
    flag += 1

# Customize the style of the plot
# Customize the style of the plot
#plt.rcParams['axes.edgecolor'] = 'gray'  # Set the color of the axes edges to gray
# plt.rcParams['axes.spines.left'] = False  # Remove the left spine
# plt.rcParams['axes.spines.right'] = False  # Remove the right spine
# plt.rcParams['axes.spines.top'] = False  # Remove the top spine

# html_content = mpld3.fig_to_html(plt.gcf())
# with open("graph.html", "w") as f:
#     f.write(html_content)

print("FIN")