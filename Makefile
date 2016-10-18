CC=g++

CFLAGS=-c -Wall -O2 -g -Werror -Wpedantic

all: client server

client: client_main.cpp client_model.o 
	$(CC) $^ -o $@

server: server_model.o
	$(CC) $^ -o $@

client_model.o: client_model.cpp client_parameters.txt client.h
	$(CC) $(CFLAGS) $<

server_model.o: server_model.c parameters.txt
	$(CC) $(CFLAGS) $<

clean:
	rm *.o client server
