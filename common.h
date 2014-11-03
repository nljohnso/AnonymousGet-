//Niklaus Johnson
//common.h

#ifndef COMMON_H_
#define COMMON_H_

#define _GNU_SOURCE
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <time.h>
#include <pthread.h>

#define DEBUG
#define TRUE 1
#define FALSE 0
#define INDEX_HTML "index.html"
#define PACKET_SIZE 1200
#define MAX_SIZE 1000
#define MAX_URL 1000
#define MAX_FILENAME 1000
#define MAX_SS 10 

/*--- Structures -------------------------------------------------------------------------------*/

typedef struct {
	char URL[2083];
	char chainFile[MAX_SS][MAX_SIZE];
	int fileSize;
        int socket;
} Packet;

/*--- Prototypes -------------------------------------------------------------------------------*/

int exists(const char *);
char * getFilename(char *);
char * getIP(char *);
char * getPort(char *);
int getRandStepNum(char * fileLines);
char * Packet2Bytes(Packet*);
void removeSS(int, Packet *);
int setupSocket(char *, int);
int writeSocket(int, Packet*);

#endif
