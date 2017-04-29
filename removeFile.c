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
		printf("Error: file name does not exist in vault");
		return -1;
	}

	if (zeroRemovedFileDelimiters() < 0)
	{
		return -1;
	}

	if ()

	free(fileAllocationTable);
	close(vaultFileDescriptor);
	return 1;
}
