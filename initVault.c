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

	FileMetaData FileAllocationTable[MAX_NUM_FILES]; // TODO needed??

	// TODO refactor, so receives only ssize argument
	if(isRepositoryTotalSizeSufficient(repoMetaData, FileAllocationTable) == -1)
	{
		printf("Error: repository-total-size request not large enough\n\n");
		return -1;
	}

	char* vaultName = argv[1];
	int vaultFileDescriptor = createEmptyVaultFile(vaultName, repoMetaData.repositoryTotalSize);
	if(vaultFileDescriptor < 0 )
	{
		printf("Error: couldn't open the vault file\n\n");
		return -1;
	}

	if(writeRepoMetaDataToVault(&repoMetaData, vaultFileDescriptor) < 0)
	{
		printf("Error: failed to write RepoMetaData to vault\n\n");
		return -1;
	}

	close(vaultFileDescriptor);
	int fd = open( vaultName , O_RDWR , 0777);

	RepoMetaData testRepo;
	getRepoMetaDataFromVault(&testRepo, fd);

	if (areRepoMetaDataEqual(repoMetaData, testRepo) < 0)
	{
		printf("Big Balagan!");
	}
	close(fd);


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

// assumes repoMetaData and FAT are initialized
int isRepositoryTotalSizeSufficient(ssize_t repoSize)
{
	ssize_t catalogSize = (sizeof(RepoMetaData) + sizeof(*MAX_NUM_FILES);
	if(repoMetaData.repositoryTotalSize < catalogSize + MIN_BYTES_FOR_DATA)
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
		if (write(vaultFileDescriptor, END_OF_FILE_CHAR , 1) < 0)
		{
			printf("Error: write failed 1 \n\n");
			return -1;
		}
	}
	return 1;
}
