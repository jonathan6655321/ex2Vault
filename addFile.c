
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <libgen.h>

#include "addFile.h"
#include "initVault.h"
#include "vaultIO.h"
#include "vaultDataStructures.h"


// $./vault my_repository.vlt add /some/path/to/file_name
int addFile (int argc, char** argv)
{
	char* addedFilePath = argv[3];
//	printf("%s\n\n",filePath);
	char* vaultName = argv[1];

	if (argc != ADD_FILE_NUM_ARGUMENTS)
	{
		printf("Error: wrong number of arguments for add operation\n\n");
		return -1;
	}

	int newFileDescriptor = open(addedFilePath, O_RDONLY);
	if (newFileDescriptor < 0 )
	{
		printf("Error: could not open file\n\n");
		return -1;
	}

	struct stat newFileStat;
	if (fstat(newFileDescriptor, &newFileStat) < 0)
	{
		printf("Error: could not get the file stat\n\n");
		return -1;
	}

	int vaultFileDescriptor = openVault(vaultName);
	if (vaultFileDescriptor < 0)
	{
		("Error: failed opening vault for adding file\n\n");
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
		printf("??????\n");
		return -1;
	}

	if (readFileAllocationTableFromVault(fileAllocationTable, vaultFileDescriptor) < 0)
	{
		printf("!!!!!!!!\n");
		return -1;
	}

//	printFileAllocationTable(fileAllocationTable, repoMetaData.numFilesInVault);

	ssize_t addedFileSize = newFileStat.st_size;
	if(vaultHasEnoughSpaceForFile(addedFileSize, repoMetaData) < 0)
	{
		printf("Error: not enough space for file in vault\n\n");
		return -1;
	}

	char *addedFileName = getFileNameFromPath(addedFilePath);
//	printf("the file name is: %s", fileName);
	if(isUniqeFileName(addedFileName, fileAllocationTable, repoMetaData.numFilesInVault) < 0)
	{
		printf("Error: file with same name exists in vault\n\n");
		return -1;
	}

	if(sizeof(*addedFileName) > MAX_CHARS_IN_FILE_NAME)
	{
		printf("Error: filename too long\n\n");
		return -1;
	}

	DataBlock *addedFileDataBlocks = malloc(sizeof(DataBlock)*MAX_BLOCKS_PER_FILE);
	if(addedFileDataBlocks == NULL)
	{
		printf("Failed Malloc\n");
		return -1;
	}


	int numBlocksFragmentedInto = writeFileToVault(newFileDescriptor, vaultFileDescriptor, repoMetaData,
			fileAllocationTable, addedFileDataBlocks, addedFileSize);
	if(numBlocksFragmentedInto < 0)
	{
		printf("Error: failed writing file to vault\n\n");
		return -1;
	}

	updateRepoMetaDataAfterAddFile(&repoMetaData, addedFileSize);

//	printf("****num files in vault: %d\n\n", repoMetaData.numFilesInVault);

//	printAllBlocks(addedFileDataBlocks, numBlocksFragmentedInto);

	FileMetaData addedFileMetaData;
	createAddedFileMetaData (addedFileName, addedFileSize,
			repoMetaData.lastModificationTimeStamp, newFileStat.st_mode, numBlocksFragmentedInto, addedFileDataBlocks, &addedFileMetaData);
	memcpy(fileAllocationTable + (repoMetaData.numFilesInVault -1) , &addedFileMetaData, FILE_META_DATA_SIZE);

//	printFileAllocationTable(fileAllocationTable, repoMetaData.numFilesInVault -1);

	if (writeFileAllocationTableToVault(fileAllocationTable, vaultFileDescriptor) < 0)
	{
		printf("Error: failed writing FAT to fiel\n");
		return -1;
	}

	if (writeRepoMetaDataToVault(&repoMetaData, vaultFileDescriptor) < 0)
	{
		printf("Error: failed writing FAT to fiel\n");
		return -1;
	}

	close(newFileDescriptor);
	close(vaultFileDescriptor);
	free(fileAllocationTable);
	free(addedFileDataBlocks);
	printf("Result: %s inserted\n\n", addedFileName); // TODO uncomment
	return 1;
}

int vaultHasEnoughSpaceForFile(off_t fileSize, RepoMetaData repoMetaData)
{
	ssize_t catalogSize = REPO_META_DATA_SIZE + FILE_ALLOCATION_TABLE_SIZE;
	ssize_t remainingSpace = repoMetaData.repositoryTotalSize -
			(catalogSize + repoMetaData.sizeOfAllFilesInRepo);
	if(fileSize + SIZE_OF_BOTH_DELIMITERS > remainingSpace)
	{
		return -1;
	}
	else
	{
		return 1;
	}
}

char* getFileNameFromPath(char *filePath)
{
	char *base = basename(filePath);
//	printf("file name is: %s \nfile path was: %s\n", base, filePath);
	return base;
}

