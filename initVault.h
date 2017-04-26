#ifndef INIT_VAULT_HEADER
#define INIT_VAULT_HEADER

#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <regex.h>

#include "vaultDataStructures.h"

int initVault (int argc, char **argv);

ssize_t parseNumberOfBytesFromFormat(char* dataAmount);
int initRepoMetaData(RepoMetaData *repoMetaData, char** argv);

#endif
