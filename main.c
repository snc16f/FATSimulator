/* Reads in command from command line */
//#include "commands.h"
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <math.h>

typedef struct{
	//OpenFile ....
	int numOfOpenFiles;
	//BootSector ....
	int size;
	int currCluster;
	char filename[100];
	char currDir[100][100];
	int currDepth;
	char * buffer;
} TheImage;

//==============================================================================
//  Initialization
void seperateBySpace(char * line, int * numOfToks, char newLines[100][100]);
void prompt(TheImage * image);
void readAndDetermine(char * usrInput, TheImage * image);

//==============================================================================

int main(int argc, char * argv[]){

	// first check the correct number of arguments are inserted in command line
	if (argc < 2)
	{
		printf("Exiting the program: Please provide an image file.\n");
		exit(EXIT_FAILURE);
	}

	TheImage * image = calloc(1, sizeof(*image));


	printf("\n");
	prompt(image);
	char usrInput[300];
	while(fgets(usrInput,300,stdin))
	{
		readAndDetermine(usrInput, image);
		prompt(image);
	}

	return 1;
}
//end of main()
//==============================================================================

void prompt(TheImage * image)	// - Prints the prompt for user input
{
	char prompt[100];
	int i = 0;
	for(i=0; i < image->currDepth; i++)
	{
		strcat(prompt, "/");
		strcat(prompt, image->currDir[i]);
	}
	printf("%s>", prompt);
}

//==============================================================================

void readAndDetermine(char * usrInput, TheImage * image) // - reads user input and runs command
{
	char tokens[100][100];
	int numOfToks;
  usrInput[strlen(usrInput)-1] = '\0';
	seperateBySpace(usrInput, &numOfToks, tokens);

	int i;
	for(i = 0; i < numOfToks; i++)
	{
		printf("%s\n", tokens[i]);
	}
}

//==============================================================================

void seperateBySpace(char * line, int * numOfToks, char newLines[100][100]) // seperates a line
{                                                                           //into tokens using space as the key value
	char helper[strlen(line)+1];
	strcpy(helper, line);
	char * check = strtok(helper, " ");
	*numOfToks = 0;
	while(check != NULL)
	{
		strcpy(newLines[*numOfToks], check);
		check = strtok(NULL, " ");
		*numOfToks = *numOfToks + 1;
	}
}
