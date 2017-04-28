#ifndef INIT_VAULT_HEADER
#define INIT_VAULT_HEADER

#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "vaultDataStructures.h"
#include "parseUserInput.h"
#include "vaultIO.h"

#define END_OF_FILE_CHAR '\0'

int initVault (int argc, char **argv);

int initRepoMetaData(RepoMetaData *repoMetaData, char** argv);
int isRepositoryTotalSizeSufficient(ssize_t repSize);
int createEmptyVaultFile(char* vaultName, ssize_t vaultSize);
int initFileAllocationTable(FileMetaData fileAllocationTable[MAX_NUM_FILES]);

#endif
