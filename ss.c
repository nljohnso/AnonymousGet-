//Niklaus Johnson
//CS457
//Project 2
//ss.c

#include <pthread.h>
#include "awget.h"

#define MAX_NBR 3
#define INDEX_HTML "index.html"

void printSS(int socketfd, struct sockaddr_in serv_addr);
int bindPort(int port);
void process(void * aPacket);

/*-----------------------------------------------------------------------------*/
/*    main                                                                     */
/*-----------------------------------------------------------------------------*/
int main(int argc, char *argv[]) {

        socklen_t clientLen;
        struct sockaddr_in cli_addr;
        int newSocket;
        pthread_t thread;

	int port = 0;
	if(argc == 2) {
		port = atoi(argv[1]);
	}

	if(argc > 2) {
		fprintf(stderr, "ERROR incorrect arguments\n");
		exit(1);
	}		

	int socketfd = bindPort(port);
	while(TRUE) {		
	        listen(socketfd, 5);
	        clientLen = sizeof(cli_addr);
	        newSocket = accept(socketfd, (struct sockaddr *) &cli_addr, &clientLen);

		Packet *aPacket = readSocket(newSocket);
                aPacket->socket = newSocket;
		
                //process(aPacket);
                if (pthread_create (&thread, NULL, (void *) &process, (void *) aPacket))
		    fprintf(stderr, "ERROR unable to create thread.\n");
	}
}

/*-----------------------------------------------------------------------------*/
/*    process                                                                  */
/*-----------------------------------------------------------------------------*/
void process(void * voidPacket) {
	Packet * aPacket = (Packet *) voidPacket;
        printf("\tRequest: %s\n", aPacket->URL);

        if(isLastSS(aPacket)) {
            char * filename = downloadFile(aPacket->URL);
            printf("\tchainlist is empty\n\tissuing wget for file <%s>\n..\n", filename);
            printf("\tFile received\n\tRelaying file ...\n");
            transmitFile(filename, aPacket->socket);
            remove(filename);   
            printf("\tGoodbye!\n");
            close(aPacket->socket);
        }
        else {
            printf("\tchainlist is\n");
            for(int j = 1; j <= aPacket->fileSize; j++) {
                printf("\t%s\n", aPacket->chainFile[j]);
            } 
   	 
            int randStepNum = getRandStepNum(aPacket->chainFile[0]);
            char * steppingStone = aPacket->chainFile[randStepNum];
            printf("\tnext SS is %s\n", steppingStone);

            int socketfd = setupSocket(getIP(steppingStone), atoi(getPort(steppingStone)));
            removeSS(randStepNum, aPacket);
            socketfd = writeSocket(socketfd, aPacket);
            printf("\twaiting for file...\n..\n");
            relayFile(socketfd, aPacket->socket);
            printf("\tRelaying file ...\n\tGoodbye!\n");
            close(aPacket->socket); 	    
        }

        pthread_exit(NULL);
}

/*-----------------------------------------------------------------------------*/
/*    relayFile                                                                */
/*-----------------------------------------------------------------------------*/
void relayFile(int inSocket, int outSocket) {
	char buffer[PACKET_SIZE];
	int readSize = 0;
	int writeSize = 0;
	while((readSize = read(inSocket, &buffer, PACKET_SIZE)) > 0) {
	    writeSize = write(outSocket, buffer, readSize);
	    if(writeSize < 0) {
  		fprintf(stderr, "ERROR replaying file\n");
		exit(1);
     	    }
	}
}

/*-----------------------------------------------------------------------------*/
/*    transmitFile                                                             */
/*-----------------------------------------------------------------------------*/
void transmitFile(char *filename, int socketfd) {
	FILE *file;
	char buffer[PACKET_SIZE];

	file = fopen(filename, "r");
        if (file == NULL) {
            fprintf(stderr, "ERROR reading file (%s)\n", filename);
            exit(1);
        }
   
        int writeSize = 0;
	while(!feof(file)) {
            writeSize = fread(buffer, 1, PACKET_SIZE, file);
            int n = write(socketfd, buffer, writeSize);
            if(n < 0) {
		fprintf(stderr, "ERROR writing data to socket\n");
		exit(0);
            }
	}

	fclose(file);
}

