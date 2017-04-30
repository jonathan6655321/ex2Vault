#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>


#include "initVault.h"
#include "vaultIO.h"


#define INIT_NUM_ARGUMENTS 4
#define MIN_BYTES_FOR_DATA 0

int initVault (int argc, char **argv)
{
	if(argc != INIT_NUM_ARGUMENTS)
	{
		printf("Error: invalid number of arguments for init\n\n");
		return -1;
	}

	RepoMetaData repoMetaData;
	if(initRepoMetaData(&repoMetaData, argv) == -1)
	{
		printf("Error: couldn't initialize the repoMetaData\n\n");
		return -1;
	}

	if(isRepositoryTotalSizeSufficient(repoMetaData.repositoryTotalSize) == -1)
	{
		printf("Error: repository-total-size request not large enough\n\n");
		return -1;
	}

	char* vaultName = argv[1];
	int vaultFileDescriptor = createEmptyVaultFile(vaultName, repoMetaData.repositoryTotalSize);
	if(vaultFileDescriptor < 0 )
	{
		printf("Error: couldn't create the vault file\n\n");
		return -1;
	}

	if(writeRepoMetaDataToVault(&repoMetaData, vaultFileDescriptor) < 0)
	{
		printf("Error: failed to write RepoMetaData to vault\n\n");
		return -1;
	}

	FileMetaData *fileAllocationTable = malloc(FILE_META_DATA_SIZE * MAX_NUM_FILES);
	initFileAllocationTable(fileAllocationTable);

	if(writeFileAllocationTableToVault(fileAllocationTable, vaultFileDescriptor) < 0)
	{
		printf("Error: failed to write fileAllocationTable to vault\n\n");
		return -1;
	}

	free(fileAllocationTable);
	close(vaultFileDescriptor);

	printf("Result: A vault created\n");
	return 1;
}

int initRepoMetaData(RepoMetaData *repoMetaData, char** argv)
{
	(*repoMetaData).repositoryTotalSize = parseNumberOfBytesFromFormat(argv[3]);
	if((*repoMetaData).repositoryTotalSize <= 0)
	{
		printf("Error: invalid vault size argument\n\n");
		return -1;
	}

	(*repoMetaData).creationTimeStamp = (*repoMetaData).lastModificationTimeStamp
			= time(NULL);

	(*repoMetaData).sizeOfAllFilesInRepo = 0;
	(*repoMetaData).numFilesInVault = 0;

	return 1;
}

int isRepositoryTotalSizeSufficient(ssize_t repoSize)
{
	ssize_t catalogSize = (REPO_META_DATA_SIZE + FILE_ALLOCATION_TABLE_SIZE);
	if(repoSize < catalogSize + MIN_BYTES_FOR_DATA)
	{
		return -1;
	} else
	{
		return 1;
	}
}

int createEmptyVaultFile(char* vaultName, ssize_t vaultSize)
{
	int vaultFileDescriptor = open(vaultName , O_RDWR | O_CREAT | O_TRUNC, 0777);
	if ( vaultFileDescriptor < 0 )
	{
		printf("Error: couldn't open the vault file\n\n");
		return -1;
	}
	else
	{
		if (lseek(vaultFileDescriptor, vaultSize, SEEK_SET) < 0 ) // TODO does write to exactly last location?
		{
			printf("Error: lseek failed 1 \n\n");
			return -1;
		}
		char * eof = END_OF_FILE_CHAR;
		if (write(vaultFileDescriptor, &eof , 1) < 0)
		{
			printf("Error: write failed 1 \n\n%s", strerror(errno));
			return -1;
		}
	}
	return vaultFileDescriptor;
}

int initFileAllocationTable(FileMetaData fileAllocationTable[MAX_NUM_FILES])
{
	int i;
	for (i = 0; i < MAX_NUM_FILES; i++)
	{
		fileAllocationTable[i].isValidFile = -1;
	}

	return 1;
}
