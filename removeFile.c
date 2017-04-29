#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


#include "removeFile.h"

int removeFile (int argc, char** argv)
{
	if (argc != REMOVE_FILE_NUM_ARGUMENTS)
	{
		printf("Error: wrong number of arguments for remove\n\n");
		return -1;
	}

	char *vaultName = argv[1];
	char *fileName = argv[3];

	int vaultFileDescriptor = openVault(vaultName);
	if (vaultFileDescriptor < 0)
	{
		printf("Error: failed to open vault\n\n");
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


	if (isUniqeFileName(fileName, fileAllocationTable, repoMetaData.numFilesInVault) > 0)
	{
		printf("Error: file name does not exist in vault\n\n");
		return -1;
	}

	if (deleteRemovedFileDelimitersFromVault(fileName, fileAllocationTable,
			repoMetaData.numFilesInVault, vaultFileDescriptor) < 0)
	{
		return -1;
	}

	ssize_t removedFileSize = getFileSize(fileAllocationTable, fileName, repoMetaData.numFilesInVault);
	updateRepoMetaDataAfterRemoveFile(&repoMetaData, removedFileSize);
	updateFileAllocationTableAfterRemoveFile(fileAllocationTable, fileName, repoMetaData.numFilesInVault);

	if (writeFileAllocationTableToVault(fileAllocationTable, vaultFileDescriptor) < 0)
	{
		printf("Error: failed writing FAT to fiel\n");
		return -1;
	}

	if (writeRepoMetaDataToVault(&repoMetaData, vaultFileDescriptor) < 0)
	{
		printf("Error: failed writing repoMetaData to file\n");
		return -1;
	}


	printf("Result: %s deleted\n\n", fileName);
	free(fileAllocationTable);
	close(vaultFileDescriptor);
	return 1;
}

int deleteRemovedFileDelimitersFromVault(char *fileName, FileMetaData *fileAllocationTable, short numFilesInVault,
		int vaultFileDescriptor)
{
	DataBlock *fileDataBlocks = malloc(sizeof(DataBlock)*MAX_BLOCKS_PER_FILE);
	int numValidBlocks;

	int i;
	for (i=0; i < numFilesInVault; i++)
	{
		if (strcmp(((fileAllocationTable + i)->fileName), fileName) == 0)
		{
			fileDataBlocks = (fileAllocationTable + i)->fileDataBlocks;
			numValidBlocks = (fileAllocationTable + i)->numValidBlocks;
			break;
		}
	}

	for (i=0; i<numValidBlocks; i++)
	{
		if(writeZerosOverDelimiter(fileDataBlocks[i].blockAbsoluteOffset, vaultFileDescriptor) < 0)
		{
			return -1;
		}
		if(writeZerosOverDelimiter(
				fileDataBlocks[i].blockAbsoluteOffset + fileDataBlocks[i].blockNumBytes - NUM_DELIMITER_CHARS*sizeof(char),
				vaultFileDescriptor) < 0)
		{
			return -1;
		}
	}

	return 1;
}

int writeZerosOverDelimiter(ssize_t offset, int vaultFileDescriptor)
{
	char* zeros = malloc(NUM_DELIMITER_CHARS*sizeof(char));
	if(zeros == NULL)
	{
		printf("Error: malloc failed during remove file\n\n");
		return -1;
	}
	int i;
	for (i=0; i < NUM_DELIMITER_CHARS; i++)
	{
		zeros[i] = END_OF_FILE_CHAR;
	}


	if (lseek(vaultFileDescriptor, offset, SEEK_SET) != offset)
	{
		printf("Error: seek failed during delete file\n\n");
		return -1;
	}

	if (write(vaultFileDescriptor, zeros, NUM_DELIMITER_CHARS) != NUM_DELIMITER_CHARS)
	{
		printf("Error: write during delete file failed\n\n");
		return -1;
	}

	free(zeros);
	return 1;
}

int updateFileAllocationTableAfterRemoveFile(FileMetaData *fileAllocationTable, char *fileName, short numFilesInVaultAfterDelete)
{
	int i;
	for (i=0; i < numFilesInVaultAfterDelete; i++)
	{
		if (strcmp(((fileAllocationTable + i)->fileName), fileName) == 0)
		{
			if(i != numFilesInVaultAfterDelete)
			{
				copyLastFileMetaDataOverFileMetaDataAtI(i,fileAllocationTable, numFilesInVaultAfterDelete);
			}
			fileAllocationTable[numFilesInVaultAfterDelete].isValidFile = 0;
			return 1;
		}
	}
	return -1;
}

void copyLastFileMetaDataOverFileMetaDataAtI(int i, FileMetaData *fileAllocationTable, short numFilesInVaultAfterDelete)
{
	memcpy(fileAllocationTable + i , fileAllocationTable + numFilesInVaultAfterDelete, FILE_META_DATA_SIZE);
}

int updateRepoMetaDataAfterRemoveFile(RepoMetaData *repoMetaData, ssize_t sizeOfRemovedFile)
{
	(*repoMetaData).numFilesInVault--;
	(*repoMetaData).lastModificationTimeStamp = time(NULL);
	(*repoMetaData).sizeOfAllFilesInRepo -= (sizeOfRemovedFile + NUM_DELIMITER_CHARS*2*sizeof(char));
	return 1;
}

ssize_t getFileSize(FileMetaData *fileAllocationTable, char *fileName, int numFilesInVault)
{
	int i;
	for (i=0; i < numFilesInVault; i++)
	{
		if (strcmp(((fileAllocationTable + i)->fileName), fileName) == 0)
		{
			return (fileAllocationTable + i)->fileSize;
		}
	}
	return -1;
}
