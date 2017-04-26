#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <regex.h>

#include "initVault.h"

#define INIT_NUM_ARGUMENTS 4
#define MIN_BYTES_FOR_DATA 0

int initVault (int argc, char **argv)
{
	printf("hey i am in initVault.c\n\n");

	if(argc != INIT_NUM_ARGUMENTS)
	{
		printf("Error: invalid number of arguments\n\n");
		return -1;
	}

	char* repoName = argv[1];
	int vaultFileDescriptor = open( repoName , O_WRONLY | O_CREAT, 0777);
	if(vaultFileDescriptor == -1)
	{
		printf("Error: couldn't open the vault file\n\n");
		return -1;
	}

	RepoMetaData repoMetaData;
	if(initRepoMetaData(&repoMetaData, argv) == -1)
	{
		printf("Error: couldn't initialize the repoMetaData")
		return -1;
	}

	FileMetaData FileAllocationTable[MAX_NUM_FILES];

	if(isRepositoryTotalSizeSufficient(repoMetaData, FileAllocationTable) == -1)
	{
		printf("Error: repository-total-size request not large enough");
		return -1;
	}




	return 1;
}


ssize_t parseNumberOfBytesFromFormat(char* dataAmount)
{
	size_t num;
	char c;

	const char *forRegex = dataAmount;
	regex_t regex;
	int reti;

	/* Compile regular expression */
	reti = regcomp(&regex, "^[1-9]{1}[0-9]*[a-zA-Z]$", REG_EXTENDED);
	if (reti) {
		fprintf(stderr, "Could not compile regex\n\n");
		return 0;
	}

	/* Execute regular expression */
	reti = regexec(&regex, forRegex, 0, NULL, 0);
	if (!reti) {
		sscanf(dataAmount, "%zd %c", &num, &c);
	}
	else
	{
		printf("Error: data amount argument in wrong format or regex execution error\n\n");
		return 0;
	}

	/* Free memory allocated to the pattern buffer by regcomp() */
	regfree(&regex);

	// printf("Data amount result: number is: %d and type is: %c\n\n", num, c);

	if (c == 'K')
	{
		num*=KILO;
	} else if (c == 'M')
	{
		num*=MEGA;
	} else if (c=='G')
	{
		num*=GIGA;
	} else if (c=='B')
	{

	} else
	{
		num = 0;
	}

	 printf("total number of bytes requested is: %zd\n\n", num);

	if(num==0)
		printf("Error: data amount argument in wrong format\n\n");

	return num;
}

int initRepoMetaData(RepoMetaData *repoMetaData, char** argv)
{
	(*repoMetaData).repositoryTotalSize = parseNumberOfBytesFromFormat(argv[3]);
	if((*repoMetaData).repositoryTotalSize <= 0)
	{
		printf("Error: invalid vault size argument\n\n");
		return -1;
	}

	(*repoMetaData).creationTimeStamp = (*repoMetaData).lastModificationTimeStamp
			= time(NULL);


//	struct tm tm = *localtime(&t);
//	printf("Time is: %d-%d-%d %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

	(*repoMetaData).numFilesInVault = 0;

	return 1;
}

// assumes repoMetaData and FAT are initialized
int isRepositoryTotalSizeSufficient(RepoMetaData repoMetaData, FileMetaData *FileAllocationTable)
{
	ssize_t catalogSize = (sizeof(repoMetaData) + sizeof(FileAllocationTable));
	if(repoMetaData.repositoryTotalSize < catalogSize + MIN_BYTES_FOR_DATA)
	{
		return -1;
	} else
	{
		return 1;
	}

}
