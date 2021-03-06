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
    rewind(imageFile);            // rewind to file start

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

int Cluster_Size(const TheImage * image)
{
   return (Hex2Decimal(image->boot.BytsPerSec, BytsSize) * Hex2Decimal(image->boot.SecPerClus, SecSize));
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

   int i;
   int k;
   int Fat_start=FAT_Index(image);
   int Data_start=DATA_Index(image);
   int size = 0;

   for (i = Fat_start; i < Data_start; i = i + 4)    //start from the begining of the FAT Region, until the begining of DATA Region
   {
      for (k = 0; k < 4; k++)  //read every 4 bytes and turn into decimal value
      {
         cluster_arr[k] = image->buffer[i+k];

      }
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

void read_Entries_from_Dir(const TheImage * image, DirectoryEntry dirEntries[], int clusterNum, int *entryCount)
{
   if(clusterNum < 2)
      clusterNum = 2;

   if(clusterNum > (Num_Clusters(image)+2))
      clusterNum = (Num_Clusters(image)+2);

   int ass_clusters[200]; // [ass]ociated [clusters]
   find_Clusters_Associated(image, clusterNum, ass_clusters);
   *entryCount = 0;
   int i = 0;

   unsigned char tempcluster[Cluster_Size(image)];
   find_Cluster(image, ass_clusters[i], tempcluster);
   int j = 1;
   while(j <= 8)
   {
      DirectoryEntry temp = read_Entry(tempcluster, j);
      if(temp.attributes[0] != 0x00) // dir is empty
      {
         dirEntries[(*entryCount)++] = temp;
      }
      j++;
   }
   i++;
}
// get array of associated characters
void find_Clusters_Associated(const TheImage * image, int strtCluster, int * ass_clusters)
{
   if(strtCluster < 2)
      strtCluster = 2;
   if(strtCluster > (Num_Clusters(image)+2))
      strtCluster = (Num_Clusters(image)+2);

   int region[Num_Clusters(image)];
   int i = 0;
   Read_FATRegion(image, region);
   while(region[strtCluster] != 0 && region[strtCluster] < 0x0FFFF8)
   {
      ass_clusters[i++] = strtCluster;
      strtCluster = region[strtCluster];
   }
   ass_clusters[i++] = strtCluster;
   ass_clusters[i] = -1; // had to add this line because there needs to be an endpoint set
}
// function for getting the bytes from a cluster
void find_Cluster(const TheImage * image, int currClusterNum, unsigned char * theCluster)
{
   if(currClusterNum < 2)
      currClusterNum = 2;
   if(currClusterNum > ((Num_Clusters(image))+2))
      currClusterNum = ((Num_Clusters(image))+2);
   int index = DATA_Index(image) + (currClusterNum - 2) * Cluster_Size(image);
   int i;
   for(i = 0; i < Cluster_Size(image); i++)
   {
      theCluster[i] = image->buffer[index+i];
   }
}

DirectoryEntry read_Entry(const unsigned char * theCluster, int entryNum)
{
   DirectoryEntry entry;
   entryNum--;
   int index = 32 + (64 * (entryNum));
   memcpy(entry.dirName, theCluster + index, 11);
   index = index + 11;
   memcpy(entry.attributes, theCluster + index, 1);
   index = index + 1;
   memcpy(entry.ntres, theCluster + index, 1);
   index = index + 1;
   memcpy(entry.tenthSecCreated, theCluster + index, 1);
   index = index + 1;
   memcpy(entry.timeCreated, theCluster + index, 2);
   index = index + 2;
   memcpy(entry.dateCreated, theCluster + index, 2);
   index = index + 2;
   memcpy(entry.lastAccessed, theCluster + index, 2);
   index = index + 2;
   memcpy(entry.fstClusHI, theCluster + index, 2);
   index = index + 2;
   memcpy(entry.writeTime, theCluster + index, 2);
   index = index + 2;
   memcpy(entry.writeDate, theCluster + index, 2);
   index = index + 2;
   memcpy(entry.fstClusLO, theCluster + index, 2);
   index = index + 2;
   memcpy(entry.size, theCluster + index, 4);
   index = index + 4;
   return entry;
}

int get_AvailableCluster(const TheImage * image) //perform a linear search over the FAT until an empty clushter is found
{
   int FAT[Num_Clusters(image)]; // creat a FAT array with the num of clusters
   Read_FATRegion(image, FAT); // stores FAT region into int array

   int i = Hex2Decimal(image->boot.RootClus, RootSize);

   while(i< Num_Clusters(image)){
      if (FAT[i] == 0)
         return i;
      i++;
   }
   return -1;
}

DirectoryEntry create_DIRENTRY(const char * Filename, unsigned char attr, unsigned int availablecluster)
{
   DirectoryEntry dirEntry;
   int i=0;
   int fileSize;

   if(strlen(Filename) <= 11)
      fileSize= strlen(Filename);

   while(i < fileSize)
   {
      dirEntry.dirName[i] = (unsigned char) Filename[i];
      i++;
   }

   i=fileSize;
   while( i < 11)
   {
      dirEntry.dirName[i] = 0x00;
      i++;
   }

   dirEntry.attributes[0] = attr;         // Set DIR_Attr to ATTR_ARCHIVE
   dirEntry.ntres[0] = 0x00;              // Set DIR_NTRes, reserved.

   unsigned char bytes[4];
   bytes[0] = availablecluster & 0xFF;
   bytes[1] = (availablecluster >> 8) & 0xFF;
   bytes[2] = (availablecluster >> 16) & 0xFF;
   bytes[3] = (availablecluster >> 24) & 0xFF;

   dirEntry.fstClusLO[0] = bytes[0];      //Set DIR_FstClusLO nad DIR_FstClusHI after converting bytes to unsigned chars
   dirEntry.fstClusLO[1] = bytes[1];
   dirEntry.fstClusHI[0] = bytes[2];
   dirEntry.fstClusHI[1] = bytes[3];

   while(i<4)                             //Set DIR_FileSize =0;
   {
      dirEntry.size[i] = 0x00;
      i++;
   }

   return dirEntry;

}

void Update_FATEntry(const TheImage * image, int cluster, unsigned char * empty_clus)
{
   int FATindexpos = FAT_Index(image);
   int position = FATindexpos + cluster * 4; //obtain the correct position located in the buffer
   int i=0;

   while(i<4)
   {
      image->buffer[position+i] = empty_clus[i];   //insert )xFFFFFFFF
      i++;
   }
}
bool add_DIRENTRY(const TheImage * image, DirectoryEntry newEntry, int cluster)
{
   int i, j;
   int position = DATA_Index(image) + (cluster - 2) * Cluster_Size(image);//obtain the correct position located in the buffer

   // check for the first empty space
   int clusterPos = -1;
   for (i = 0; i < 8; i++)
   {
      for (j = 0; j < 64; j++)
      {
         if (image->buffer[position + j + (i * 64)] != 0x00)
            break;
         else if (j == 63)
            clusterPos = i;
      }
      if (clusterPos != -1) break;
   }

   position += (clusterPos * 64) + 32;

   for (i = 0; i <11; i++)
      image->buffer[position++] = newEntry.dirName[i];
   for (i = 0; i < 1; i++)
      image->buffer[position++] = newEntry.attributes[i];
   for (i = 0; i < 1; i++)
      image->buffer[position++] = newEntry.ntres[i];
   for (i = 0; i < 1; i++)
      image->buffer[position++] = newEntry.tenthSecCreated[i];
   for (i = 0; i < 2; i++)
      image->buffer[position++] = newEntry.timeCreated[i];
   for (i = 0; i < 2; i++)
      image->buffer[position++] = newEntry.dateCreated[i];
   for (i = 0; i < 2; i++)
      image->buffer[position++] = newEntry.lastAccessed[i];
   for (i = 0; i < 2; i++)
      image->buffer[position++] = newEntry.fstClusHI[i];
   for (i = 0; i < 2; i++)
      image->buffer[position++] = newEntry.writeTime[i];
   for (i = 0; i < 2; i++)
      image->buffer[position++] = newEntry.writeDate[i];
   for (i = 0; i < 2; i++)
      image->buffer[position++] = newEntry.fstClusLO[i];
   for (i = 0; i < 4; i++)
      image->buffer[position++] = newEntry.size[i];

   return true;

}
