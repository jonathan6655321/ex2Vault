#ifndef ADD_FILE_HEADER
#define ADD_FILE_HEADER

#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <libgen.h>


#include "initVault.h"
#include "vaultIO.h"
#include "vaultDataStructures.h"
#include "defragVault.h"

#define ADD_FILE_NUM_ARGUMENTS 4
// $./vault my_repository.vlt add /some/path/to/file_name

typedef struct Gap
{
 	ssize_t gapAbsoluteOffset;
	ssize_t gapNumBytes;
}Gap;


int addFile (int argc, char** argv);
int vaultHasEnoughSpaceForFile(off_t fileSize, RepoMetaData repoMetaData);
char* getFileNameFromPath(char* filePath);
int isUniqeFileName(char *fileName, FileMetaData fileAllocationTable[MAX_NUM_FILES], int numFilesInVault);
int writeFileToVault(int newFileDescriptor, int vaultFileDescriptor,
		RepoMetaData repoMetaData, FileMetaData *fileAllocationTable, DataBlock *addedFileDataBlocks, ssize_t addedFileSize);

int updateRepoMetaDataAfterAddFile(RepoMetaData *repoMetaData, ssize_t addedFileSize);
int createAddedFileMetaData(char addedFileName[MAX_CHARS_IN_FILE_NAME], ssize_t addedFileSize,
		time_t insertionTime, mode_t filePermissions, short numBlocksDividedInto ,DataBlock addedFileDataBlocks[MAX_BLOCKS_PER_FILE], FileMetaData *newFileMetaData);
void initAllGapsArray(Gap *allGapsArray, DataBlock **allBlocksPointersSortedByOffset, int numBlocks, ssize_t repoTotalSize);
void sortGapsPointersByGapSize(Gap **allGapsPointers, int numGaps);
int findGapsToWriteFileTo(Gap **allGapsPointersArray,DataBlock *addedFileDataBlocks, int numGaps, ssize_t addedFileSize);

void printGaps( Gap *allGapsArray, int numBlocks);

#endif