/*-----------------------------------------------------------------------------*/
/*    downloadFile                                                             */
/*-----------------------------------------------------------------------------*/
char * downloadFile(char *URL) {

	char command[MAX_URL + 9];
	strcpy(command, "wget -q ");
	strcat(command, URL);
	system(command);
        return getFilename(URL);
}

/*-----------------------------------------------------------------------------*/
/*    isLastSS                                                                 */
/*-----------------------------------------------------------------------------*/
int isLastSS(Packet *aPacket) {

	char number[MAX_NBR];
	int j = 0;

	for(int i = 0; (char)aPacket->chainFile[0][i] != '\0'; i++) {
            if(isdigit(aPacket->chainFile[0][i])) {
                number[j] = aPacket->chainFile[0][i];
                j++;
            }
        }

	number[j] = '\0';

	if(atoi(number) > 0) {
		return FALSE;
	} 
        else {
		return TRUE;
	}
}

/*-----------------------------------------------------------------------------*/
/*    readSocket                                                               */
/*-----------------------------------------------------------------------------*/
Packet * readSocket(int socketfd) {

	int size;
        int n = read(socketfd, &size, sizeof(int));
        if(n < 0){
            fprintf(stderr, "ERROR reading from socket\n");
            exit(1);
        }

        int memsize = sizeof(char) * MAX_SIZE * MAX_SS + MAX_URL + sizeof(int);
        char* data = malloc(memsize);
	n = 0;
        while(n < size - sizeof(int)) {
            n += read(socketfd, &data[n], size - sizeof(int));
            if(n < 0){
                fprintf(stderr, "ERROR reading from socket\n");
               	exit(1);
            }
	}

	Packet *aPacket = Bytes2Packet(data, n);

	return aPacket;
}

/*-----------------------------------------------------------------------------*/
/*    Bytes2Packet                                                             */
/*-----------------------------------------------------------------------------*/
Packet * Bytes2Packet(char* data, int size) {
	Packet *aPacket = malloc(sizeof(Packet));
	strcpy(aPacket->URL, data);
	aPacket->fileSize = (size - MAX_URL) / MAX_SIZE;
	for(int i = 0; i < aPacket->fileSize; i++) {
            memcpy(aPacket->chainFile[i], &data[MAX_URL + (i * MAX_SIZE)], MAX_SIZE);
	}

	return aPacket;
}

/*-----------------------------------------------------------------------------*/
/*    bindPort                                                            */
/*-----------------------------------------------------------------------------*/
int bindPort(int port) {
	int socketfd, portNum, newSocket, n;
        struct sockaddr_in serv_addr;

        socketfd = socket(AF_INET, SOCK_STREAM, 0);

        if(socketfd < 0) {
                fprintf(stderr, "ERROR opening socket\n");
                exit(1);
        }
        bzero((char *) &serv_addr, sizeof(serv_addr));

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        serv_addr.sin_port = htons(port);

        if(bind(socketfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
                fprintf(stderr, "ERROR on binding\n");
                exit(1);
        }

	printSS(socketfd, serv_addr);

	return socketfd;
}

/*-----------------------------------------------------------------------------*/
/*    printSS                                                                  */
/*-----------------------------------------------------------------------------*/
void printSS(int socketfd, struct sockaddr_in serv_addr) {

	socklen_t len = sizeof(serv_addr);
        if (getsockname(socketfd, (struct sockaddr *)&serv_addr, &len) == -1) {
            fprintf(stderr, "ERROR on getsockname()\n");
        } 
        else {
            printf("ss <%s, %d>: \n", getIPAddress(), ntohs(serv_addr.sin_port));
        }
}

/*-----------------------------------------------------------------------------*/
/*    getIPAddress                                                             */
/*-----------------------------------------------------------------------------*/
const char * getIPAddress() {

	struct ifaddrs *ifap, *ifa;
	struct sockaddr_in *sa;
	char *addr;

	getifaddrs(&ifap);
	for(ifa = ifap; ifa; ifa = ifa->ifa_next) {
		if(ifa->ifa_addr->sa_family==AF_INET) { 
			sa = (struct sockaddr_in *) ifa->ifa_addr;
			addr = inet_ntoa(sa->sin_addr);
			if(strcmp(ifa->ifa_name, "eth0") == 0) {
				break;
			}
		}
	}

	freeifaddrs(ifap);

	return addr;
}
