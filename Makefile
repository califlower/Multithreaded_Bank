all: client server
client:
	gcc -pthread -Wall -o client client.c
server:
	gcc -pthread -Wall -o server server.c
clean:
	rm -f client
	rm -f server
	rm -rf *.o
