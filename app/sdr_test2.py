import matplotlib.pyplot as plt
import mpld3

from os import remove
import numpy as np
import matplotlib.pyplot as plt
import matplotlib
import ast
import sys
# Read the file pps_docker/ADALM-PLUTO-SDR/output/datos.txt
with open("../ADALM-PLUTO-SDR/output/datos_0.txt", "r") as file:
    content0 = file.read()
with open("../ADALM-PLUTO-SDR/output/datos_1.txt", "r") as file:
    content1 = file.read()
with open("../ADALM-PLUTO-SDR/output/datos_2.txt", "r") as file:
    content2 = file.read()
#.devcontainer/practica_profesional/ADALM-PLUTO-SDR/configuration/debug_attr.txt

try:
    signal_rx_0 = ast.literal_eval(content0)
    signal_rx_1 = ast.literal_eval(content1)
    signal_rx_2 = ast.literal_eval(content2)
except (ValueError, SyntaxError):
    print("Error al convertir la cadena en una lista de números complejos.")
    sys.exit()

signal_rx = signal_rx_0 + signal_rx_1 + signal_rx_2
#print("0",len(signal_rx_0))
#print("1",len(signal_rx_1))
#print("2",len(signal_rx_2))
matplotlib.rcParams['axes.edgecolor'] = 'blue'

def plotSignal(signal_rx):

    fig = plt.figure(figsize=(20, 8), dpi=80, facecolor='w', edgecolor='k')
    ax1 = fig.add_subplot(1, 1, 1)
    ax1.plot(np.real(signal_rx),color = "blue", label = "received RX")
    matplotlib.rc('axes',edgecolor='blue')
    num_samples = len(signal_rx)
    x_ticks = np.linspace(0, num_samples-1, 11, dtype=int)
    ax1.set_xticks(x_ticks)
    ax1.legend()
    x_v = len(signal_rx)/3
    ax1.axvline(x=x_v, color='red', linestyle='-')
    ax1.axvline(x=x_v*2, color='red', linestyle='-')
    ax1.axvline(x=x_v*3, color='red', linestyle='-')
    #ax1.axvline(x=6, color='blue', linestyle=':')
    ax1.set_ylabel('Amplitude')
    ax1.set_xlabel('samples')
    ax1.set_title('In-phase Component of the Signal_rx and tx {}.Las lineas rojas separan cada lectura del buffer (test2)'.format(len(signal_rx)))
    ax1.grid()
    plt.tight_layout()
    plt.savefig('test.png')
#plt.style.use('dark_background')
    #plt.show()
plotSignal(signal_rx)

#print("FIN")