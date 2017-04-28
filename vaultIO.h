#ifndef VAULT_IO_HEADER
#define VAULT_IO_HEADER

#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "vaultDataStructures.h"


int openVault(char *vaultName);

int lseekToStartOfFile(int fileDescriptor);
int lseekToOffset(int fileDescriptor, ssize_t offset);

ssize_t writeRepoMetaDataToVault(RepoMetaData *repoMetaData, int vaultFileDescriptor);
ssize_t readRepoMetaDataFromVault(RepoMetaData *repoMetaData, int vaultFileDescriptor);

ssize_t writeFileAllocationTableToVault(FileMetaData *FileAllocationTable, int vaultFileDescriptor);
ssize_t readFileAllocationTableFromVault(FileMetaData *FileAllocationTable, int vaultFileDescriptor);

int areRepoMetaDataStructsEqual(RepoMetaData rep1, RepoMetaData rep2);

void printRepoMetaData(RepoMetaData repoMetaData);
void printFileAllocationTable(FileMetaData *FileAllocationTable, int numFilesInVault);


#endif
