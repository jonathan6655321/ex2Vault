#ifndef SUPPORTED_OPERATIONS_HEADER
#define SUPPORTED_OPERATIONS_HEADER

#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

#include "initVault.h"
#include "listFiles.h"
#include "addFile.h"
#include "removeFile.h"
#include "fetchFile.h"
#include "defragVault.h"
#include "getVaultStatus.h"

typedef enum OperationType {
	INIT_VAULT,
	LIST_FILES,
	ADD_FILE,
	REMOVE_FILE,
	FETCH_FILE,
	DEFRAG_VAULT,
	GET_VAULT_STATUS,
	INVALID_OPERATION} OperationType;

	OperationType getOperationTypeFromString(char* s);
	int runOperation(OperationType op, int argc, char** argv);



#endif
