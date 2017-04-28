#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>


#include "vaultIO.h"

int openVault(char *vaultName)
{
	return open(vaultName , O_RDWR);
}

ssize_t writeRepoMetaDataToVault(RepoMetaData *repoMetaData, int vaultFileDescriptor)
{
	// repo meta data starts at the beginning of the file
	if(lseek(vaultFileDescriptor, 0, SEEK_SET) != 0)
	{
		printf("Error: lseek failed while writing repoMetaData to vault\n\n%s\n\n", strerror(errno));
		return -1;
	}

	ssize_t bytesWritten = write(vaultFileDescriptor, repoMetaData, REPO_META_DATA_SIZE);
	if(bytesWritten < 0)
	{
		printf("Error: write failed\n\n");
		return -1;
	} else if (bytesWritten != REPO_META_DATA_SIZE)
	{
		printf("Error: number of bytes written not equal to repo size\n\n");
		return -1;
	}

	return bytesWritten;
}

ssize_t getRepoMetaDataFromVault(RepoMetaData *repoMetaData, int vaultFileDescriptor)
{
	if(lseek(vaultFileDescriptor, 0, SEEK_SET) != 0)
	{
		printf("Error: lseek failed\n\n");
		return -1;
	}

	ssize_t bytesRead = read(vaultFileDescriptor, repoMetaData, REPO_META_DATA_SIZE);
	if(bytesRead < 0)
	{
		printf("Error: read failed\n\n %s\n\n", strerror(errno));
		return -1;
	} else if (bytesRead != REPO_META_DATA_SIZE)
	{
		printf("Error: number of bytes read not equal to repo size\n\n");
		return -1;
	}

	return bytesRead;
}

int areRepoMetaDataStructsEqual(RepoMetaData rep1, RepoMetaData rep2)
{
	int equal = 1;
	if(rep1.repositoryTotalSize != rep2.repositoryTotalSize)
	{
		printf("Not the SAME! 1");
		equal = -1;
	}
	if(rep1.creationTimeStamp != rep2.creationTimeStamp)
		{
			printf("Not the SAME! 2");
			equal = -1;
		}
	if(rep1.lastModificationTimeStamp != rep2.lastModificationTimeStamp)
		{
			printf("Not the SAME! 3");
			equal = -1;
		}
	if(rep1.numFilesInVault != rep2.numFilesInVault)
		{
			printf("Not the SAME! 4");
			equal = -1;
		}
	if(rep1.sizeOfAllFilesInRepo != rep2.sizeOfAllFilesInRepo)
			{
				printf("Not the SAME! 5");
				equal = -1;
			}

	return equal;
}

ssize_t writeFileAllocationTableToVault(FileMetaData *FileAllocationTable, int vaultFileDescriptor)
{
	ssize_t FileAllocationTableSize = FILE_META_DATA_SIZE*MAX_NUM_FILES;

	// file meta data starts immediately after the repo meta data
	if(lseek(vaultFileDescriptor, REPO_META_DATA_SIZE, SEEK_SET) < 0)
	{
		printf("Error: lseek failed\n\n");
		return -1;
	}

	ssize_t bytesWritten = write(vaultFileDescriptor, FileAllocationTable, FileAllocationTableSize);
	if(bytesWritten < 0)
	{
		printf("Error: write failed\n\n");
		return -1;
	} else if (bytesWritten != FileAllocationTableSize)
	{
		printf("Error: number of bytes written not equal to repo size\n\n");
		return -1;
	}

	return bytesWritten;
}


ssize_t readFileAllocationTableFromVault(FileMetaData *FileAllocationTable, int vaultFileDescriptor)
{
	ssize_t FileAllocationTableSize = FILE_META_DATA_SIZE*MAX_NUM_FILES;

	// file meta data starts immediately after the repo meta data
	if(lseek(vaultFileDescriptor, REPO_META_DATA_SIZE, SEEK_SET) < 0)
	{
		printf("Error: lseek failed\n\n");
		return -1;
	}

	ssize_t bytesRead = read(vaultFileDescriptor, FileAllocationTable, FileAllocationTableSize);
	if(bytesRead < 0)
	{
		printf("Error: write failed\n\n");
		return -1;
	} else if (bytesRead != FileAllocationTableSize)
	{
		printf("Error: number of bytes written not equal to repo size\n\n");
		return -1;
	}

	return bytesRead;
}
