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

/*
	Creates the client connection to the specified server
	Runs in main thread
*/
void initConnection(char * inputHost)
{
	struct sockaddr_in address;
	struct hostent * host;
	
	/* create socket */
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	/* connect to server */
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	host = gethostbyname(inputHost);

	memcpy(&address.sin_addr, host->h_addr_list[0], host->h_length);
	
	connect(sock, (struct sockaddr *)&address, sizeof(address));

}

/*
	Sits and waits for an input, sits in its own thread.
	Need to implement a way for the server to send a shutdown signal
	to the client
*/
void *recieveInput(void *emptyPtr)
{
	while (1)
	{	
		int len=1000;
		char input[1000];	

		/* read length of message */

		read(sock, &len, sizeof(int));
		input[len] = 0;

		/* read message */
		read(sock, input, len);
		
		/* print message */
		printf("%s\n", input);
	}

	return NULL;
}

/*
	Waits for a client input to send to the server
	Sits in its own thread waiting. Exits if 3 is inputted
	Also closes the thread in the server
*/

void *sendInput(void *emptyPtr)
{
	int len;
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

	return NULL;

}

/*
	Exits the server process when the terminal
	gets closed so that it doesnt get stuck in its
	stupid loop forever and ever and keep printing stuff.
*/
void exitHandler(int signum)
{
	char input[5]="3";
	int len= strlen(input);

	if (sock!=-1)
	{
		write(sock, &len, sizeof(int));
		write(sock, input, len);
	}	
}

int main(int argc, char ** argv)
{
	pthread_t rThread;
	pthread_t sThread;

	signal(SIGHUP, exitHandler);

	initConnection(argv[1]);
	printInstructions();

	pthread_create(&sThread, NULL, sendInput, NULL);
	pthread_create(&rThread, NULL, recieveInput, NULL);

	pthread_join(rThread,NULL);
	pthread_join(sThread,NULL);
	

	/* close socket */
	close(sock);

	return 0;
}
