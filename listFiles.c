#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


#include "listFiles.h"

int listFiles (int argc, char** argv)
{
	if(argc != LIST_FILES_NUM_ARGUMENTS)
	{
		printf("Error: wrong number of arguments for list\n\n");
		return -1;
	}

	char *vaultName = argv[1];

	RepoMetaData repoMetaData;
	FileMetaData *fileAllocationTable = malloc(FILE_ALLOCATION_TABLE_SIZE);
	if(fileAllocationTable == NULL)
	{
		printf("Error: malloc failed\n\n");
		return -1;
	}

	int vaultFileDescriptor = openVault(vaultName);
	readFileAllocationTableFromVault(fileAllocationTable,vaultFileDescriptor);
	readRepoMetaDataFromVault(&repoMetaData, vaultFileDescriptor);

	printFileAllocationTable(fileAllocationTable, repoMetaData.numFilesInVault);
	printRepoMetaData(repoMetaData); // TODO comment this out!

	free(fileAllocationTable);
	close(vaultFileDescriptor);
	return 1;
}
