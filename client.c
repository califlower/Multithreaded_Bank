#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

void printInstructions()
{

	printf("Connection to Online Bank Successful\n");
	printf("Welcome to Your Online Bank\n");
	printf("-----------------------------\n");
	printf("Options\n");
	printf("1: Open\n");
	printf("2: Start\n");
	printf("3: Exit\n");

}

void cHandler(int signum)
{

}

int main(int argc, char ** argv)
{

	signal(SIGTERM, cHandler);
	int port=2500;
	int sock = -1;
	
	struct sockaddr_in address;
	struct hostent * host;

	int len;
	
	/* create socket */
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	/* connect to server */
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	host = gethostbyname(argv[1]);

	memcpy(&address.sin_addr, host->h_addr_list[0], host->h_length);
	
	connect(sock, (struct sockaddr *)&address, sizeof(address));

	printInstructions();
	
	char input[1000];
	
	while (1)
	{
		scanf("%s", (char *)&input);

		
		len = strlen(input);
		write(sock, &len, sizeof(int));
		write(sock, input, len);

		if (strcmp(input, "3")==0)
			break;
	}
	

	/* close socket */
	close(sock);

	return 0;
}
