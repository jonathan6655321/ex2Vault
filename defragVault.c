#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


#include "defragVault.h"


int defragVault(int argc, char** argv)
{
	if(argc != DEFRAG_VAULT_NUM_ARGUMENTS)
	{
		printf("Error: wrong number of arguments for status\n\n");
		return -1;
	}

	char *vaultName = argv[1];

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

	int numFilesInVault = repoMetaData.numFilesInVault;
	int numBlocks = getNumBlocksInVault(fileAllocationTable, numFilesInVault);

	DataBlock *allBlocks = malloc(numBlocks*sizeof(DataBlock));
	if (allBlocks == NULL)
	{
		printf("Error: malloc failed\n\n");
		return -1;
	}

	if(loadAllBlocks(allBlocks, fileAllocationTable, numFilesInVault) != numBlocks)
	{
		printf("Error: this should not have happened...\n\n");
		return -1;
	}

	sortDataBlocksByOffset(allBlocks, numBlocks);

	if (defrag(vaultFileDescriptor, allBlocks, numBlocks) < 0)
	{
		printf("Error: defrag failed\n\n");
		return -1;
	}

	printf("Result: Defragmentation complete\n\n");
	free(allBlocks);
	free(fileAllocationTable);
	close(vaultFileDescriptor);
}

int writeBlockToOffsetMinusShift(ssize_t blockAbsoluteOffset, ssize_t totalAccumulatedGap, ssize_t blockNumBytes
				, int vaultFileDescriptor)
{

	int totalBytesWritten = 0;
	char *buffer = malloc(BUFFER_SIZE*sizeof(char));
	if (buffer == NULL)
	{
		printf("Error: malloc failed\n\n");
		return -1;
	}

	int bytesRead;
	int bytesWritten = 0;
	while(totalBytesWritten < blockNumBytes)
	{

		// set to read from original location
		if(lseek(vaultFileDescriptor, blockAbsoluteOffset + totalBytesWritten, SEEK_SET) < 0)
		{
			printf("Error: failed lseek!\n\n");
			return -1;
		}

		if(blockNumBytes - totalBytesWritten < BUFFER_SIZE)
		{
			bytesRead = read(vaultFileDescriptor, buffer, blockNumBytes - totalBytesWritten);
		}
		else
		{
			bytesRead = read(vaultFileDescriptor, buffer, BUFFER_SIZE);
		}
		if(bytesRead < 0)
		{
			printf("Error: read failed\n\n");
			return -1;
		}

		// set to write at original locatio minus shift
		if(lseek(vaultFileDescriptor, blockAbsoluteOffset + totalBytesWritten - totalAccumulatedGap, SEEK_SET) < 0)
		{
			printf("Error: failed lseek!\n\n");
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

	free(buffer);

	if (totalBytesWritten != blockNumBytes)
	{
		return -1;
	}
	else
	{
		return 1;
	}

	return 1;
}

int defrag(int vaultFileDescriptor, DataBlock  *allBlocks, int numBlocks)
{
	ssize_t totalAccumulatedGap = allBlocks[0].blockAbsoluteOffset - (FILE_META_DATA_SIZE + FILE_ALLOCATION_TABLE_SIZE);
	int i;
	ssize_t nextGap;

	for (i=0; i < numBlocks; i++)
	{
		if (i != numBlocks -1)
		{
			nextGap = allBlocks[i+1].blockAbsoluteOffset - (allBlocks[i].blockAbsoluteOffset + allBlocks[i].blockNumBytes);
		}
		else
		{
			nextGap = 0;
		}

		if (writeBlockToOffsetMinusShift(allBlocks[i].blockAbsoluteOffset, totalAccumulatedGap, allBlocks[i].blockNumBytes
				, vaultFileDescriptor) < 0)
		{
			return -1;
		}

		allBlocks[i].blockAbsoluteOffset -= totalAccumulatedGap;

		totalAccumulatedGap += nextGap;
	}

	return 1;
}
