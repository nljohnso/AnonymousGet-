//Niklaus Johnson
//awget.h

#ifndef AWGET_H_
#define AWGET_H_

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

#include "common.h"

/*--- Structures -------------------------------------------------------------------------------*/

typedef struct {
        char URL[MAX_URL];
        char filename[MAX_FILENAME];
} Input;

/*--- Prototypes -------------------------------------------------------------------------------*/

Input validateInput(int, char**);
void printChainFile(const char *);
char * fileToString(const char *);
void removeSS(int, Packet *);
Packet * buildPacket(char **, char *);
const char * getIPAddress();
char ** readFile(const char *);
int bindAndListen(int);
Packet * readSocket(int);
Packet * Bytes2Packet(char *, int);
int isLastSS(Packet*);
char * downloadFile(char *);
void transmitFile(char *, int);
void makeFile(int, char *);
void relayFile(int, int);

#endif
