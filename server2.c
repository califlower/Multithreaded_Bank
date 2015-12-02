#include "client_server.h"

/*GLOBAL VARIABLES and STRUCTS*/
/******************************
 * accountList holds accounts
 * port is a gloabl variable for the Server Port, can be modified in server.h
 * numAcc holds the total number of accounts in the system
******************************/

static account *accountList[maxAcc]	={NULL}; /*share this*/
static int port				=pNum;
static int *numAcc			=0; /*share this*/
static int sock=			-1; /*share this?*/

pthread_t 				thread;
pthread_t				lThread;
pthread_mutex_t 			addLock;  /*share this*/
pthread_mutex_t 			startLock; /*share this*/
sem_t 					semaphore; /*share this*/

/*********************
 * Handles printing every 20 seconds
 * Contains the semaphore unlock after 20 seconds
*******************/

void alarmHandler()
{
	sem_post(&semaphore);
}

/**************************
 * Print list of all account every 20 seconds
 * Happens in a seperate thread
***************************/

void *printAccounts(void *emptyPtr)
{
	while (1)
	{
		signal(SIGALRM, alarmHandler);
		alarm(20);
		sem_wait(&semaphore);
		
		printf("ACCOUNT LIST\n");
		int i=0;
		printf("---------------------------------------\n");
		for (i=0;(i<*numAcc && accountList[i]!=NULL);i++)
		{
			printf("%s\n", accountList[i]->accountName);
			printf("        %f\n", accountList[i]->balance);
			(accountList[i]->inUse==1) ? printf("        IN SESSION\n"):printf("        NOT IN SESSION\n");
		}
		
		printf("----------------------------------------\n");
		
		
	}
	pthread_exit(0);
}

/*******************
 * Cleanly handles exit from terminal closes or control c
 * Goes through the socket list and disconnects all the clients cleanly
 * Clients then close
******************/
void exitHandler()
{
	struct sockNode *iter=head;
	char input[strSize]="exit";
	int len= strlen(input);


	while (iter)
	{
		write(iter->sock, &len, sizeof(int));
		write(iter->sock, input, len);
		close(iter->sock);
		iter=iter->next;
	}
	exit(0);	
}

/***************************
 	Adds an Account to the AccountList Array
 	Contains a mutex Lock, function is strictly sequential
	Return 0 if Sucess 
	Return 1 if too many accounts
	return 2 if accountname too large
	return 3 if account already exists
**************************/

int addAccount(char * name)
{
	pthread_mutex_lock(&addLock);
	
	if (*numAcc>=maxAcc)
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
		int i;
		for (i=0; i<*numAcc && accountList[i]!=NULL; i++)
		{
			if (strcasecmp(name, accountList[i]->accountName)==0)
			{
				pthread_mutex_unlock(&addLock);
				return 3;
			}
				

		}
		account *newAccount=malloc(sizeof(account));
		strcpy(newAccount->accountName,name);
		newAccount->balance=0;
		newAccount->inUse=0;
		accountList[*numAcc]=newAccount;
		*numAcc++;
		
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
			
			if (accountList[i]->inUse==0)
			{
				accountList[i]->inUse=1;
				pthread_mutex_unlock(&startLock);
				return i;
			}
			else
			{
				pthread_mutex_unlock(&startLock);
				return -2;
			}

		}
	
	}
	
	return -1;
}
/**************
 * removes money from an account
 * Returns 0 if sucess
 * Returns 1 if it would go below zero with a credit amount
************/
int debitAccount(int id, float amount)
{
	
	if ((accountList[id]->balance)-amount<0)
		return 1;
	else
	{
		accountList[id]->balance-=amount;
		return 0;
	}
}

/***********
 * Adds money to an account
***********/
void creditAccount(int id, float amount)
{
	accountList[id]->balance+=amount;
}

