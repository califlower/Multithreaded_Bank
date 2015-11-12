#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

typedef struct
{
	int sock;
	struct sockaddr address;
	socklen_t addr_len;

} connection_t;


typedef struct
{
	char accountName[100];
	float balance;
	int inUse;
} account;