int isUniqeFileName(char *fileName, FileMetaData fileAllocationTable[MAX_NUM_FILES], int numFilesInVault)
{
	int i;
	for(i=0; i < numFilesInVault; i++)
	{
		if(strcmp(fileAllocationTable[i].fileName, fileName) == 0)
		{
			return -1;
		}
	}
	return 1;
}


int writeFileToVault(int newFileDescriptor, int vaultFileDescriptor,
		RepoMetaData repoMetaData, FileMetaData *fileAllocationTable, DataBlock *addedFileDataBlocks, ssize_t addedFileSize)
{
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

	int numGaps = numBlocks+1;


	Gap *allGapsArray = malloc((numGaps)*sizeof(Gap)); // 1 gap between each block  and after/before edges
	if ( allGapsArray == NULL )
	{
		printf("Error: malloc failed\n\n");
	}

	initAllGapsArray(allGapsArray, allBlocksPointers, numBlocks, repoMetaData.repositoryTotalSize);


//	printGaps(allGapsArray, numBlocks);

	Gap **allGapsPointersArray = malloc((numGaps)*sizeof(Gap*)); // 1 gap between each block  and after/before edges
	if ( allGapsPointersArray == NULL )
	{
		printf("Error: malloc failed\n\n");
	}

	int i;
	for(i=0; i < numGaps; i++)
	{
		allGapsPointersArray[i] = &allGapsArray[i];
	}

	sortGapsPointersByGapSize(allGapsPointersArray, numGaps);


	for (i=0; i<MAX_BLOCKS_PER_FILE; i++)
	{
		(addedFileDataBlocks[i]).blockAbsoluteOffset = -1;
		(addedFileDataBlocks[i]).blockNumBytes = -1;
	}

	int numBlocksFragmentedInto = findGapsToWriteFileTo(allGapsPointersArray, addedFileDataBlocks, numGaps, addedFileSize);
	if (numBlocksFragmentedInto <= 0  || numBlocksFragmentedInto > MAX_BLOCKS_PER_FILE)
	{
		printf("Error: cannot insert file. Must fragment into to many parts\n\n");
		return -1;
	}


	if(lseekToStartOfFile(newFileDescriptor) < 0)
	{
		return -1;
	}


	for (i=0; i < numBlocksFragmentedInto; i++)
	{
		if (lseekToOffset(vaultFileDescriptor, (addedFileDataBlocks[i]).blockAbsoluteOffset) < 0)
		{
			return -1;
		}

		if (writeStartDelimiter(vaultFileDescriptor) < 0)
		{
			return -1;
		}

		if((bufferedWriteFromFileToFile(newFileDescriptor, vaultFileDescriptor,
				(addedFileDataBlocks[i]).blockNumBytes - SIZE_OF_BOTH_DELIMITERS))
				< 0)
		{
			return -1;
		}

		if (writeEndDelimiter(vaultFileDescriptor) < 0)
		{
			return -1;
		}
	}

	free(allGapsPointersArray);
	free(allBlocksPointers);
	return numBlocksFragmentedInto;
}

int updateRepoMetaDataAfterAddFile(RepoMetaData *repoMetaData, ssize_t addedFileSize)
{
	(*repoMetaData).lastModificationTimeStamp = time(NULL);
	(*repoMetaData).numFilesInVault++;
	(*repoMetaData).sizeOfAllFilesInRepo += addedFileSize + SIZE_OF_BOTH_DELIMITERS;
	return 1;
}

int createAddedFileMetaData(char addedFileName[MAX_CHARS_IN_FILE_NAME], ssize_t addedFileSize,
		time_t insertionTime, mode_t filePermissions, short numBlocksDividedInto ,DataBlock addedFileDataBlocks[MAX_BLOCKS_PER_FILE], FileMetaData *newFileMetaData)
{
	strcpy((*newFileMetaData).fileName, addedFileName);
	(*newFileMetaData).fileSize = addedFileSize;
	(*newFileMetaData).insertionDateStamp = insertionTime;
	(*newFileMetaData).fileProtection = filePermissions;
	(*newFileMetaData).isValidFile = 1;
	(*newFileMetaData).numValidBlocks = numBlocksDividedInto;
	int i;
	for(i=0; i < numBlocksDividedInto; i++)
	{
		(*newFileMetaData).fileDataBlocks[i].blockAbsoluteOffset = addedFileDataBlocks->blockAbsoluteOffset;
		(*newFileMetaData).fileDataBlocks[i].blockNumBytes = addedFileDataBlocks->blockNumBytes;
	}
	return 1;
}

