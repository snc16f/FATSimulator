/* All of the function declarations for all the functions of the program */
#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <math.h>

// predefined variables for Boot Sector per Specification PDF file
#define BytsSize 2
#define SecSize 1
#define RsvdSize 2
#define FATsSize 1
#define Total32Size 4
#define FAT32Size 4
#define RootSize 4

typedef struct {
	unsigned char BytsPerSec[BytsSize];
	unsigned char SecPerClus[SecSize];
	unsigned char RsvdSecCnt[RsvdSize];
	unsigned char NumFATs[FATsSize];
	unsigned char TotSec32[Total32Size];
	unsigned char FATSz32[FAT32Size];
	unsigned char RootClus[RootSize];

} BootSector;

typedef struct{
	int numOfOpenFiles;
	BootSector boot;
	int size;
	int currCluster;
	char filename[100];
	char currDir[100][100];
	int currDepth;
	char * buffer;
} TheImage;

//---------------------------------------Functions for Main.cpp---------------------------------//
void prompt(TheImage * image);
void readAndDetermine(char * usrInput, TheImage * image);


//---------------------------------------Functions for Parsing Input---------------------------------//
void seperateBySpace(char * line, int * numOfToks, char newLines[100][100]);
int Hex2Decimal(const unsigned char * buffer, int bufferSize);
int power(int base, int raise);


//---------------------------------------Functions for Reading in the Image File---------------------------------//
void read_IMGFile(const char * imageFileName, TheImage * image) ;
BootSector read_Sector(const char * imageFileName);


//---------------------------------------Functions for getting data from the Image File---------------------------------//
int Num_Clusters(const TheImage * image);
int FAT_Size(const TheImage * image);
void Read_FATRegion(const TheImage * image, int * FATRegion);
int FAT_Index(const TheImage * image);
int DATA_Index(const TheImage * image);
int RSRVD_Size(const TheImage * image);


//---------------------------------------Functions for the 14 Input Commands---------------------------------//
void get_info(const TheImage * image);

#endif
