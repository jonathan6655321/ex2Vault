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
#include "getVaultStatus.h"


#define DEFRAG_VAULT_NUM_ARGUMENTS 3

int defragVault (int argc, char** argv);
int writeBlockToOffsetMinusShift(ssize_t blockAbsoluteOffset, ssize_t totalAccumulatedGap, ssize_t blockNumBytes
				, int vaultFileDescriptor);
int loadAllBlocksPointers(DataBlock **allBlocksPointers, FileMetaData *fileAllocationTable, int numFilesInVault);
void sortDataBlocksPointersByOffset(DataBlock **allBlocksPointers, int numBlocks);

int zeroDelimitersAtOffset(ssize_t offset, int vaultFileDescriptor);

int defrag(int vaultFileDescriptor, DataBlock  **allBlocksPointers, int numBlocks);
#endif