void initAllGapsArray(Gap *allGapsArray, DataBlock **allBlocksPointersSortedByOffset, int numBlocks, ssize_t repoTotalSize)
{
	if(numBlocks == 0)
	{
		allGapsArray[0].gapNumBytes = repoTotalSize - (REPO_META_DATA_SIZE + FILE_ALLOCATION_TABLE_SIZE);
		allGapsArray[0].gapAbsoluteOffset = (REPO_META_DATA_SIZE + FILE_ALLOCATION_TABLE_SIZE);
		return;
	}
	// first gap:
	allGapsArray[0].gapNumBytes = (*allBlocksPointersSortedByOffset[0]).blockAbsoluteOffset - (REPO_META_DATA_SIZE + FILE_ALLOCATION_TABLE_SIZE);
	allGapsArray[0].gapAbsoluteOffset = (REPO_META_DATA_SIZE + FILE_ALLOCATION_TABLE_SIZE);

	// mid gaps:
	int i;
	ssize_t prevBlockEndsAt;
	ssize_t currentBlockStartsAt;
	for (i=1; i < numBlocks; i++)
	{
		prevBlockEndsAt = ((*allBlocksPointersSortedByOffset[i-1]).blockAbsoluteOffset + (*allBlocksPointersSortedByOffset[i-1]).blockNumBytes);
		currentBlockStartsAt = (*allBlocksPointersSortedByOffset[i]).blockAbsoluteOffset;
		allGapsArray[i].gapNumBytes = currentBlockStartsAt - prevBlockEndsAt;
		allGapsArray[i].gapAbsoluteOffset = prevBlockEndsAt;
	}

	// end gap:
	allGapsArray[numBlocks].gapNumBytes = repoTotalSize -
			((*allBlocksPointersSortedByOffset[numBlocks-1]).blockAbsoluteOffset + (*allBlocksPointersSortedByOffset[numBlocks-1]).blockNumBytes);

	allGapsArray[numBlocks].gapAbsoluteOffset =
			((*allBlocksPointersSortedByOffset[numBlocks-1]).blockAbsoluteOffset + (*allBlocksPointersSortedByOffset[numBlocks-1]).blockNumBytes);
}

void sortGapsPointersByGapSize(Gap **allGapsPointers, int numGaps)
{
	if (numGaps <= 1)
	{
		return;
	}

	Gap *tempGapPointer;
	int i;
	int j;
	for (i=1; i < numGaps; i++)
	{
		j = i;
		while ( j >= 1 && (*allGapsPointers[j]).gapNumBytes < (*allGapsPointers[j-1]).gapNumBytes)
		{
			tempGapPointer = allGapsPointers[j-1];
			allGapsPointers[j-1] = allGapsPointers[j];
			allGapsPointers[j] = tempGapPointer;
			j--;
		}
	}
}

// returns number of blocks fragmented into, or -1 if cannot insert
// inits addedFileDataBlocks to hold correct values for writing
int findGapsToWriteFileTo(Gap **allGapsPointersArray,DataBlock *addedFileDataBlocks, int numGaps, ssize_t addedFileSize)
{
	ssize_t remainingBytesToAssignBlocks = addedFileSize;


	int i;
	int j;
	for (i=0; i<MAX_BLOCKS_PER_FILE; i++)
	{
		// -i is because if we iterate on another i we have already assigned data to the max gap
		for (j=0; j < numGaps - i; j++)
		{
//			printf("In Gap Number %d\n\n", j);
//			printf("Size of current gap: %zd\n\n",  (*allGapsPointersArray[j]).gapNumBytes - SIZE_OF_BOTH_DELIMITERS);
//			printf("remaingin bytes to assign blocks: %zd\n\n", remainingBytesToAssignBlocks);

			if (((*allGapsPointersArray[j]).gapNumBytes) <= SIZE_OF_BOTH_DELIMITERS)
			{
				continue;
			}
			if(remainingBytesToAssignBlocks <= ((*allGapsPointersArray[j]).gapNumBytes - SIZE_OF_BOTH_DELIMITERS))
			{
//				printf("Did this happer???\n\n");
				(addedFileDataBlocks[i]).blockAbsoluteOffset = (*allGapsPointersArray[j]).gapAbsoluteOffset;
				(addedFileDataBlocks[i]).blockNumBytes = remainingBytesToAssignBlocks + SIZE_OF_BOTH_DELIMITERS;
				return i + 1;
			}
		}
		// reached end, with no gap large enough for remaining data
		// assign as much as possible to biggest gap and try again with another iteration
		// if this is the last iteration you cannot add the file with this number of blocks!
		if (i == MAX_BLOCKS_PER_FILE -1)
		{
			return -1;
		}
		else
		{
			(addedFileDataBlocks[i]).blockAbsoluteOffset = (*allGapsPointersArray[j]).gapAbsoluteOffset;
			(addedFileDataBlocks[i]).blockNumBytes = (*allGapsPointersArray[j]).gapNumBytes;
			remainingBytesToAssignBlocks -= ((*allGapsPointersArray[j]).gapNumBytes - SIZE_OF_BOTH_DELIMITERS) ;
		}
	}
	return i + 1;
}

void printGaps( Gap *allGapsArray, int numBlocks)
{
	int i;
	for (i =0; i < numBlocks + 1; i++)
	{
		printf("Gap number %d offset is %zd and size is: %zd\n\n", i, allGapsArray[i].gapAbsoluteOffset, allGapsArray[i].gapNumBytes);
	}
}
