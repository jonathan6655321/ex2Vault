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

#define ADD_FILE_NUM_ARGUMENTS 4
// $./vault my_repository.vlt add /some/path/to/file_name



int addFile (int argc, char** argv);
int vaultHasEnoughSpaceForFile(off_t fileSize, RepoMetaData repoMetaData);
char* getFileNameFromPath(char* filePath);
int isUniqeFileName(char *fileName, FileMetaData fileAllocationTable[MAX_NUM_FILES], int numFilesInVault);
int naiveWriteFileToVault(int newFileDescriptor, int vaultFileDescriptor ,
		RepoMetaData repoMetaData);
int updateRepoMetaDataAfterAddFile(RepoMetaData *repoMetaData, ssize_t addedFileSize);
int createAddedFileMetaData(char addedFileName[MAX_CHARS_IN_FILE_NAME], ssize_t addedFileSize,
		time_t insertionTime, mode_t filePermissions, short numBlocksDividedInto ,DataBlock addedFileDataBlocks[MAX_BLOCKS_PER_FILE], FileMetaData *newFileMetaData);


#endif
