#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <libgen.h>

#include "initVault.h"
#include "vaultIO.h"
#include "vaultDataStructures.h"


#include "addFile.h"
// $./vault my_repository.vlt add /some/path/to/file_name
int addFile (int argc, char** argv)
{
	char* addedFilePath = argv[3];
//	printf("%s\n\n",filePath);
	char* vaultName = argv[1];

	if (argc != ADD_FILE_NUM_ARGUMENTS)
	{
		printf("Error: wrong number of arguments for add operation\n\n");
		return -1;
	}

	int newFileDescriptor = open(addedFilePath, O_RDONLY);
	if (newFileDescriptor < 0 )
	{
		printf("Error: could not open file\n\n");
		return -1;
	}

	struct stat newFileStat;
	if (fstat(newFileDescriptor, &newFileStat) < 0)
	{
		printf("Error: could not get the file stat\n\n");
		return -1;
	}

	int vaultFileDescriptor = openVault(vaultName);
	if (vaultFileDescriptor < 0)
	{
		("Error: failed opening vault for adding file\n\n");
		return -1;
	}

	RepoMetaData repoMetaData;
	FileMetaData *fileAllocationTable = malloc(FILE_ALLOCATION_TABLE_SIZE);
	if(fileAllocationTable == NULL)
		{
			printf("Failed Malloc\n");
			return -1;
		}
	readRepoMetaDataFromVault(&repoMetaData, vaultFileDescriptor);
	readFileAllocationTableFromVault(fileAllocationTable, vaultFileDescriptor);

	ssize_t addedFileSize = newFileStat.st_size;
	if(vaultHasEnoughSpaceForFile(addedFileSize, repoMetaData) < 0)
	{
		printf("Error: not enough space for file in vault\n\n");
		return -1;
	}

	char *addedFileName = getFileNameFromPath(addedFilePath);
//	printf("the file name is: %s", fileName);
	if(isUniqeFileName(addedFileName, fileAllocationTable, repoMetaData.numFilesInVault) < 0)
	{
		printf("Error: file with same name exists in vault\n\n");
		return -1;
	}

	if(sizeof(*addedFileName) > MAX_CHARS_IN_FILE_NAME)
	{
		printf("Error: filename too long\n\n");
		return -1;
	}


	if(naiveWriteFileToVault(newFileDescriptor, vaultFileDescriptor, repoMetaData) < 0)
	{
		printf("Error: failed writing file to vault\n\n");
		return -1;
	}

	updateRepoMetaDataAfterAddFile(&repoMetaData, addedFileSize);
//	printRepoMetaData(repoMetaData);

	DataBlock *addedFileDataBlocks = malloc(sizeof(DataBlock)*MAX_BLOCKS_PER_FILE);
	if(addedFileDataBlocks == NULL)
	{
		printf("Failed Malloc");
		return -1;
	}
	addedFileDataBlocks[0].blockAbsoluteOffset = REPO_META_DATA_SIZE + FILE_ALLOCATION_TABLE_SIZE + repoMetaData.sizeOfAllFilesInRepo
			- (addedFileSize + SIZE_OF_BOTH_DELIMITERS);
	addedFileDataBlocks[0].blockNumBytes = addedFileSize + SIZE_OF_BOTH_DELIMITERS;

//	printf("absolute offset: %zd\nnum bytes: %zd\n",
//			addedFileDataBlocks[0].blockAbsoluteOffset, addedFileDataBlocks[0].blockNumBytes);

	FileMetaData addedFileMetaData;
	createAddedFileMetaData (addedFileName, addedFileSize,
			repoMetaData.lastModificationTimeStamp, newFileStat.st_mode, 1, addedFileDataBlocks, &addedFileMetaData);
	memcpy(fileAllocationTable + (repoMetaData.numFilesInVault -1) , &addedFileMetaData, FILE_META_DATA_SIZE);
//	printFileAllocationTable(fileAllocationTable, repoMetaData.numFilesInVault);

	if (writeFileAllocationTableToVault(fileAllocationTable, vaultFileDescriptor) < 0)
	{
		printf("Error: failed writing FAT to fiel\n");
		return -1;
	}

	if (writeRepoMetaDataToVault(&repoMetaData, vaultFileDescriptor) < 0)
	{
		printf("Error: failed writing FAT to fiel\n");
		return -1;
	}

	close(newFileDescriptor);
	close(vaultFileDescriptor);
	free(fileAllocationTable);
	free(addedFileDataBlocks);
	printf("Result: %s inserted\n\n", addedFileName); // TODO uncomment
	return 1;
}

int vaultHasEnoughSpaceForFile(off_t fileSize, RepoMetaData repoMetaData)
{
	ssize_t catalogSize = REPO_META_DATA_SIZE + FILE_ALLOCATION_TABLE_SIZE;
	ssize_t remainingSpace = repoMetaData.repositoryTotalSize -
			(catalogSize + repoMetaData.sizeOfAllFilesInRepo);
	if(fileSize + SIZE_OF_BOTH_DELIMITERS > remainingSpace)
	{
		return -1;
	}
	else
	{
		return 1;
	}
}

char* getFileNameFromPath(char *filePath)
{
	char *base = basename(filePath);
//	printf("file name is: %s \nfile path was: %s\n", base, filePath);
	return base;
}

int isUniqeFileName(char *fileName, FileMetaData fileAllocationTable[MAX_NUM_FILES], int numFilesInVault)
{
	int i;
	for(i=0; i < numFilesInVault; i++)
	{
		if(strcmp(fileAllocationTable[i].fileName, fileName) == 0)
		{
			return -1;
		}
	}
	return 1;
}

int naiveWriteFileToVault(int newFileDescriptor, int vaultFileDescriptor, RepoMetaData repoMetaData)
{
	ssize_t absoluteOffsetInVault = REPO_META_DATA_SIZE + FILE_ALLOCATION_TABLE_SIZE + repoMetaData.sizeOfAllFilesInRepo;
	if(bufferedWriteFileToVault(newFileDescriptor, vaultFileDescriptor, absoluteOffsetInVault) < 0)
	{
		return -1;
	}
	return 1;
}

int updateRepoMetaDataAfterAddFile(RepoMetaData *repoMetaData, ssize_t addedFileSize)
{
	(*repoMetaData).lastModificationTimeStamp = time(NULL);
	(*repoMetaData).numFilesInVault++;
	(*repoMetaData).sizeOfAllFilesInRepo += addedFileSize + SIZE_OF_BOTH_DELIMITERS;
	return 1;
}

int createAddedFileMetaData(char addedFileName[MAX_CHARS_IN_FILE_NAME], ssize_t addedFileSize,
		time_t insertionTime, mode_t filePermissions, short numBlocksDividedInto ,DataBlock addedFileDataBlocks[MAX_BLOCKS_PER_FILE], FileMetaData *newFileMetaData)
{
	strcpy((*newFileMetaData).fileName, addedFileName);
	(*newFileMetaData).fileSize = addedFileSize;
	(*newFileMetaData).insertionDateStamp = insertionTime;
	(*newFileMetaData).fileProtection = filePermissions;
	(*newFileMetaData).isValidFile = 1;
	(*newFileMetaData).numValidBlocks = numBlocksDividedInto;
	int i;
	for(i=0; i < numBlocksDividedInto; i++)
	{
		(*newFileMetaData).fileDataBlocks[i].blockAbsoluteOffset = addedFileDataBlocks->blockAbsoluteOffset;
		(*newFileMetaData).fileDataBlocks[i].blockNumBytes = addedFileDataBlocks->blockNumBytes;
	}
	return 1;
}
