all: client server server2
client:
	gcc -pthread -g -Wall -o client client.c
server:
	gcc -pthread -g -Wall -o server server.c
server2:
	gcc -pthread -g -Wall -o server2 server2.c
clean:
	rm -f client
	rm -f server
	rm -f server2
	rm -rf *.o
