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



ssize_t writeRepoMetaDataToVault(RepoMetaData repoMetaData, int vaultFileDescriptor)
{
	// repo meta data starts at the beginning of the file
	if(lseek(vaultFileDescriptor, 0, SEEK_SET) != 0)
	{
		printf("Error: lseek failed\n\n");
		return -1;
	}

	ssize_t bytesWritten = write(vaultFileDescriptor, (void *)(&repoMetaData), sizeof(repoMetaData));
	if(bytesWritten < 0)
	{
		printf("Error: write failed\n\n");
		return -1;
	} else if (bytesWritten != sizeof(repoMetaData))
	{
		printf("Error: number of bytes written not equal to repo size\n\n");
		return -1;
	}

	return bytesWritten;
}

ssize_t getRepoMetaDataFromVault(RepoMetaData repoMetaData, int vaultFileDescriptor)
{
	if(lseek(vaultFileDescriptor, 0, SEEK_SET) != 0)
	{
		printf("Error: lseek failed\n\n");
		return -1;
	}
	ssize_t bytesRead = read(vaultFileDescriptor,(void *)(&repoMetaData), sizeof(repoMetaData));
	if(bytesRead < 0)
	{
		printf("Error: read failed\n\n %s\n\n", strerror(errno));
		return -1;
	} else if (bytesRead != sizeof(repoMetaData))
	{
		printf("Error: number of bytes read not equal to repo size\n\n");
		return -1;
	}

	return bytesRead;
}

