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

	char* repoName = argv[1];
	int vaultFileDescriptor = open( repoName , O_WRONLY | O_CREAT, 0777);
	if(vaultFileDescriptor == -1)
	{
		printf("Error: couldn't open the vault file\n\n");
		return -1;
	}

	RepoMetaData repoMetaData;
	if(initRepoMetaData(&repoMetaData, argv) == -1)
	{
		printf("Error: couldn't initialize the repoMetaData\n\n");
		return -1;
	}

	FileMetaData FileAllocationTable[MAX_NUM_FILES];

	if(isRepositoryTotalSizeSufficient(repoMetaData, FileAllocationTable) == -1)
	{
		printf("Error: repository-total-size request not large enough\n\n");
		return -1;
	}

	if(writeRepoMetaDataToVault(repoMetaData, vaultFileDescriptor) < 0)
	{
		printf("Error: failed to write RepoMetaData to vault\n\n");
		return -1;
	}

	close(vaultFileDescriptor);

	int fd = open( repoName , O_RDONLY , 0777);

	RepoMetaData testRepo;
	getRepoMetaDataFromVault(testRepo, fd);

	printf("new value: %zd \n\nold value: %zd", testRepo.repositoryTotalSize , repoMetaData.repositoryTotalSize);

	close(fd);
	if(testRepo.repositoryTotalSize != repoMetaData.repositoryTotalSize)
	{
		printf("total size wrong\n\n");
	}else
	{
		printf("my name a borat!\n\n");
	}




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
int isRepositoryTotalSizeSufficient(RepoMetaData repoMetaData, FileMetaData FileAllocationTable[MAX_NUM_FILES])
{
	ssize_t catalogSize = (sizeof(repoMetaData) + sizeof(FileAllocationTable[0])*MAX_NUM_FILES);
	if(repoMetaData.repositoryTotalSize < catalogSize + MIN_BYTES_FOR_DATA)
	{
		return -1;
	} else
	{
		return 1;
	}
}
