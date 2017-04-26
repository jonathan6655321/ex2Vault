#ifndef VAULT_DATA_STRUCTURES_HEADER
#define VAULT_DATA_STRUCTURES_HEADER

#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>


#define MAX_CHARS_IN_FILE_NAME 256
#define MAX_NUM_FILES 100
#define START_OF_FILE_DELIMITER <<<<<<<<
#define END_OF_FILE_DELIMITER >>>>>>>>
#define MAX_BLOCKS_PER_FILE 3

#define KILO 1024
#define MEGA 1048576
#define GIGA 1073741824

typedef struct RepoMetaData 
{
	ssize_t repositoryTotalSize;
	time_t creationTimeStamp;
	time_t lastModificationTimeStamp;
	short numFilesInVault;
}RepoMetaData;

typedef struct DataBlock
{
	off_t blockAbsoluteOffset;
	ssize_t blockNumBytes;
}DataBlock;

typedef struct FileMetaData // AKA FileAllocationTableEntry
{
	char* fileName;
	ssize_t fileSize;
//	st_mode fileProtection;
	time_t insertionDateStamp;
	DataBlock fileDataBlocks[MAX_BLOCKS_PER_FILE];
}FileMetaData;



#endif
