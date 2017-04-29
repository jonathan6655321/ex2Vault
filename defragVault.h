#ifndef DEFRAG_VAULT_HEADER
#define DEFRAG_VAULT_HEADER

#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "initVault.h"
#include "vaultIO.h"
#include "vaultDataStructures.h"

#define DEFRAG_VAULT_NUM_ARGUMENTS 3

int defragVault (int argc, char** argv);

#endif
