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

#endif
