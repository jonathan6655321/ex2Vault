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

#define _FILE_OFFSET_BITS 64

/*
 This is the main file in the project.
 It gets the arguments from the user and calls the relevant functions.
 */

int main(int argc, char **argv) {
	int res;
	struct timeval start, end;
	long seconds, useconds;
	double mtime;

	// start time measurement
	gettimeofday(&start, NULL);

	if (argc < 3) {
		printf("Error: invalid number of arguments\n\n");
		// end time measurement and print result
		gettimeofday(&end, NULL);

		seconds = end.tv_sec - start.tv_sec;
		useconds = end.tv_usec - start.tv_usec;

		mtime = ((seconds) * 1000 + useconds / 1000.0);
		printf("Elapsed time: %.3f milliseconds\n", mtime);

		return -1;
	}

	OperationType op = getOperationTypeFromString(argv[2]);

	res = runOperation(op, argc, argv);

	// end time measurement and print result
	gettimeofday(&end, NULL);

	seconds = end.tv_sec - start.tv_sec;
	useconds = end.tv_usec - start.tv_usec;

	mtime = ((seconds) * 1000 + useconds / 1000.0);
	printf("Elapsed time: %.3f milliseconds\n", mtime);

	return res;
}

