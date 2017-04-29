#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


#include "fetchFile.h"

// $./vault my_repository.vlt fetch some_file_name

int fetchFile (int argc, char** argv)
{
	if(argc != FETCH_FILE_NUM_ARGUMENTS)
	{
		printf("Error: wrong number of arguments for fetch\n\n");
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

	if (readRepoMetaDataFromVault(&repoMetaData, vaultFileDescriptor) < 0)
	{
		return -1;
	}

	if (readFileAllocationTableFromVault(fileAllocationTable, vaultFileDescriptor) < 0)
	{
		return -1;
	}


	if (isUniqeFileName(fileName, fileAllocationTable, repoMetaData.numFilesInVault) > 0)
	{
		printf("Error: file name does not exist in vault\n\n");
		return -1;
	}


	int newFileDescriptor = createFile(fileName);
	if (newFileDescriptor < 0)
	{
		printf("Error: couldn't create the file");
		return -1;
	}

	if (writeFileDataToFile(newFileDescriptor, vaultFileDescriptor, fileName, fileAllocationTable, repoMetaData.numFilesInVault) < 0)
	{
		return -1;
	}


	close(vaultFileDescriptor);
	free(fileAllocationTable);
	return 1;
}


int createFile(char *fileName)
{
	int newFileDescriptor = open(fileName , O_RDWR | O_CREAT | O_TRUNC, 0777);
	if ( newFileDescriptor < 0 )
	{
		printf("Error: couldn't open the vault file\n\n");
		return -1;
	}

	return newFileDescriptor;
}

int writeFileDataToFile(int fileDescriptor, int vaultFileDescriptor,char *fileName, FileMetaData *fileAllocationTable,
		int numFilesInVault)
{
	if (lseekToStartOfFile(fileDescriptor) < 0)
	{
		printf("Error: lseek failed\n\n");
		return -1;
	}
	int i;
	for (i=0; i < numFilesInVault; i++)
	{
		if (strcmp(((fileAllocationTable + i)->fileName), fileName) == 0)
		{
			int j;
			for (j=0; j < (fileAllocationTable + i)->numValidBlocks; j++)
			{
				DataBlock currentBlock = ((fileAllocationTable + i)->fileDataBlocks)[j];
				if (lseekToOffset(vaultFileDescriptor, currentBlock.blockAbsoluteOffset + (NUM_DELIMITER_CHARS*sizeof(char))) < 0)
				{
					printf("Error: lseek failed\n\n");
					return -1;
				}
				if (bufferedWriteFromFileToFile(vaultFileDescriptor,fileDescriptor,
						currentBlock.blockNumBytes - SIZE_OF_BOTH_DELIMITERS) < 0)
				{
					return -1;
				}
			}
		}
	}

	return 1;
}
