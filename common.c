//Niklaus Johnson
//CS457
//Project 2
//common.c

#include "awget.h"
#include "common.h"

/*-----------------------------------------------------------------------------*/
/*    getFilename                                                              */
/*-----------------------------------------------------------------------------*/
char * getFilename(char* url) {

        char * filename = malloc (MAX_FILENAME);

        int i = MAX_FILENAME-1;
        int j = 0;
        filename[MAX_FILENAME] = '\0';
        for (j = strlen(url); j > 0; j--) {
            if(url[j] == '/') {
                j = 1;
                break;
            }
            filename[i--] = url[j];

        }
	
        if (j == 0 || strcmp(&filename[i+1], "") == 0)
            return INDEX_HTML;

        return &filename[i+1];
}

/*-----------------------------------------------------------------------------*/
/*    setupSocket                                                              */ 
/*-----------------------------------------------------------------------------*/
int setupSocket(char * ip, int port) {
	
	int socketfd;
	struct sockaddr_in serv_addr;

	socketfd = socket(AF_INET, SOCK_STREAM, 0);

	if(socketfd < 0) {
            fprintf(stderr, "ERROR opening socket\n");
            exit(1);
	}

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(ip);
	serv_addr.sin_port = htons(port);

	if(connect(socketfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
            fprintf(stderr, "ERROR connecting\n");
            exit(0);
	}

        return socketfd;
}

/*-----------------------------------------------------------------------------*/
/*    writeSocket                                                              */ 
/*-----------------------------------------------------------------------------*/
int writeSocket(int socketfd, Packet* aPacket) {
        char * data = Packet2Bytes(aPacket);
	int memsize = sizeof(char) * MAX_SIZE * aPacket->fileSize + MAX_URL + sizeof (int);
	int n = write(socketfd, data, memsize);
	if(n < 0) {
            fprintf(stderr, "ERROR writing data to socket\n");
	    exit(0);
	}

	return socketfd;
}

/*-----------------------------------------------------------------------------*/    
/*    Packet2Bytes                                                             */ 
/*-----------------------------------------------------------------------------*/
char * Packet2Bytes(Packet* aPacket) {

	int memsize = sizeof(char) * MAX_SIZE * aPacket->fileSize + MAX_URL + sizeof (int);
	char * bytes = (unsigned char *) malloc(memsize);
        memset(bytes, ' ', memsize);
        memcpy(bytes, &memsize, sizeof(int));
        memcpy(&bytes[sizeof(int)], aPacket->URL, MAX_URL);
	for(int i = 0; i < aPacket->fileSize; i++) {
            memcpy(&bytes[sizeof(int) + MAX_URL + (i * MAX_SIZE)], aPacket->chainFile[i], MAX_SIZE);
	}

	return bytes;
}

/*-----------------------------------------------------------------------------*/
/*    removeSS                                                                 */
/*-----------------------------------------------------------------------------*/
void removeSS(int SSNum, Packet *aPacket) {
        int i = 0;
	for(i = SSNum; aPacket->chainFile[i] != NULL && i < MAX_SS; i++) {
		strcpy(aPacket->chainFile[i], aPacket->chainFile[i + 1]);
	}
	aPacket->fileSize = aPacket->fileSize - 1;
	
	char *tmp = aPacket->chainFile[0];
	char *charToNum = malloc(sizeof (char) * 1);
	charToNum[0] = tmp[1];
	int num = (atoi(charToNum)) - 1;
	char *newLineNum = malloc(sizeof (char) * 4);
	newLineNum[0] = '<';
	newLineNum[1] = (char)(((int)'0')+(num));
	newLineNum[2] = '>';
	newLineNum[3] = '\0';
	strcpy(aPacket->chainFile[0], newLineNum);
}

/*-----------------------------------------------------------------------------*/
/*    getIP                                                                    */
/*-----------------------------------------------------------------------------*/
char * getIP(char *ss) {
	int found = FALSE;
	char* ip = malloc (sizeof (char) * 16);
	int j = 0;
	for(int i = 0; i < strlen(ss); i++) {
		if(ss[i] == '<' && !found) {
			found = TRUE;
			continue;	
		}
		if(ss[i] == ',') {
			ip[j] = '\0';
			break;
		}
		if(found) {
			ip[j] = ss[i];
			j++;
		}
	}
	return ip;
}

/*-----------------------------------------------------------------------------*/
/*    getPort                                                                  */
/*-----------------------------------------------------------------------------*/
char * getPort(char *ss) {
	int found = 0;
	char* port = malloc (sizeof (char) * 5);
	for(int i, j = 0; i < strlen(ss); i++) {
		if(ss[i] == ' ' && !found) {
			found = 1;
			continue;
		}
		if(ss[i] == '>') {
			port[j] = '\0';
			break;
		}
		if(found) {
			port[j] = ss[i];
			j++;
		}
	}
	return port;
}

/*-----------------------------------------------------------------------------*/
/*    getRandStepNum                                                           */
/*-----------------------------------------------------------------------------*/
int getRandStepNum(char *fileContents) {
	char* n = fileContents;
	char* tmp = malloc (sizeof (char) * 1);
	tmp[0] = n[1];
	int num = atoi(tmp);
	
	srand(time(NULL));
	
	int numOfSS = (rand() % num) + 1;
	
	return numOfSS;
}

/*-----------------------------------------------------------------------------*/
/*    exists                                                                   */
/*-----------------------------------------------------------------------------*/
int exists(const char *fname) {
    FILE *file;
    if (file = fopen(fname, "r"))
    {
        fclose(file);
        return 1;
    }
    return 0;
}
