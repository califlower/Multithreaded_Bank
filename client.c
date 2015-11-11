#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>

#define PORT 2500
#define LOCALHOST 127.0.0.1

int main(int argc, char ** argv)
{
	
	int sock = -1;
	struct sockaddr_in address;
	struct hostent * host;
	int len;
	

	/* create socket */
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock <= 0)
	{
		fprintf(stderr, "%s: error: cannot create socket\n", argv[0]);
		return -3;
	}

	/* connect to server */
	address.sin_family = AF_INET;
	address.sin_port = htons(PORT);
	
	host = LOCALHOST;
	
	memcpy(&address.sin_addr, host->h_addr_list[0], host->h_length);
	
	if (connect(sock, (struct sockaddr *)&address, sizeof(address)))
	{
		fprintf(stderr, "%s: error: cannot connect to host %s\n", argv[0], argv[1]);
		return -5;
	}

	/* send text to server */
	len = strlen(argv[1]);
	write(sock, &len, sizeof(int));
	write(sock, argv[1], len);

	/* close socket */
	close(sock);

	return 0;
}

