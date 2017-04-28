#ifndef VAULT_DATA_STRUCTURES_HEADER
#define VAULT_DATA_STRUCTURES_HEADER

#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>


#define MAX_CHARS_IN_FILE_NAME 257
#define MAX_NUM_FILES 100
#define NUM_DELIMITER_CHARS 8
#define START_OF_FILE_DELIMITER <
#define END_OF_FILE_DELIMITER >
#define SIZE_OF_BOTH_DELIMITERS sizeof(char)*NUM_DELIMITER_CHARS*2
#define MAX_BLOCKS_PER_FILE 3


#define REPO_META_DATA_SIZE sizeof(RepoMetaData)
#define FILE_META_DATA_SIZE sizeof(FileMetaData)
#define FILE_ALLOCATION_TABLE_SIZE sizeof(FileMetaData)*MAX_NUM_FILES
#define FILE_PATH_DELIMITER '/'
// TODO FILE_PATH_DELIMITER PER OS???

typedef struct RepoMetaData 
{
	ssize_t repositoryTotalSize;
	ssize_t sizeOfAllFilesInRepo;
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
	int isValidFile;
	char fileName[MAX_CHARS_IN_FILE_NAME];
	ssize_t fileSize;
//	st_mode fileProtection;
	time_t insertionDateStamp;
	DataBlock fileDataBlocks[MAX_BLOCKS_PER_FILE];
}FileMetaData;



#endif
