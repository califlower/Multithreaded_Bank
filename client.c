#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char ** argv)
{
	int port=2500;
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
	address.sin_port = htons(port);
	host = gethostbyname(argv[1]);
	if (!host)
	{
		fprintf(stderr, "%s: error: unknown host %s\n", argv[0], argv[1]);
		return -4;
	}
	memcpy(&address.sin_addr, host->h_addr_list[0], host->h_length);
	
	if (connect(sock, (struct sockaddr *)&address, sizeof(address)))
	{
		fprintf(stderr, "%s: error: cannot connect to host %s\n", argv[0], argv[1]);
		return -5;
	}

	
	
	
	printf("Connection to Online Bank Successful\n");
	printf("Welcome to Your Online Bank\n");
	printf("-----------------------------\n");
	printf("Options\n");
	printf("1: Open\n");
	printf("2: Start\n");
	printf("3: Exit\n");
	
	char input[10];
	
	while (1)
	{
		scanf("%s", (char *)&input);
		if (strcmp(input, "1")==0)
			break;
		
		len = strlen(input);
		write(sock, &len, sizeof(int));
		write(sock, input, len);
	}
	

	/* close socket */
	close(sock);

	return 0;
}

