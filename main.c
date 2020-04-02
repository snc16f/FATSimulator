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

	// int i;
	// for(i = 0; i < numOfToks; i++)
	// {
	// 	printf("%s\n", tokens[i]);
	// }

	// Check what our command is and run
	if(strcmp(tokens[0], "exit") == 0)
	{
		printf("You want to run the 'exit' command with %d arguments\n", numOfToks-1);
    //free the image allocation
    exit(0);
	}
	else if(strcmp(tokens[0], "info") == 0)
	{
		printf("You want to run the 'info' command with %d arguments\n", numOfToks-1);
	}
	else if(strcmp(tokens[0], "size") == 0)
	{
		printf("You want to run the 'size' command with %d arguments\n", numOfToks-1);
	}
	else if(strcmp(tokens[0], "ls") == 0)
	{
		printf("You want to run the 'ls' command with %d arguments\n", numOfToks-1);
	}
	else if(strcmp(tokens[0], "cd") == 0)
	{
		printf("You want to run the 'cd' command with %d arguments\n", numOfToks-1);
	}
	else if(strcmp(tokens[0], "creat") == 0)
	{
		printf("You want to run the 'creat' command with %d arguments\n", numOfToks-1);
	}
	else if(strcmp(tokens[0], "mkdir") == 0)
	{
		printf("You want to run the 'mkdir' command with %d arguments\n", numOfToks-1);
	}
	else if(strcmp(tokens[0], "mv") == 0)
	{
		printf("You want to run the 'mv' command with %d arguments\n", numOfToks-1);
	}
	else if(strcmp(tokens[0], "open") == 0)
	{
		printf("You want to run the 'open' command with %d arguments\n", numOfToks-1);
	}
	else if(strcmp(tokens[0], "close") == 0)
	{
		printf("You want to run the 'close' command with %d arguments\n", numOfToks-1);
	}
	else if(strcmp(tokens[0], "read") == 0)
	{
		printf("You want to run the 'read' command with %d arguments\n", numOfToks-1);
	}
	else if(strcmp(tokens[0], "write") == 0)
	{
		printf("You want to run the 'write' command with %d arguments\n", numOfToks-1);
	}
	else if(strcmp(tokens[0], "rm") == 0)
	{
		printf("You want to run the 'rm' command with %d arguments\n", numOfToks-1);
	}
	else if(strcmp(tokens[0], "cp") == 0)
	{
		printf("You want to run the 'cp' command with %d arguments\n", numOfToks-1);
	}
	else if(strcmp(tokens[0], "rmdir") == 0)
	{
		printf("You want to run the 'rmdir' command with %d arguments\n", numOfToks-1);
	}
	else
	{
		printf("Command entered (%s) is not supported\n", tokens[0]);
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
