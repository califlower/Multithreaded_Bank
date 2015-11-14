#include "server.h"


/*GLOBAL VARIABLES and STRUCTS*/
/******************************
 * accountList holds accounts
 * port is a gloabl variable for the Server Port, can be modified in server.h
 * numAcc holds the total number of accounts in the system
******************************/

static account *accountList[maxAcc]	={NULL};
static int port				=pNum;
static int numAcc			=0;

pthread_t 				thread;
pthread_mutex_t 			addLock;
pthread_mutex_t 			startLock;


/***************************
 	Adds an Account to the AccountList Array
 	Contains a mutex Lock, function is strictly sequential
	Return 0 if Sucess 
	Return 1 if too many accounts
	return 2 if accountname too large
**************************/

int addAccount(char * name)
{
	pthread_mutex_lock(&addLock);
	
	if (numAcc>=maxAcc)
	{
		pthread_mutex_unlock(&addLock);
		
		return 1;	
	}
	else if (strlen(name)>maxName)
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

/*************************
 * Starts an account session
 * Return -2 if account is already open
 * return -1 if account does not exist
 * return index number of account if it exists and isnt open
 * mutex lock on checking if an account is inUse so two threads dont try to modify the variable at the same time
 * mutex lock not on the search itself so two threads can still search seperatly
****************************/

int startAccount(char * name)
{
	int i=0;
	
	for (i=0; i<maxAcc; i++)
	{
		if (!accountList[i])
		{
			return -1;
		}
		else if (strcasecmp(name, accountList[i]->accountName)==0)
		{
			pthread_mutex_lock(&startLock);
			
			if (accountList->inUse==0)
			{
				accountList[i]->inUse=1;
				pthread_mutex_unlock(&startLock);
				return i
			}
			else
			{
				pthread_mutex_unlock(&startLock);
				return -2;
			}
;
		}
	
	}
	
	return -1;
}

/*MAIN CLIENT PROCESS*/
void * process(void * ptr)
{
	char * accountname;
	int accountId;
	
	char buffer[strSize];
	int len=strSize;
	
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
				char str[strSize]= "Account Added";
				len=strlen(str);
				write(conn->sock, &len, sizeof(int));
				write(conn->sock, str, strlen(str));

			}
			else if (x==1)
			{
				char str[strSize]= "Too many accounts";
				len=strlen(str);
				write(conn->sock, &len, sizeof(int));
				write(conn->sock, str, len);
			}
			else
			{
				char str[strSize]= "Account name too large";
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

/*****************
 * Sits and listens for incoming client connections
 * Starts a client thread on every connection
 * ADD AN EXIT CONDITION AT SOME POINT
*********************/
void listenConnection(connection_t * connection, int sock)
{
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
}

/******************
 * Starts the server connection
******************/

void initConnection()
{
	int sock;
	struct sockaddr_in address;
	pthread_mutex_init(&addLock, NULL);
	pthread_mutex_init(&startLock, NULL);
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
	
	listenConnection(connection, sock);
	

}

int main(int argc, char ** argv)
{
	initConnection();
	return 0;

}
