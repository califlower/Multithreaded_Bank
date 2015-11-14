#include			<stdio.h>
#include 			<stdlib.h>
#include 			<pthread.h>
#include 			<sys/socket.h>
#include 			<netinet/in.h>
#include 			<unistd.h>
#include 			<string.h>
#include 			<netdb.h>
#include 			<signal.h>


/*****************************
 * pNum is the port number of the server
 * maxAcc is the maximum number of accounts the server can hold
 * strSize is a magic number that I've been setting the strings too. Feel free to make it bigger
 	Any smaller than like, 30 though and stuff stops working. Keep it above 100 almost always
 	Keep it at a 1000 though because it works
 *maxName is the maximum size of an account name
******************************/


#define pNum 			2500
#define maxAcc 			20
#define strSize 		1000
#define maxName 		100

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
