/* All of the function declarations for all the functions of the program */
#ifndef COMMANDS_H
#define COMMANDS_H
// colored output
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_RESET "\x1b[0m"

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>


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

typedef struct {   //structure that represents the open file path
	char path[150];
	int firstCluster;
	int mode;
} OpenFile;

typedef struct{
	int numOfOpenFiles;
	BootSector boot;
	int size;
	int currCluster;
	char filename[100];
	char currDir[100][100];
	int currDepth;
	char * buffer;
	OpenFile openFiles[150];    //stores structure OpenFile in a list

} TheImage;

typedef struct{
   unsigned char dirName[11];
   unsigned char attributes[1];
   unsigned char ntres[1];
   unsigned char tenthSecCreated[1];
   unsigned char timeCreated[2];
   unsigned char dateCreated[2];
   unsigned char lastAccessed[2];
   unsigned char fstClusHI[2];
   unsigned char writeTime[2];
   unsigned char writeDate[2];
   unsigned char fstClusLO[2];
   unsigned char size[4];
} DirectoryEntry;

//---------------------------------------Functions for Main.cpp---------------------------------//
void prompt(TheImage * image);
void readAndDetermine(char * usrInput, TheImage * image);


//---------------------------------------Functions for Parsing Input---------------------------------//
void seperateBySpace(char * line, int * numOfToks, char newLines[100][100]);
int Hex2Decimal(const unsigned char * buffer, int bufferSize);
void Hex2ASCII(const unsigned char * buffer, int bufferSize, char * str);
int power(int base, int raise);
int compare2Directories(const void * a, const void * b);
int set_FileMode(char * permission);



//---------------------------------------Functions for Reading in the Image File---------------------------------//
void read_IMGFile(const char * imageFileName, TheImage * image) ;
BootSector read_Sector(const char * imageFileName);


//---------------------------------------Functions for getting data from the Image File---------------------------------//
int Num_Clusters(const TheImage * image);
int Cluster_Size(const TheImage * image);
int FAT_Size(const TheImage * image);
void Read_FATRegion(const TheImage * image, int * FATRegion);
int FAT_Index(const TheImage * image);
int DATA_Index(const TheImage * image);
int RSRVD_Size(const TheImage * image);


// //---------------------------------------Directory Entry Functions--------------------------------------------//
 void read_Entries_from_Dir(const TheImage * image, DirectoryEntry dirEntries[], int clusterNum, int *entryCount);
 void find_Clusters_Associated(const TheImage * image, int strtCluster, int * ass_clusters);
 void find_Cluster(const TheImage * image, int currClusterNum, unsigned char * theCluster);
 DirectoryEntry read_Entry(const unsigned char * theCluster, int entryNum);

// //---------------------------------------File Creation Functions--------------------------------------------//
DirectoryEntry create_DIRENTRY(const char * Filename, unsigned char attr, unsigned int availablecluster);
int get_AvailableCluster(const TheImage * image);
void Update_FATEntry(const TheImage * image, int cluster, unsigned char * empty_clus);
bool add_DIRENTRY(const TheImage * image, DirectoryEntry d, int cluster);


//---------------------------------------Functions for the 14 Input Commands---------------------------------//
void get_info(const TheImage * image);
void show_size(const TheImage * image, char tokens[100][100]);
void show_ls(const TheImage * image, char tokens[100][100]);
void do_cd(TheImage * image, char tokens[100][100]);
void creat_command(TheImage * image,char tokens[100][100]);
void make_Dir(TheImage * image,char tokens[100][100]);
bool open_file( TheImage * image, char tokens[100][100]);
void close_file(TheImage * image, char tokens[100][100]);
void read_file(TheImage * image, char tokens[100][100]);
void rm_file(TheImage * image, char tokens[100][100]);

#endif
