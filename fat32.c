#include "commands.h"
//---------------------------------------Functions only for reading Image File---------------------------------//

void read_IMGFile(const char * imageFileName, TheImage * image) {

	FILE * imageFile;
	int readSize;

	imageFile = fopen(imageFileName, "rb");
	if (!imageFile) {
		printf("ERROR: Unable to open image file\n");
		exit(1);
	}

								   //store the information from the image file into our struct
	fseek(imageFile, 0, SEEK_END); // read the file to the end to obtain the file size
    image->size = ftell(imageFile);

    image->buffer = (unsigned char*)calloc(image->size, sizeof(unsigned char)); //stores the content of the image file
    readSize = fread(image->buffer, sizeof(unsigned char), image->size, imageFile);

    image->numOfOpenFiles = 0;
    image->currDepth = 0;
    strcpy(image->filename, imageFileName);
}
BootSector read_Sector(const char * imageFileName) {

	BootSector boot;
	FILE * imageFile;

	imageFile = fopen(imageFileName, "rb");
	if (!imageFile) {
		printf("ERROR: Unable to open image file");
		exit(1);
	}

	//otain and store all BPB information using the offset and byte size from specification
	fseek(imageFile,11,SEEK_SET);
	fread(boot.BytsPerSec, sizeof(unsigned char),BytsSize, imageFile);

	fseek(imageFile,13,SEEK_SET);
	fread(boot.SecPerClus, sizeof(unsigned char),SecSize, imageFile);


	fseek(imageFile,14,SEEK_SET);
	fread(boot.RsvdSecCnt, sizeof(unsigned char),RsvdSize, imageFile);

	fseek(imageFile,16,SEEK_SET);
	fread(boot.NumFATs, sizeof(unsigned char),FATsSize, imageFile);

	fseek(imageFile,32,SEEK_SET);
	fread(boot.TotSec32, sizeof(unsigned char),Total32Size, imageFile);

	fseek(imageFile,36,SEEK_SET);
	fread(boot.FATSz32, sizeof(unsigned char),FAT32Size, imageFile);

	fseek(imageFile,44,SEEK_SET);
	fread(boot.RootClus, sizeof(unsigned char),RootSize, imageFile);

	return boot;
}

int Num_Clusters(const TheImage * image)
{
	//divie the size of the FAT Region by 4 bytes to obtain the # of clusters
	return FAT_Size(image)/4;
}

int FAT_Size(const TheImage * image) //obtain the size of the FAT Region
{
	//obtain the decimal value of the FATSz32, NumFATS, and BytsPerSec in the bootsector struct
	int BPB_FATSz32  = Hex2Decimal(image->boot.FATSz32, FAT32Size);
	int BPB_NumFATs  = Hex2Decimal(image->boot.NumFATs, FATsSize);
	int BPB_BytsPerSec = Hex2Decimal(image->boot.BytsPerSec, BytsSize);

	//multiply each value together to get the FAT region size
	int FATsize= BPB_FATSz32  * BPB_NumFATs * BPB_BytsPerSec;

	return FATsize;
}

void Read_FATRegion(const TheImage * image, int * FATRegion)
{
	unsigned char cluster_arr[4];

	int i=0;
	int k=0;
	int Fat_start=FAT_Index(image);
	int Data_start=DATA_Index(image);
	int size = 0;

	for (i = Fat_start; i < Data_start; i = i + 4)    //start from the begining of the FAT Region, until the begining of DATA Region
	{
		for (k = 0; k < 4; k++)  //read every 4 bytes and turn into decimal value
			cluster_arr[k] = image->buffer[i+k];

		FATRegion[size++] = Hex2Decimal(cluster_arr,4);
	}
}
int FAT_Index(const TheImage * image)
{
	// get the size of the reseved region and that is your start index for the Fat refion
	return RSRVD_Size(image);
}

int DATA_Index(const TheImage * image)
{
	// add the size of the Reserved region with FAT region to get the start index of the Data Region
	return RSRVD_Size(image) + FAT_Size(image);
}

int RSRVD_Size(const TheImage * image)  //obtain the size of the Reserved Region
{
	int BPB_BytsPerSec = Hex2Decimal(image->boot.BytsPerSec, BytsSize);
	int BPB_RsvdSecCnt  = Hex2Decimal(image->boot.RsvdSecCnt, RsvdSize);

	int RSRVDsize= BPB_BytsPerSec * BPB_RsvdSecCnt ;

	return RSRVDsize;
}
