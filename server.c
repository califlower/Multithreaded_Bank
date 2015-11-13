#include "server.h"

account accountList[20]= {NULL };
int port=2500;

void * process(void * ptr)
{
	char buffer[1000];
	int len=1000;
	connection_t * conn;

	if (!ptr) 
		pthread_exit(0); 

	conn = (connection_t *)ptr;

	while (1)
	{
		/* read length of message */
		read(conn->sock, &len, sizeof(int));
	
		buffer[len] = 0;

		/* read message */
		read(conn->sock, buffer, len);
		
		if (strcmp(buffer,"3")==0)
			break;

		/* print message */
		printf("%s\n", buffer);
		
	}

	/* close socket and clean up */
	close(conn->sock);
	free(conn);
	pthread_exit(0);

}

void addAccount(char * name)
{
	
}

void debitAccount(char *account, float amount)
{
	
}


void creditAccount(char *account, float amount)
{
	
}

void initConnection()
{
	int sock;
	struct sockaddr_in address;
	
	
	connection_t * connection;
	pthread_t thread;

	/* create socket */
	sock = socket(AF_INET, SOCK_STREAM, 0);


	/* bind socket to port */
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);

	if(bind(sock, (struct sockaddr *)&address, sizeof(struct sockaddr_in)) < 0)
	{
		printf("Error in Binding!");
		exit(0);
	}
	
	/* listen on port */
	listen(sock, 5);


	printf("Now Accepting Incoming Client Connections...\n");
	
	while (1)
	{
		/* accept incoming connections */
		connection = (connection_t *)malloc(sizeof(connection_t));
		connection->sock = accept(sock, &connection->address, &connection->addr_len);
		if (connection->sock <= 0)
		{
			free(connection);
		}
		else
		{
			/* start a new thread but do not wait for it */
			pthread_create(&thread, 0, process, (void *)connection);
			pthread_detach(thread);
		}
	}
	
	return;
}

int main(int argc, char ** argv)
{
	initConnection();
	return 0;

}
