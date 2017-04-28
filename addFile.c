#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <libgen.h>

#include "initVault.h"
#include "vaultIO.h"


#include "addFile.h"
// $./vault my_repository.vlt add /some/path/to/file_name
int addFile (int argc, char** argv)
{
	char* filePath = argv[3];
//	printf("%s\n\n",filePath);
	char* vaultName = argv[1];

	if (argc != ADD_FILE_NUM_ARGUMENTS)
	{
		printf("Error: wrong number of arguments for add operation\n\n");
		return -1;
	}

	int newFileDescriptor = open( filePath, O_RDONLY);
	if (newFileDescriptor < 0 )
	{
		printf("Error: could not open file\n\n");
		return -1;
	}

	struct stat newFileStat;
	if (fstat(newFileDescriptor, &newFileStat) < 0)
	{
		printf("Error: could not get the file stat\n\n");
		return -1;
	}

	int vaultFileDescriptor = openVault(vaultName);
	if (vaultFileDescriptor < 0)
	{
		("Error: failed opening vault for adding file\n\n");
		return -1;
	}

	RepoMetaData repoMetaData;
	getRepoMetaDataFromVault(&repoMetaData, vaultFileDescriptor);

	if(vaultHasEnoughSpaceForFile(newFileStat.st_size, repoMetaData) < 0)
	{
		printf("Error: not enough space for file in vault\n\n");
		return -1;
	}

	char *fileName = getFileNameFromPath(filePath);
	printf("the file name is: %s", fileName);

//	if(uniqeFileName() < 0)
//	{
//		printf("Error: file with same name exists in vault\n\n");
//		return -1;
//	}

//	printf("the new file's permissions: %d\n\n", newFileStat.st_mode );

	// from stack overflow
    printf("File Permissions: \t");
    printf( (S_ISDIR(newFileStat.st_mode)) ? "d" : "-");
    printf( (newFileStat.st_mode & S_IRUSR) ? "r" : "-");
    printf( (newFileStat.st_mode & S_IWUSR) ? "w" : "-");
    printf( (newFileStat.st_mode & S_IXUSR) ? "x" : "-");
    printf( (newFileStat.st_mode & S_IRGRP) ? "r" : "-");
    printf( (newFileStat.st_mode & S_IWGRP) ? "w" : "-");
    printf( (newFileStat.st_mode & S_IXGRP) ? "x" : "-");
    printf( (newFileStat.st_mode & S_IROTH) ? "r" : "-");
    printf( (newFileStat.st_mode & S_IWOTH) ? "w" : "-");
    printf( (newFileStat.st_mode & S_IXOTH) ? "x" : "-");
    printf("\n\n");


    printf("the size of FAT %zd\n\n", FILE_ALLOCATION_TABLE_SIZE);

//	printf("Result: file_name inserted\n\n"); // TODO uncomment
	return 1;
}

int vaultHasEnoughSpaceForFile(off_t fileSize, RepoMetaData repoMetaData)
{
	ssize_t catalogSize = REPO_META_DATA_SIZE + FILE_ALLOCATION_TABLE_SIZE;
	ssize_t remainingSpace = repoMetaData.repositoryTotalSize - catalogSize;
	if(fileSize > remainingSpace)
	{
		return -1;
	}
	else
	{
		return 1;
	}
}

char* getFileNameFromPath(char *filePath)
{
	char *base = basename(filePath);
	printf("file name is: %s \nfile path was: %s", base, filePath);
	return base;
}
