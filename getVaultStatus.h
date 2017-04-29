#ifndef GET_VAULT_STATUS_HEADER
#define GET_VAULT_STATUS_HEADER

#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "initVault.h"
#include "vaultIO.h"
#include "vaultDataStructures.h"

#define GET_VAULT_STATUS_NUM_ARGUMENTS 3

int getVaultStatus (int argc, char** argv);

double getFragmentationRatio(FileMetaData *fileAllocationTable, int numFilesInVault, ssize_t sizeOfAllFilesInRepo);
int getNumBlocksInVault(FileMetaData *fileAllocationTable, int numFilesInVault);
int loadAllBlocks(DataBlock *allBlocks, FileMetaData *fileAllocationTable, int numFilesInVault);
void sortDataBlocksByOffset(DataBlock *allBlocks, int numBlocks);
void printAllBlocks(DataBlock *allBlocks, int numBlocks);

#endif
