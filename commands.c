/* All of the function definitions for the 14 main commands */
#include "commands.h"

//---------------------------------------Functions for Parsing Input---------------------------------//
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
int Hex2Decimal(const unsigned char * buffer, int bufferSize) {  //Little Endian: Convert from Hex to Decimal Value

	int i, decimal = 0;
	int values[bufferSize];

	for (i = 0; i < bufferSize; i++)
		values[i] = (int) buffer[i];
	for (i = 0; i < bufferSize; i++)
		decimal += values[i] * power(256, i);

	return decimal;
}

void Hex2ASCII(const unsigned char * buffer, int bufferSize, char * str)
{
   int i = 0;
   int j;
   char c;
   while(i < bufferSize)
   {
      j = (int) buffer[i];
      c = (char) j;
      str[i] = c;
      i++;
   }
   str[bufferSize] = '\0'; // must null terminate
}

int power(int base, int raise) {
	if (raise == 0)
		return 1;
	else
		return (base * power(base, raise-1));
}
//function to compare two strings, used by qsort
int compareDirs(const void * p1, const void * p2)
{
	DirectoryEntry * d1 = (DirectoryEntry *) p1;
	DirectoryEntry * d2 = (DirectoryEntry *) p2;
	return strcmp( d1 -> dirName, d2 -> dirName );
}


//---------------------------------------Functions for Input Commands---------------------------------//
void get_info(const TheImage * image)
{
	printf("BytsPerSec: %d\n", Hex2Decimal(image->boot.BytsPerSec, BytsSize));
	printf("SecPerClus: %d\n", Hex2Decimal(image->boot.SecPerClus, SecSize));
	printf("RsvdSecCnt: %d\n", Hex2Decimal(image->boot.RsvdSecCnt, RsvdSize));
	printf("NumFATs: %d\n", Hex2Decimal(image->boot.NumFATs, FATsSize));
	printf("TotSec32: %d\n", Hex2Decimal(image->boot.TotSec32, Total32Size));
	printf("FATSz32: %d\n", Hex2Decimal(image->boot.FATSz32, FAT32Size));
	printf("RootClus: %d\n", Hex2Decimal(image->boot.RootClus, RootSize));
}

void show_size(const TheImage * image, char tokens[100][100])
{
   DirectoryEntry dirEntries[100];
   int entryCount = 0;
   int index = -1;

   read_Entries_from_Dir(image , dirEntries, image->currCluster, &entryCount);
   int i = 0;
   while(i < entryCount)
   {
      char tmp_filename[12];
      Hex2ASCII(dirEntries[i].dirName, 11, tmp_filename);
      if(strcmp(tokens[1],tmp_filename) == 0 && dirEntries[i].attributes[0] == 0x20)
      {
         index = i;
         break;
      }
      i++;
   }

   if(index == -1) // index never changed so file isnt real
	{
		printf("Error: The filename you have provided is not an existing file\n");
		return;
	}

   int sizeOfFile;
   sizeOfFile = Hex2Decimal(dirEntries[index].size, 4);
   printf("The size of '%s' is - %d\n", tokens[1], sizeOfFile);

   entryCount = 0;
}

