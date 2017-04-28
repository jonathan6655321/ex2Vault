#ifndef LIST_FILES_HEADER
#define LIST_FILES_HEADER

#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "initVault.h"
#include "vaultIO.h"
#include "vaultDataStructures.h"

#define LIST_FILES_NUM_ARGUMENTS 3

int listFiles (int argc, char** argv);

#endif