/*****************************
 * Client process. Handles inputs and then decides what to do based on the input
 * Each client gets one thread
 * Not super complicated
 * Each client process gets its own accountName and ID
******************************/
void * process(void * ptr)
{
	char * accountName=NULL;
	int accountId;
	
	char buffer[strSize];
	int len=strSize;
	
	connection_t * conn = (connection_t *) ptr;

	while (1)
	{
		
		read(conn->sock, &len, sizeof(int));
		buffer[len] = 0;
		read(conn->sock, buffer, len);
		
/**************************************************************
 * If client wants to exit account and close terminal
 * Set inUse to 0
 * Loop is broken
 * Client exits
*******************************************************************/
		if (strcasecmp(buffer,"exit")==0)
		{
			if (accountName)
				accountList[accountId]->inUse=0;
			break;
		}
		
/**************************************************************
 * Creates a new user account
 * Handles the different return conditions
***************************************************************/
		
		else if (strcasecmp(buffer,"open")==0)
		{
			read(conn->sock, &len, sizeof(int));
			buffer[len] = 0;
			read(conn->sock, buffer, len);

			if (strcasecmp(buffer,"exit")==0)
			{
				if (accountName)
					accountList[accountId]->inUse=0;
				break;
			}
			
			int x=addAccount(buffer);
			
			if (x==0)
			{
				char str[strSize]= "Account Added";
				len=strlen(str);
				write(conn->sock, &len, sizeof(int));
				write(conn->sock, str, strlen(str));
				continue;

			}
			else if (x==1)
			{
				char str[strSize]= "Too many accounts";
				len=strlen(str);
				write(conn->sock, &len, sizeof(int));
				write(conn->sock, str, len);
				continue;
			}
			else if (x==2)
			{
				char str[strSize]= "Account name too large";
				len=strlen(str);
				write(conn->sock, &len, sizeof(int));
				write(conn->sock, str, len);
				continue;

			}
			else if (x==3)
			{
				char str[strSize]= "Account already exists";
				len=strlen(str);
				write(conn->sock, &len, sizeof(int));
				write(conn->sock, str, len);
				continue;
			}
			
		}
		
/*******************************************************************
 * Starts account session
 * Deals with error conditions
 * Sets account id and name
********************************************************************/
		else if (strcasecmp(buffer,"start")==0)
		{
			read(conn->sock, &len, sizeof(int));
			buffer[len] = 0;
			read(conn->sock, buffer, len);

			if (strcasecmp(buffer,"exit")==0)
			{
				if (accountName)
					accountList[accountId]->inUse=0;
				break;
			}
			
			int x=startAccount(buffer);
			
			if (x==-1)
			{
				char str[strSize]= "Account does not exist";
				len=strlen(str);
				write(conn->sock, &len, sizeof(int));
				write(conn->sock, str, strlen(str));
				continue;

			}
			else if (x==-2)
			{
				char str[strSize]= "Account in use";
				len=strlen(str);
				write(conn->sock, &len, sizeof(int));
				write(conn->sock, str, strlen(str));
				continue;

			}
			else
			{
				char str[strSize]= "Started session with account name : ";
				strcat(str, accountList[x]->accountName);
				accountId=x;
				accountName=accountList[x]->accountName;
				len=strlen(str);
				write(conn->sock, &len, sizeof(int));
				write(conn->sock, str, strlen(str));
				continue;
			}
		}
		
/*********************************************************************************
 * Debits the account money
 * handles error conditions from debit
 * Uses snprintf to combine strings and ints
*********************************************************************************/
		else if (strcasecmp(buffer,"debit")==0)
		{
			if (!accountName)
			{
				char str[strSize]= "No account session started";
				len=strlen(str);
				write(conn->sock, &len, sizeof(int));
				write(conn->sock, str, strlen(str));
				continue;

			}

			read(conn->sock, &len, sizeof(int));
			buffer[len] = 0;
			read(conn->sock, buffer, len);

			if (strcasecmp(buffer,"exit")==0)
			{
				if (accountName)
					accountList[accountId]->inUse=0;
				break;
			}
			
			int x=debitAccount(accountId,atof(buffer));
			
			if (x==0)
			{
				char str[strSize];
				
				snprintf(str, sizeof(str), "Account debited -%f", atof(buffer));
				len=strlen(str);
				write(conn->sock, &len, sizeof(int));
				write(conn->sock, str, strlen(str));
				continue;

			}
			else if (x==1)
			{
				char str[strSize]= "Cannot debit. Not enough money in account";
				len=strlen(str);
				write(conn->sock, &len, sizeof(int));
				write(conn->sock, str, strlen(str));
				continue;

			}
			
		}
		
/****************************************************************
 * Credits the account
 * Operates the same as debit
****************************************************************/
		else if (strcasecmp(buffer,"credit")==0)
		{

			if (!accountName)
			{
				char str[strSize]= "No account session started";
				len=strlen(str);
				write(conn->sock, &len, sizeof(int));
				write(conn->sock, str, strlen(str));
				continue;

			}

			read(conn->sock, &len, sizeof(int));
			buffer[len] = 0;
			read(conn->sock, buffer, len);

			if (strcasecmp(buffer,"exit")==0)
			{
				if (accountName)
					accountList[accountId]->inUse=0;
				break;
			}
			
			creditAccount(accountId,atof(buffer));
			char str[strSize];	
			snprintf(str, sizeof(str), "Account credited +%f", atof(buffer));
			len=strlen(str);
			write(conn->sock, &len, sizeof(int));
			write(conn->sock, str, strlen(str));
			continue;

			
			
		}
		
/*********************************************************************
* Gets the account balance and sends it to the client
***********************************************************************/
		else if (strcasecmp(buffer, "balance")==0)
		{
			if (!accountName)
			{
				char str[strSize]= "No account session started";
				len=strlen(str);
				write(conn->sock, &len, sizeof(int));
				write(conn->sock, str, strlen(str));
				continue;
			}
			char str[strSize];
			snprintf(str, sizeof(str), "Total Balance of Account: %f", accountList[accountId]->balance);
			len=strlen(str);
			write(conn->sock, &len, sizeof(int));
			write(conn->sock, str, strlen(str));
			continue;
			
		}
		
/**********************************************************************
* Sets account and account id to -1 (honestly I hope that works)
* Sets account name to null so that a new account can be used
* Sets inuse to 0 so account can be used by someone else
* Doesnt exit thread, client still not exited either
************************************************************************/
		else if (strcasecmp(buffer, "finish")==0)
		{
			if (!accountName)
			{
				char str[strSize]= "No account session started";
				len=strlen(str);
				write(conn->sock, &len, sizeof(int));
				write(conn->sock, str, strlen(str));
				continue;
			}
			accountList[accountId]->inUse=0;
			accountName=NULL;
			accountId=0;	
	
			char str[strSize]="Account released";
			len=strlen(str);
			write(conn->sock, &len, sizeof(int));
			write(conn->sock, str, strlen(str));
			
			continue;
			
		}
		
	}

	printf("Client Exiting\n");
	close(conn->sock);
	free(conn);
	pthread_exit(0);

}



