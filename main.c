#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "commands.h"


int main(int argc, char * argv[]){

	// first check the correct number of arguments are inserted in command line
	if (argc < 2)
	{
		printf("Exiting the program: Please provide an image file.\n");
		exit(EXIT_FAILURE);
	}

	char line[10000]; // for reading command line input
	char * imageFileName; // string for storing the file name
	TheImage * image = calloc(1, sizeof(*image));

	imageFileName = (char*)calloc(strlen(argv[1])+1,sizeof(char));  //read name of the input file
	strcpy(imageFileName, argv[1]);								// contains "fat32.img

	read_IMGFile(imageFileName, image);
	image->boot = read_Sector(imageFileName);				//parse through the image file to read in boot sector info

	free(imageFileName);

	int root = Hex2Decimal(image->boot.RootClus, RootSize);    	// set root cluster as current cluster
	image->currCluster = root;

	int clusSize[Num_Clusters(image)];
	Read_FATRegion(image, clusSize);

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

void prompt(TheImage * image)	// - Prints the prompt for user input
{
	char prompt[100];
	prompt[0] = '\0';
	int i = 0;
	for(i=0; i < image->currDepth; i++)
	{
		strcat(prompt, "/");
		strcat(prompt, image->currDir[i]);
	}
	printf("%s>", prompt);
}

void readAndDetermine(char * usrInput, TheImage * image) // - reads user input and runs command
{
	char tokens[100][100];
	int numOfToks;
    usrInput[strlen(usrInput)-1] = '\0';
	seperateBySpace(usrInput, &numOfToks, tokens);

	// Check what our command is and run
	if(strcmp(tokens[0], "exit") == 0)
	{
		printf("You want to run the 'exit' command with %d arguments\n", numOfToks-1);
   		free(image);	//free image file
   		exit(0);
	}
	else if(strcmp(tokens[0], "info") == 0)  //get BPB information from image file and prints it in decimal value
	{
		printf("You want to run the 'info' command with %d arguments\n", numOfToks-1);
		get_info(image);
	}
	else if(strcmp(tokens[0], "size") == 0)
	{
		printf("You want to run the 'size' command with %d arguments\n", numOfToks-1);
		if(numOfToks == 2)
		{
			show_size(image, tokens);
		}
		else
		{
			printf("Invalid command entry. Too many/little arguments provided.\nUsage: size [FILENAME]\n");
		}
	}
	else if(strcmp(tokens[0], "ls") == 0)
	{
		printf("You want to run the 'ls' command with %d arguments\n", numOfToks-1);
		if(numOfToks==2)
		{
		    show_ls(image, tokens);
		}
		else
			show_ls(image,tokens);

	}
	else if(strcmp(tokens[0], "cd") == 0)
	{
		printf("You want to run the 'cd' command with %d arguments\n", numOfToks-1);
		if(strcmp(tokens[1],".") == 0)
		{
			//do nothing
		}
		else if(numOfToks == 2)
		{
			do_cd(image, tokens);
		}
		else if(numOfToks == 1)
		{
			//do nothing
		}
		else{
			printf("Invalid command entry. Too many/little arguments provided.\nUsage: cd [DIRECTORY]\n");
		}
	}
	else if(strcmp(tokens[0], "creat") == 0)
	{
		printf("You want to run the 'creat' command with %d arguments\n", numOfToks-1);
		if(numOfToks ==2)
		{
			creat_command(image, tokens);
		}
		else
		{
			printf("Error: No filename provided.\n");  //checks if there was a filename provided

		}
	}
	else if(strcmp(tokens[0], "mkdir") == 0)
	{
		printf("You want to run the 'mkdir' command with %d arguments\n", numOfToks-1);
		if(numOfToks == 2)
		{
			make_Dir(image, tokens);
		}
		else
		{
			printf("Error: Too many arugments or missing directory name\n");
		}
	}
	else if(strcmp(tokens[0], "mv") == 0)
	{
		printf("You want to run the 'mv' command with %d arguments\n", numOfToks-1);
	}
	else if(strcmp(tokens[0], "open") == 0)
	{
		printf("You want to run the 'open' command with %d arguments\n", numOfToks-1);
		if(numOfToks ==3)
		{
			open_file(image, tokens);
		}
		else
		{
			printf("Error: Too few or too many arguments.\n");  //checks if there was a filename provided

		}

	}
	else if(strcmp(tokens[0], "close") == 0)
	{
		printf("You want to run the 'close' command with %d arguments\n", numOfToks-1);
		if(numOfToks == 2)
		{
			close_file(image, tokens);
		}
		else
		{
			printf("Improper command entry. Usage: close [FILENAME]\n");
		}
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
	int i;
	for(i = 0; i < 99; i++){
	tokens[i][0] = '\0';
	}
}
