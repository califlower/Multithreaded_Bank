#include "client.h"

int port=2500;
int sock = -1;


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

void initConnection(char * inputHost)
{
	struct sockaddr_in address;
	struct hostent * host;

	int len;
	
	/* create socket */
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	/* connect to server */
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	host = gethostbyname(inputHost);

	memcpy(&address.sin_addr, host->h_addr_list[0], host->h_length);
	
	connect(sock, (struct sockaddr *)&address, sizeof(address));

	
	
	
	

}

void *recieveInput()
{
	
}

void *sendInput(void *emptyPtr)
{
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
}

int main(int argc, char ** argv)
{
	pthread_t rThread;
	pthread_t sThread;
	
	initConnection();
	
	pthread_create(&sThread, NULL, sendInput, NULL);
	pthread_join(sThread,NULL);
	
	printInstructions();
	

	/* close socket */
	close(sock);

	return 0;
}