// print the name field for the directories within the contents of DIRNAME including the “.” and “..”
// directories. For simplicity, you may print each of the directory entries on separate lines
void show_ls(const TheImage * image, char tokens[100][100]) {

	DirectoryEntry dirEntries[150];
	int num_directories = 0;
	int i, j;
	int search = 0;

	// if no arguments, then use cwd
	if (strcmp(tokens[1],"") == 0) {
		read_Entries_from_Dir(image, dirEntries,image->currCluster, &num_directories);
	}
	else {

//Space error checking can be added

	// Looking for entered dirName
	int whereDir = -1;
	read_Entries_from_Dir(image, dirEntries,image->currCluster, &num_directories);
	for (i = 0; i < num_directories; i++) {
		char dirName[12];
		Hex2ASCII(dirEntries[i].dirName, 11, dirName);
		j = 0;
		while(j < strlen(dirName)){
			if(dirName[j] == ' ')
				dirName[j] = '\0';
				j++;
		}
		if (dirEntries[i].attributes[0] == 0x10 && strcmp(tokens[1],dirName) == 0) {
			whereDir = i;
			break;
		}
	}
	num_directories = 0;

	// dirName does not exist
	if (whereDir == -1) {
		printf("Directory %s can not be found\n", tokens[1]);
		search = -1;
	}

	//find the cluster of the dir
	unsigned char findClusterNum[4];
	for (i = 0; i < 2; i++)
		findClusterNum[i] = dirEntries[whereDir].fstClusLO[i];
	for (i = 0; i < 2; i++)
		findClusterNum[i+2] = dirEntries[whereDir].fstClusHI[i];

	int finalClusterNum = Hex2Decimal(findClusterNum, 4);

	read_Entries_from_Dir(image, dirEntries,finalClusterNum, &num_directories);
}

if(search == 0){
	DirectoryEntry sortDirectories[num_directories];

	for (i = 0; i < num_directories; i++)
		sortDirectories[i] = dirEntries[i];

	qsort(sortDirectories, num_directories, sizeof(const DirectoryEntry), compareDirs);

	// print out dir entry names
	printf("\n");
	for (i = 0; i < num_directories; i++) {
		if (sortDirectories[i].attributes[0] == 0x10)
			printf(ANSI_COLOR_MAGENTA);
		char name[11];
		Hex2ASCII(sortDirectories[i].dirName, 11, name);
		if (strcmp(name, "") == 0) continue; // ignore empty dir entries
		printf("%s\n", name);
		if (sortDirectories[i].attributes[0] == 0x10)
			printf(ANSI_COLOR_RESET);
	}
}
}

void do_cd(TheImage * image, char tokens[100][100]){

	DirectoryEntry dirEntries[150];
	int num_directories = 0;
	int i, j;
	bool search = 1;

	int whereDir = -1;
	read_Entries_from_Dir(image, dirEntries,image->currCluster, &num_directories);
	for (i = 0; i < num_directories; i++) {
		char dirName[12];
		Hex2ASCII(dirEntries[i].dirName, 11, dirName);
		j = 0;
		while(j < strlen(dirName)){
			if(dirName[j] == ' ')
				dirName[j] = '\0';
				j++;
		}
		if (dirEntries[i].attributes[0] == 0x10 && strcmp(tokens[1],dirName) == 0) {
			whereDir = i;
			break;
		}
	}
	num_directories = 0;

	// dirName does not exist
	if (whereDir == -1) {
		printf("Directory %s can not be found\n", tokens[1]);
		search = 0;
	}

if(search == 1){
	unsigned char findClusterNum[4];
	for (i = 0; i < 2; i++)
		findClusterNum[i] = dirEntries[whereDir].fstClusLO[i];
	for (i = 0; i < 2; i++)
		findClusterNum[i+2] = dirEntries[whereDir].fstClusHI[i];

	int finalClusterNum = Hex2Decimal(findClusterNum, 4);

	image->currCluster = finalClusterNum;

	if (image->currCluster <= Hex2Decimal(image->boot.RootClus, 4))
		image->currDepth = 0;
	else {
		if (strcmp(tokens[1],"..") != 0 && (strcmp(tokens[1],".")!= 0)) {
			char dirName[12];
			Hex2ASCII(dirEntries[whereDir].dirName, 11, dirName);
			j = 0;
			while(j < strlen(dirName)){
				if(dirName[j] == ' ')
					dirName[j] = '\0';
					j++;
			}
			strcpy(image->currDir[image->currDepth], dirName);
			image->currDepth++;
		}
		else if(strcmp(tokens[1],"..") == 0){
			image->currDepth--;
		}
	}}
}
