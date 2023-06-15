CC = gcc
CFLAGS = -Wall

all: server client1 client2 client3 client4 input1 input2 input3 input4

server: server.c
	$(CC) -o server server.c

client1: client-monitor.c
	$(CC) -DCLIENT1  -o client1 client-monitor.c

client2: client-monitor.c
	$(CC) -DCLIENT2  -o client2 client-monitor.c

client3: client-monitor.c
	$(CC) -DCLIENT3  -o client3 client-monitor.c

client4: client-monitor.c
	$(CC) -DCLIENT4  -o client4 client-monitor.c

input1: client-input.c
	$(CC) -DCLIENT1  -o input1 client-input.c

input2: client-input.c
	$(CC) -DCLIENT2  -o input2 client-input.c

input3: client-input.c
	$(CC) -DCLIENT3  -o input3 client-input.c

input4: client-input.c
	$(CC) -DCLIENT4  -o input4 client-input.c

clean:
	rm -f server client1 client2 client3 client4 input1 input2 input3 input4