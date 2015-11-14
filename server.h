#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

#define pNum 2500
#define maxAcc 20
#define strSize 1000
#define maxName 100

typedef struct
{
	int sock;
	struct sockaddr address;
	socklen_t addr_len;

} connection_t;


typedef struct
{
	char accountName[maxName];
	float balance;
	int inUse;
} account;
