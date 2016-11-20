CC=g++

CFLAGS=-c -Wall -O0 -g -Werror -Wpedantic

all: client server

client: client_main.o client_model.o 
	$(CC) $^ -o $@

server: server_model.o
	$(CC) $^ -o $@

client_model.o: client_model.cpp client_parameters.txt client.h
	$(CC) $(CFLAGS) $<

client_main.o: client_main.cpp client.h
	$(CC) $(CFLAGS) $<

server_model.o: server_model.cpp parameters.txt
	$(CC) $(CFLAGS) $<

clean:
	rm *.o client server
