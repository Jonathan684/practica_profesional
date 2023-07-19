CC=@arm-linux-gnueabihf-gcc
CFLAGS=-mfloat-abi=hard --sysroot=$(HOME)/pluto-0.32.sysroot -std=gnu99 -g -Wall -Wextra
LIBS=-lpthread -liio -lm
all: tx_rx tx_rx_v2
tx_rx: tx_rx 
	$(CC) $(CFLAGS) -o tx_rx tx_rx.c $(LIBS)
tx_rx.o: tx_rx.c
	$(CC) $(CFLAGS) -c tx_rx.c 

tx_rx_v2: tx_rx_v2
	$(CC) $(CFLAGS) -o tx_rx_v2 tx_rx_v2.c $(LIBS)
tx_rx_v2.o: tx_rx_v2.c
	$(CC) $(CFLAGS) -c tx_rx_v2.c 
clean:
	rm  -f *.o tx_rx tx_rx_v2
#rm  -f *.o  sdr_1 sdr_2 prueba_33