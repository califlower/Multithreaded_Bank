#include "client_server.h"

int port		=pNum;
int sock 		= -1;
sem_t 			semaphore;
sem_t			recSemaphore;

/****************
 * Prints the instructions for using the bank
****************/
void printInstructions()
{

	printf("Connection to Online Bank Successful\n");
	printf("Welcome to Your Online Bank\n");
	printf("-----------------------------\n");
	printf("Options\n");
	printf("open   <account name> to open a new account\n");
	printf("start  <account name> to start a new account session\n");
	printf("credit <amount> to credit an account that was opened\n");
	printf("debit  <amount> to debit an account that was opened\n");
	printf("balance to print total balance in bank account\n");
	printf("finish to release your current account session\n");
	printf("exit to exit your session completely\n");
	printf("-----------------------------\n");
	printf("\n");
}

/********************
	Exits the server process when the terminal
	gets closed so that it doesnt get stuck in its
	stupid loop forever and ever and keep printing stuff.
	
	This is called pretty much any time the client needs to exit
	It allows the client to tell the server what's happening and exit everything cleanly
*****************/
void exitHandler()
{
	char input[strSize]="exit";
	int len= strlen(input);

	if (sock!=-1)
	{
		write(sock, &len, sizeof(int));
		write(sock, input, len);
	}
	exit(0);	
}
void alarmHandler()
{
	sem_post(&semaphore);
}
void reconnectAlarmHandler()
{
	sem_post(&recSemaphore);
}

/*******************
	Creates the client connection to the specified server
	Runs in main thread
********************/

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
	
	while (connect(sock, (struct sockaddr *)&address, sizeof(address))==-1)
	{
		printf("NO SERVER FOUND...Trying again\n");
		signal(SIGALRM,reconnectAlarmHandler);
		alarm(2);
		sem_wait(&recSemaphore);
		
	}

}

/*************************
	Sits and waits for an input, sits in its own thread.
	Need to implement a way for the server to send a shutdown signal
	to the client
***************************/
void *recieveInput(void *emptyPtr)
{
	int len=strSize;
	char input[strSize];
	
	while (1)
	{	
		/* read length of message */
		
		read(sock, &len, sizeof(int));
		input[len] = 0;

		/* read message */
		read(sock, input, len);
		
		if (strcmp(input, "exit")==0)
		{
			printf("Server Exiting\n");
			exitHandler();
			pthread_exit(0);
		}
		signal(SIGALRM, alarmHandler);
		alarm(2);
		sem_wait(&semaphore);
		
		/* print message */
		printf("%s\n", input);
		

		
	}

	return NULL;
}

/**********************************
	Waits for a client input to send to the server
	Sits in its own thread waiting. Exits if "exit" is inputted
	Also closes the thread in the server by calling exithandler()
	
	This function is throttled. Only accepts input every 2 seconds
	This is the project requirments
*********************/

void *sendInput(void *emptyPtr)
{
	int len;
	char input[strSize];

	while (1)
	{
		scanf("%s", (char *)&input);
		
		len = strlen(input);

		write(sock, &len, sizeof(int));
		write(sock, input, len);

		if (strcmp(input, "exit")==0)
		{
			exitHandler();
		}
	}

	return NULL;

}

/**********
 * Main threads
 * Inits the threads and calls all the proper commands to start the client
************/

int main(int argc, char ** argv)
{
	pthread_t rThread;
	pthread_t sThread;

	sem_init(&semaphore, 0, 0);
	sem_init(&recSemaphore, 0, 0);
	
	signal(SIGHUP, exitHandler);
	signal(SIGINT, exitHandler);
	
	(argc==2) ? initConnection(argv[1]): initConnection("localhost");

	printInstructions();

	pthread_create(&sThread, NULL, sendInput, NULL);
	pthread_create(&rThread, NULL, recieveInput, NULL);

	pthread_join(rThread,NULL);
	pthread_join(sThread,NULL);

	/* close socket */
	close(sock);

	return 0;
}
