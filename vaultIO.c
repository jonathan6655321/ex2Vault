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

	if (writeStartDelimiter(vaultFileDescriptor)  != NUM_DELIMITER_CHARS)
	{
		printf("Error: failed to write to vault\n");
		return -1;
	}

	bufferedWriteFromFileToFile(newFileDescriptor, vaultFileDescriptor, fileSize);

	if (writeEndDelimiter(vaultFileDescriptor)  != NUM_DELIMITER_CHARS)
	{
		printf("Error: failed to write to vault\n");
		return -1;
	}

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
		printf("%-20s", (char*)((FileAllocationTable + i)->fileName)); // TODO alignment cool?
		ssize_t numBytes = (FileAllocationTable + i)->fileSize;
		int j=0;
		while (numBytes / KILO > 0)
		{
			numBytes /= KILO;
			j++;
		}
		char *sizeUnits = "BKMG";
		if(j < 0 || j > 3)
		{
			printf("Error: call me at 0523447550, this error should not have occured!");
			return ;
		}
		printf("%zd%-10c",numBytes,(char)sizeUnits[j]); //TODO notice this rounds out the data...
		printf("%-10o", (((FileAllocationTable + i)->fileProtection) &(S_IRWXU | S_IRWXG | S_IRWXO))); // TODO is drive thing too?
		printf("%-20s", asctime(localtime(&((FileAllocationTable + i)->insertionDateStamp))));
	}
}

void printRepoMetaData(RepoMetaData repoMetaData)
{
	printf("num files in vault: %d\n", repoMetaData.numFilesInVault);
	printf("size of all files in repo: %zd\n", repoMetaData.sizeOfAllFilesInRepo);
}

int writeStartDelimiter(int vaultFileDescriptor)
{
	char *startDelimiter = START_OF_FILE_DELIMITER;
	return write(vaultFileDescriptor, startDelimiter, 8);
}

int writeEndDelimiter(int vaultFileDescriptor)
{
	char *endDelimiter = END_OF_FILE_DELIMITER;
	return write(vaultFileDescriptor, endDelimiter, 8);
}

int bufferedWriteFromFileToFile(int fromFileDescriptor, int toFileDescriptor, ssize_t numBytesToWrite)
{
	int totalBytesWritten = 0;
	char *buffer = malloc(BUFFER_SIZE*sizeof(char));
	if (buffer == NULL)
	{
		printf("Error: malloc failed\n\n");
		return -1;
	}

	int bytesRead;
	int bytesWritten;
	while(totalBytesWritten < numBytesToWrite)
	{
		if(numBytesToWrite - totalBytesWritten < BUFFER_SIZE)
		{
			bytesRead = read(fromFileDescriptor, buffer, numBytesToWrite - totalBytesWritten);
		}
		else
		{
			bytesRead = read(fromFileDescriptor, buffer, BUFFER_SIZE);
		}
		if(bytesRead < 0)
		{
			printf("Error: read failed\n\n");
			return -1;
		}

		bytesWritten = write(toFileDescriptor, buffer, bytesRead);
		if(bytesRead < 0)
		{
			printf("Error: write failed\n\n");
			return -1;
		}

		totalBytesWritten += bytesWritten;
	}

	free(buffer);

	if (totalBytesWritten != numBytesToWrite)
	{
		return -1;
	}
	else
	{
		return 1;
	}
}
