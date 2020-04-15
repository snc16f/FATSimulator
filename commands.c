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

int power(int base, int raise) {
	if (raise == 0)
		return 1;
	else
		return (base * power(base, raise-1));
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
