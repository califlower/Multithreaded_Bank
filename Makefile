all: client server
client:
	gcc -pthread -g -Wall -o client client.c
server:
	gcc -pthread -g -Wall -o server server.c
clean:
	rm -f client
	rm -f server
	rm -rf *.o
