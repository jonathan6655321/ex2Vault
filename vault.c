#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include "addFile.h"
#include "supportedOperations.h"
#include "initVault.h"
#include "listFiles.h"
#include "removeFile.h"
#include "fetchFile.h"
#include "defragVault.h"
#include "getVaultStatus.h"



/*
This is the main file in the project.
It gets the arguments from the user and calls the relevant functions.
*/

int main (int argc, char **argv)
{
	if(argc < 3)
	{
		printf("Error: invalid number of arguments\n\n");
		return -1;
	}
		
	OperationType op = getOperationTypeFromString(argv[2]);

	if(op == INVALID_OPERATION)
	{
		printf("Error: invalid operation argument\n\n");
		return -1;
	}
	else
	{
		runOperation(op, argc, argv);
	}
	
	return 1;
}
