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

#define BUFFER_SIZE 1024*4

int openVault(char *vaultName)
{
	return open(vaultName , O_RDWR);
}

ssize_t writeRepoMetaDataToVault(RepoMetaData *repoMetaData, int vaultFileDescriptor)
{
	// repo meta data starts at the beginning of the file
	if(lseekToStartOfFile(vaultFileDescriptor) < 0)
	{
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

ssize_t readRepoMetaDataFromVault(RepoMetaData *repoMetaData, int vaultFileDescriptor)
{
	if(lseekToStartOfFile(vaultFileDescriptor) < 0)
	{
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

ssize_t writeFileAllocationTableToVault(FileMetaData *fileAllocationTable, int vaultFileDescriptor)
{
	// file meta data starts immediately after the repo meta data
	if(lseek(vaultFileDescriptor, REPO_META_DATA_SIZE, SEEK_SET) < 0)
	{
		printf("Error: lseek failed\n\n");
		return -1;
	}

	ssize_t bytesWritten = write(vaultFileDescriptor, fileAllocationTable, FILE_ALLOCATION_TABLE_SIZE);
	if(bytesWritten < 0)
	{
		printf("Error: write failed\n\n");
		return -1;
	} else if (bytesWritten != FILE_ALLOCATION_TABLE_SIZE)
	{
		printf("Error: number of bytes written not equal to repo size\n\n");
		return -1;
	}

	return bytesWritten;
}


ssize_t readFileAllocationTableFromVault(FileMetaData *fileAllocationTable, int vaultFileDescriptor)
{
	// file meta data starts immediately after the repo meta data
	if(lseek(vaultFileDescriptor, REPO_META_DATA_SIZE, SEEK_SET) < 0)
	{
		printf("Error: lseek failed\n\n");
		return -1;
	}

	ssize_t bytesRead = read(vaultFileDescriptor, fileAllocationTable, FILE_ALLOCATION_TABLE_SIZE);
	if(bytesRead < 0)
	{
		printf("Error: write failed\n\n");
		return -1;
	} else if (bytesRead != FILE_ALLOCATION_TABLE_SIZE)
	{
		printf("Error: number of bytes written not equal to repo size\n\n");
		return -1;
	}

	return bytesRead;
}


int bufferedWriteFileToVault(int newFileDescriptor, int vaultFileDescriptor, ssize_t absoluteOffsetInVault)
{
	int totalBytesWritten = 0;
	char *buffer = malloc(BUFFER_SIZE*sizeof(char));

	if(lseekToStartOfFile(newFileDescriptor) < 0)
	{
		return -1;
	}

	if(lseekToOffset(vaultFileDescriptor, absoluteOffsetInVault) < 0)
	{
		return -1;
	}

	struct stat newFileStat;
	if (fstat(newFileDescriptor, &newFileStat) < 0)
	{
		printf("Error: could not get the file stat\n\n");
		return -1;
	}
	ssize_t fileSize = newFileStat.st_size;

	char *startDelimiter = "<<<<<<<<";
	write(vaultFileDescriptor, startDelimiter, 8);


	int bytesRead;
	int bytesWritten;
	while(totalBytesWritten < fileSize)
	{
		if(fileSize - totalBytesWritten < BUFFER_SIZE)
		{
			bytesRead = read(newFileDescriptor, buffer, fileSize - totalBytesWritten);
		}
		else
		{
			bytesRead = read(newFileDescriptor, buffer, BUFFER_SIZE);
		}
		if(bytesRead < 0)
		{
			printf("Error: read failed\n\n");
			return -1;
		}

		bytesWritten = write(vaultFileDescriptor, buffer, bytesRead);
		if(bytesRead < 0)
		{
			printf("Error: write failed\n\n");
			return -1;
		}

		totalBytesWritten += bytesWritten;
	}

	char *endDelimiter = ">>>>>>>>";
	write(vaultFileDescriptor, endDelimiter, 8);

	free(buffer);
	return 1;
}

int lseekToStartOfFile(int fileDescriptor)
{
	if(lseek(fileDescriptor, 0 , SEEK_SET) < 0)
	{
		printf("Error: lseek failed\n\n");
		return -1;
	}

	return 1;
}

int lseekToOffset(int fileDescriptor, ssize_t offset)
{
	if(lseek(fileDescriptor, offset , SEEK_SET) < 0)
	{
		printf("Error: lseek failed\n\n");
		return -1;
	}

	return 1;
}

void printFileAllocationTable(FileMetaData *FileAllocationTable, int numFilesInVault)
{
	int i;
	for(i=0; i < numFilesInVault; i = i+1)
	{
		printf("File Name: %s\n", (char*)((FileAllocationTable + i)->fileName));
	}
}

void printRepoMetaData(RepoMetaData repoMetaData)
{
	printf("num files in vault: %d\n", repoMetaData.numFilesInVault);
	printf("size of all files in repo: %zd\n", repoMetaData.sizeOfAllFilesInRepo);
}
