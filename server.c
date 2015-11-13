#include "server.h"

/*GLOBAL VARIABLES and STRUCTS*/
static account *accountList[20]= {NULL};
static int port=2500;
static int numAcc=0;

pthread_t thread;
pthread_mutex_t addLock;




/* Return 0 if Sucess, Return 1 if too many accounts, return 2 if accountname too large*/
int addAccount(char * name)
{
	pthread_mutex_lock(&addLock);
	
	if (numAcc>=20)
	{
		pthread_mutex_unlock(&addLock);
		
		return 1;	
	}
	else if (strlen(name)>100)
	{
		pthread_mutex_unlock(&addLock);
		
		return 2;
	}
	else
	{
		account *newAccount=malloc(sizeof(account));
		strcpy(newAccount->accountName,name);
		newAccount->balance=0;
		newAccount->inUse=0;
		accountList[numAcc]=newAccount;
		numAcc++;
		pthread_mutex_unlock(&addLock);
		
		return 0;
	}
	
}

/*MAIN CLIENT PROCESS*/
void * process(void * ptr)
{
	char buffer[1000];
	int len=1000;
	connection_t * conn = (connection_t *) ptr;

	while (1)
	{
		/* read length of message */
		read(conn->sock, &len, sizeof(int));
		buffer[len] = 0;
		
		/* read message */
		read(conn->sock, buffer, len);
		
		if (strcmp(buffer,"3")==0)
			break;
			
		else if (strcasecmp(buffer,"open")==0)
		{
			read(conn->sock, &len, sizeof(int));
			buffer[len] = 0;
			read(conn->sock, buffer, len);
			
			int x=addAccount(buffer);
			
			if (x==0)
			{
				char str[30]= "Account Added";
				len=strlen(str);
				write(conn->sock, &len, sizeof(int));
				write(conn->sock, str, strlen(str));

			}
			else if (x==1)
			{
				char str[30]= "Too many accounts";
				len=strlen(str);
				write(conn->sock, &len, sizeof(int));
				write(conn->sock, str, len);
			}
			else
			{
				char str[30]= "Account name too large";
				len=strlen(str);
				write(conn->sock, &len, sizeof(int));
				write(conn->sock, str, len);

			}
			
			
			
		}

		/* print message */
		printf("%s\n", buffer);
		
	}

	/* close socket and clean up */
	close(conn->sock);
	free(conn);
	pthread_exit(0);

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
	pthread_mutex_init(&addLock, NULL);
	connection_t * connection;

	/* create socket */
	sock = socket(AF_INET, SOCK_STREAM, 0);


	/* bind socket to port */
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);

	bind(sock, (struct sockaddr *)&address, sizeof(struct sockaddr_in));
	
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
