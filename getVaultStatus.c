#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


#include "getVaultStatus.h"

int getVaultStatus (int argc, char** argv)
{
	if(argc != GET_VAULT_STATUS_NUM_ARGUMENTS)
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

	double fragRatio = getFragmentationRatio(fileAllocationTable,
			repoMetaData.numFilesInVault, repoMetaData.sizeOfAllFilesInRepo);

	printf("%-25s%d\n", "Number of files:" ,repoMetaData.numFilesInVault);
	printf("%-25s%zdB\n", "Total size:" , repoMetaData.sizeOfAllFilesInRepo);
	printf("%-25s%1.1f\n", "Fragmentation ratio:" , fragRatio);

	free(fileAllocationTable);
	close(vaultFileDescriptor);
	return 1;
}

double getFragmentationRatio(FileMetaData *fileAllocationTable, int numFilesInVault, ssize_t sizeOfAllFilesInRepo)
{
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

//	printAllBlocks(allBlocks, numBlocks);  // TODO might need this
	sortDataBlocksByOffset(allBlocks, numBlocks);


	int firstDelimiterToLastDelimiter =
			(allBlocks[numBlocks -1].blockAbsoluteOffset + allBlocks[numBlocks -1].blockNumBytes)
			- allBlocks[0].blockAbsoluteOffset;

	free(allBlocks);
	return ((double)(firstDelimiterToLastDelimiter - (sizeOfAllFilesInRepo - numBlocks*SIZE_OF_BOTH_DELIMITERS))
			/ firstDelimiterToLastDelimiter);
}

int getNumBlocksInVault(FileMetaData *fileAllocationTable, int numFilesInVault)
{
	int numBlocks = 0;
	int i;
	for(i=0; i < numFilesInVault; i++)
	{
		numBlocks += (fileAllocationTable + i)->numValidBlocks;
	}
	return numBlocks;
}

int loadAllBlocks(DataBlock *allBlocks, FileMetaData *fileAllocationTable, int numFilesInVault)
{
	int currentBlockIndex = 0;
	int numFile;
	for (numFile = 0; numFile < numFilesInVault; numFile++)
	{
		int blockInFile;
		for(blockInFile=0; blockInFile < fileAllocationTable[numFile].numValidBlocks; blockInFile ++)
		{
			allBlocks[currentBlockIndex] = fileAllocationTable[numFile].fileDataBlocks[blockInFile];
			currentBlockIndex++;
		}
	}

	return currentBlockIndex;
}

void sortDataBlocksByOffset(DataBlock *allBlocks, int numBlocks)
{
	DataBlock tempBlock;
	int i;
	int j;
	for (i=1; i < numBlocks; i++)
	{
		j = i;
		while (allBlocks[j].blockAbsoluteOffset < allBlocks[j-1].blockAbsoluteOffset && (j >= 1))
		{
			tempBlock = allBlocks[j-1];
			allBlocks[j-1] = allBlocks[j];
			allBlocks[j] = tempBlock;
			j--;
		}
	}
}

void printAllBlocks(DataBlock *allBlocks, int numBlocks)
{
	int i;
	for (i=0; i < numBlocks; i++)
	{
		printf("block %d offset: %zd size: %zd\n",i, allBlocks[i].blockAbsoluteOffset, allBlocks[i].blockNumBytes);
	}
}
