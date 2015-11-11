all: client server
client:
	gcc -Wall -o client client.c
server:
	gcc -pthread -Wall -o server server.c
clean:
	rm -f s1
	rm -rf *.o