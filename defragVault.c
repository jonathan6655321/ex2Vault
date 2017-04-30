#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


#include "defragVault.h"

#define ZEROS "00000000"

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

	DataBlock **allBlocksPointers = malloc(numBlocks*sizeof(DataBlock*));
	if (allBlocksPointers == NULL)
	{
		printf("Error: malloc failed\n\n");
		return -1;
	}

	if(loadAllBlocksPointers(allBlocksPointers, fileAllocationTable, numFilesInVault) != numBlocks)
	{
		printf("Error: this should not have happened...\n\n");
		return -1;
	}

	sortDataBlocksPointersByOffset(allBlocksPointers, numBlocks);

	if (defrag(vaultFileDescriptor, allBlocksPointers, numBlocks) < 0)
	{
		printf("Error: defrag failed\n\n");
		return -1;
	}

	if (writeFileAllocationTableToVault(fileAllocationTable, vaultFileDescriptor) < 0)
	{
		printf("Error: failed writing FAT to file\n");
		return -1;
	}

	printf("Result: Defragmentation complete\n\n");
	free(allBlocksPointers);
	free(fileAllocationTable);
	close(vaultFileDescriptor);
	return 1;
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

int defrag(int vaultFileDescriptor, DataBlock  **allBlocksPointers, int numBlocks)
{
	ssize_t totalAccumulatedGap = (*allBlocksPointers[0]).blockAbsoluteOffset - (FILE_META_DATA_SIZE + FILE_ALLOCATION_TABLE_SIZE);
	int i;
	ssize_t nextGap;

	for (i=0; i < numBlocks; i++)
	{
		if (i != numBlocks -1)
		{
			nextGap = (*allBlocksPointers[i+1]).blockAbsoluteOffset
					- ((*allBlocksPointers[i]).blockAbsoluteOffset + (*allBlocksPointers[i]).blockNumBytes);
		}
		else
		{
			nextGap = 0;
		}

		if (writeBlockToOffsetMinusShift((*allBlocksPointers[i]).blockAbsoluteOffset, totalAccumulatedGap,
				(*allBlocksPointers[i]).blockNumBytes
				, vaultFileDescriptor) < 0)
		{
			return -1;
		}

		if(zeroDelimitersAtOffset((*allBlocksPointers[i]).blockAbsoluteOffset, vaultFileDescriptor) < 0)
		{
			printf("Error writing\n");
			return -1;
		}

		if(zeroDelimitersAtOffset((*allBlocksPointers[i]).blockAbsoluteOffset + (*allBlocksPointers[i]).blockNumBytes - NUM_DELIMITER_CHARS*sizeof(char) , vaultFileDescriptor) < 0)
		{
			printf("Error writing\n");
			return -1;
		}

		(*allBlocksPointers[i]).blockAbsoluteOffset -= totalAccumulatedGap;

		totalAccumulatedGap += nextGap;
	}

	return 1;
}

int loadAllBlocksPointers(DataBlock **allBlocksPointers, FileMetaData *fileAllocationTable, int numFilesInVault)
{
	int currentBlockIndex = 0;
	int numFile;
	for (numFile = 0; numFile < numFilesInVault; numFile++)
	{
		int blockInFile;
		for(blockInFile=0; blockInFile < fileAllocationTable[numFile].numValidBlocks; blockInFile ++)
		{
			allBlocksPointers[currentBlockIndex] = &(fileAllocationTable[numFile].fileDataBlocks[blockInFile]);
			currentBlockIndex++;
		}
	}

	return currentBlockIndex;
}

void sortDataBlocksPointersByOffset(DataBlock **allBlocksPointers, int numBlocks)
{
	if (numBlocks <= 1)
	{
		return;
	}


	DataBlock *tempBlockPointer;
	int i;
	int j;
	for (i=1; i < numBlocks; i++)
	{
		j = i;
		while ( j >= 1 && (*allBlocksPointers[j]).blockAbsoluteOffset < (*allBlocksPointers[j-1]).blockAbsoluteOffset)
		{
			tempBlockPointer = allBlocksPointers[j-1];
			allBlocksPointers[j-1] = allBlocksPointers[j];
			allBlocksPointers[j] = tempBlockPointer;
			j--;
		}
	}
}

int zeroDelimitersAtOffset(ssize_t offset, int vaultFileDescriptor)
{
	char *zeros = ZEROS;
	return write(vaultFileDescriptor, zeros, 8);
}
