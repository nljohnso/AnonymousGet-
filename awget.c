//Niklaus Johnson
//CS457
//Project 2
//awget.c

#include "awget.h"
#include "common.h"

/*-----------------------------------------------------------------------------*/
/*    main                                                                     */
/*-----------------------------------------------------------------------------*/
int main(int argc, char *argv[]) {

        Input input = validateInput(argc, argv);
	printf("awget:\n\tRequest: %s\n\t", input.URL);

	char ** fileLines = readFile(input.filename);	

	printf("chainlist is\n\t");
        for(int j = 1; fileLines[j] != NULL; j++) {
	    printf("%s\n\t", fileLines[j]);
	}

	Packet * aPacket = buildPacket(fileLines, input.URL);

	int randStepNum = getRandStepNum(aPacket->chainFile[0]); //Get random stepping stone number
	
	char * steppingStone = fileLines[randStepNum]; //Get random stepping stone

	printf("next SS is %s\n\t", steppingStone);

	//Connect to stepping stone
        int socketfd = setupSocket(getIP(steppingStone), atoi(getPort(steppingStone)));

	//Remove stepping stone from file
	removeSS(randStepNum, aPacket); 

	//Write to stepping stone
	socketfd = writeSocket(socketfd, aPacket);

	printf("waiting for file...\n..\n");	

	//Read the data
	char *filename = getFilename(input.URL);
	makeFile(socketfd, filename);
	printf("\tReceived file <%s>\n\tGoodbye!\n", filename);
	close(socketfd);
        free (aPacket);
}

/*-----------------------------------------------------------------------------*/
/*    makeFile                                                                 */
/*-----------------------------------------------------------------------------*/
void makeFile(int socketfd, char *filename) {

	FILE *file;
	file = fopen(filename, "w");
	
	char data[PACKET_SIZE];
	int n = 0;
	while((n = read(socketfd, &data, PACKET_SIZE)) > 0) {
            fwrite(data, 1, n, file);
	}
	fclose(file);
}

/*-----------------------------------------------------------------------------*/
/*    validateInput                                                            */ 
/*-----------------------------------------------------------------------------*/
Input validateInput(int argc, char *argv[]) {

        static Input input; 
	if(argc == 3) {
	    strcpy(input.URL, argv[1]);
	    strcpy(input.filename, argv[2]);
	} 
        else if(argc == 2) {
	    strcpy(input.URL, argv[1]);
	    strcpy(input.filename, "chaingang.txt");
	} 
        else {
	    fprintf(stderr, "ERROR incorrect arguments\n");
	    exit(1);
	}

	if(!exists(input.filename)) { //Check to see if the chainfile exists
	    fprintf(stderr, "ERROR, cannot locate file: %s\n", input.filename);
	    exit(1);
	}
       
        return input;
}

/*-----------------------------------------------------------------------------*/
/*    buildPacket                                                              */ 
/*-----------------------------------------------------------------------------*/
Packet * buildPacket(char ** fileLines, char * URL) {

	Packet *aPacket = malloc (sizeof (Packet));
	strcpy(aPacket->URL, URL);
	int i;
	for(i = 0; fileLines[i] != NULL; i++) {
		strcpy(aPacket->chainFile[i], fileLines[i]);
	}
	aPacket->fileSize = i;
	return aPacket;
}

/*-----------------------------------------------------------------------------*/
/*    readFile                                                                 */
/*-----------------------------------------------------------------------------*/
char ** readFile(const char *fname) {
	FILE *file;
	file = fopen(fname, "r");
	if(file == NULL) {
		return NULL;
	}
        char ** lines = malloc (sizeof (char *) * MAX_SS);
	char * line;
	size_t len = 0;
	ssize_t read;
	int i = 0;
	while(read = getline(&line, &len, file) > 0) {
                line[strlen(line)-1] = '\0';
                lines[i] = malloc (sizeof (char) * (strlen(line) +1)); 
                strcpy (lines[i], line);
                i++; 
	}
	//printf("%d\n", i);
	lines[i] = NULL;
	fclose(file);

	return lines;
}
