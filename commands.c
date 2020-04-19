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
int compare2Directories(const void * a, const void * b)
{
	DirectoryEntry * d1 = (DirectoryEntry *) a;
	DirectoryEntry * d2 = (DirectoryEntry *) b;

	return strcmp( d1 -> dirName, d2 -> dirName );
}
int set_FileMode(char * permission)
{
	if(strcmp(permission, "r") == 0)
		 return 0;
	else if(strcmp(permission, "w") == 0)
		 return 1;
	else if(strcmp(permission, "rw") == 0)
		 return 2;
	else if(strcmp(permission, "wr") == 0)
		 return 3;
	else
		return -1;

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
		int j;
		j = 0;
		while(j < strlen(tmp_filename)){
			if(tmp_filename[j] == ' ')
				tmp_filename[j] = '\0';
				j++;
		}
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
   printf("The size of '%s' is: %d\n", tokens[1], sizeOfFile);

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

	qsort(sortDirectories, num_directories, sizeof(const DirectoryEntry), compare2Directories);

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

void creat_command(TheImage * image,char tokens[100][100]) {

	int clusters[100];
	DirectoryEntry entries[100];
	int FileCreated=0;
	int entryCount = 0;
	int i, j;
	bool error = 0;

	read_Entries_from_Dir(image, entries, image->currCluster, &entryCount);

	i=0;
	while(i<entryCount)
	{
		char fileName[11+1];
		Hex2ASCII(entries[i].dirName, 11, fileName);
		j = 0;
		while(j < strlen(fileName))
		{
			if(fileName[j] == ' ')
				fileName[j] = '\0';
				j++;
		}

		if (strcmp(tokens[1],fileName)==0) //check if the fileName is already listed as a directory
		{
			printf("Error: File %s already exists.\n",fileName);
			error = 1;
		}
		i++;
	}
	entryCount = 0; // reset this

	//allocate the first cluster of the file
	if(error == 0){
	int availableclus = get_AvailableCluster(image);

	//update the FAT entry to 0xFFFFFFFF
	unsigned char emptyCluster[] = {0xFF,0xFF,0xFF,0xFF};
	Update_FATEntry(image, availableclus, emptyCluster);

	char * newfileName = tokens[1];

	// Create a DIRENTRY using the newfile name passed in as an argument
	//set DIR_Attr to ATTR_ARCHIVE
	DirectoryEntry newDir = create_DIRENTRY(newfileName, 0x20, availableclus);
	find_Clusters_Associated(image, image->currCluster, clusters);

	//Store the DIRENTRY at the first empty space
	i = 0;
	while (clusters[i] != -1)
	{
		if (add_DIRENTRY(image,newDir,clusters[i]))
		{
			FileCreated = 1;
			break;
		}
		i++;
	}
	printf("File %s was successfully created.\n", tokens[1]);
	}

}

void make_Dir(TheImage * image,char tokens[100][100]){

	DirectoryEntry dirEntries[150];
	int num_directories = 0;
	int i, j;
	int clus[100];
	bool error = 0;

	//check if dir exists or illegal
	read_Entries_from_Dir(image, dirEntries, image->currCluster, &num_directories);
	for (i = 0; i < num_directories; i++) {
		char dirName[12];
		Hex2ASCII(dirEntries[i].dirName, 11, dirName);
		j = 0;
		while(j < strlen(dirName))
		{
			if(dirName[j] == ' ')
				dirName[j] = '\0';
				j++;
		}
		if (strcmp(tokens[1],dirName) == 0 || strcmp(tokens[1],".") == 0 || strcmp(tokens[1],"..") == 0) {
			printf("The directory %s already exists", tokens[1]);
			error = 1;
		}
	}
	num_directories = 0;

	if(error == 0){
	//allocate the first cluster of the dir
	int availableclus = get_AvailableCluster(image);

	//update the FAT entry to 0xFFFFFFFF
	unsigned char emptyCluster[] = {0xFF,0xFF,0xFF,0xFF};
	Update_FATEntry(image, availableclus, emptyCluster);

	char * newfileName = tokens[1];

	// Create a DIRENTRY using the newfile name passed in as an argument
	//set DIR_Attr to ATTR_ARCHIVE
	DirectoryEntry newDir = create_DIRENTRY(newfileName, 0x10, availableclus);
	find_Clusters_Associated(image, image->currCluster, clus);


	i = 0;
	while (clus[i] != -1) {
	if (add_DIRENTRY(image,newDir,clus[i])) {
		break;
	}
	i++;
	}

	DirectoryEntry curDir = create_DIRENTRY(".", 0x10, availableclus);
	add_DIRENTRY(image,curDir,availableclus);

	DirectoryEntry parDir = create_DIRENTRY("..", 0x10, image->currCluster);
	add_DIRENTRY(image,parDir,availableclus);

	printf("Directory %s was created\n", tokens[1]);
}

}
bool open_file( TheImage * image, char tokens[100][100])
{
	DirectoryEntry entries[100];
	int i,j, mode;
	int entryCount = 0;
	char file[100];


	strcpy(file, tokens[1]);
	mode= set_FileMode(tokens[2]); //set the mode of the file to be opened

	if(mode== -1)
	{
		printf("Error: There was an invalid mode entered.\n");
		return false;
	}

	while(i<image->numOfOpenFiles)
	{
		if (strcmp(file, image->openFiles[i].path) == 0) //search in open file list
		{
			printf( "Error: File is already open\n");  		//checks whether the file is already opened
			return false;
		}
		i++;

	}

	read_Entries_from_Dir(image, entries,image->currCluster, &entryCount);
	int index = -1;

	while(i < entryCount)
	{
		char fileName[11+1];
		Hex2ASCII(entries[i].dirName, 11, fileName);

		j = 0;
		while(j < strlen(fileName))
		{
			if(fileName[j] == ' ')
				fileName[j] = '\0';
				j++;
		}

		if (entries[i].attributes[0] == 0x20 && strcmp(file,fileName) == 0)
		{
			index = i;
			break;
		}
		i++;
	}

	if (index == -1)
	{
		printf("Error: Filename %s was not found.\n", file);
		return false;
	}

	unsigned char clusterNum[46];

	while(i<26)   //size of fstClusLO specification pdf
	{
		clusterNum[i] = entries[index].fstClusLO[i];
		i++;
	}

	while(i<20) //size of fstClusHI specification pdf
	{
		clusterNum[i+26] = entries[index].fstClusHI[i];
		i++;
	}

	int NumOfCluster = Hex2Decimal(clusterNum, 26+20);  //size of fstClusLO and fstClusHI from spec file

	strcpy(image->openFiles[image->numOfOpenFiles].path, file);  //add the file information to the openfiles structure

	image->openFiles[image->numOfOpenFiles].firstCluster = NumOfCluster;
	image->openFiles[image->numOfOpenFiles].mode = mode;
	image->numOfOpenFiles++;

	printf("File %s is now opened.\n", file);
	return true;
}
