#ifndef FETCH_FILE_HEADER
#define FETCH_FILE_HEADER

#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "initVault.h"
#include "vaultIO.h"
#include "vaultDataStructures.h"

#define FETCH_FILE_NUM_ARGUMENTS 4

// $./vault my_repository.vlt fetch some_file_name
int fetchFile(int argc, char** argv);

int writeFileDataToFile(int fileDescriptor, int vaultFileDescriptor,char *fileName, FileMetaData *fileAllocationTable,
		int numFilesInVault);
#endif
