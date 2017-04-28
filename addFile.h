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


#define ADD_FILE_NUM_ARGUMENTS 4
// $./vault my_repository.vlt add /some/path/to/file_name



int addFile (int argc, char** argv);
int vaultHasEnoughSpaceForFile(off_t fileSize, RepoMetaData repoMetaData);
char* getFileNameFromPath(char* filePath);

#endif
