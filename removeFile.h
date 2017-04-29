#ifndef REMOVE_FILE_HEADER
#define REMOVE_FILE_HEADER

#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "initVault.h"
#include "vaultIO.h"
#include "vaultDataStructures.h"

#define REMOVE_FILE_NUM_ARGUMENTS 4

 //$./vault my_repository.vlt rm file_name
int removeFile (int argc, char** argv);
int deleteRemovedFileDelimitersFromVault(char *fileName, FileMetaData *fileAllocationTable, short numFilesInVault,
		int vaultFileDescriptor);
int writeZerosOverDelimiter(ssize_t offset, int vaultFileDescriptor);

int updateFileAllocationTableAfterRemoveFile(FileMetaData *fileAllocationTable, char *fileName, short numFilesInVault);
void copyLastFileMetaDataOverFileMetaDataAtI(int i, FileMetaData *fileAllocationTable, short numFilesInVault);

ssize_t getFileSize(FileMetaData *fileAllocationTable, char *fileName, int numFilesInVault);

#endif