/*****************
 * Sits and listens for incoming client connections
 * Starts a client thread on every connection
 * ADD AN EXIT CONDITION AT SOME POINT
*********************/
void listenConnection(int sock)
{
	while (1)
	{
		/* accept incoming connections */
		connection_t *connection = (connection_t *)calloc(3, sizeof(connection_t));
		connection->sock = accept(sock, &connection->address, &connection->addr_len);
		
		if (connection->sock <= 0)
		{
			free(connection);
		}
		else
		{
			int pid=fork();
			if (pid==0)
			{
				printf("Client Connection Accepted\n");
				int temp=getpid();
				printf("Process Created with ID : ");
				printf("%i\n", temp);
				pthread_create(&thread, 0, process, (void *)connection);
				pthread_detach(thread);
				
				struct sockNode *temp=malloc(sizeof(struct sockNode));
				temp->sock=connection->sock;
				temp->next=head;
				head=temp;	
			}
			
		}
	}
}

/******************
 * Starts the server connection
******************/

void initConnection()
{
	
	struct sockaddr_in address;
	
	pthread_mutexattr_t addAtt;
	pthread_mutexattr_t addAtt2;

	pthread_mutexattr_init(&addAtt)
	pthread_mutexattr_init(&addAtt2);
	
	pthread_mutexattr_setpshared(&addAtt, PTHREAD_PROCESS_SHARED);
	pthread_mutexattr_setpshared(&addAtt2, PTHREAD_PROCESS_SHARED);

	pthread_mutex_init(&addLock, &addAtt);
	pthread_mutex_init(&startLock, &addAtt2);
	
	sem_init(&semaphore, 0, 0);

	signal(SIGHUP, exitHandler);
	signal(SIGINT, exitHandler);
	
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
	
	pthread_create(&lThread, NULL, printAccounts, NULL);
	pthread_detach(lThread);
	
	listenConnection(sock);
	

}

int main(int argc, char ** argv)
{
	accountList = mmap(NULL, sizeof *accountList, PROT_READ | PROT_WRITE, 
                    MAP_SHARED | MAP_ANONYMOUS, -1, 0);
        numAcc = mmap(NULL, sizeof *numAcc, PROT_READ | PROT_WRITE, 
                    MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	initConnection();
	return 0;

}
