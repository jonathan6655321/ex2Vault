#ifndef PARSE_USER_INPUT_HEADER
#define PARSE_USER_INPUT_HEADER

#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define KILO 1024
#define MEGA 1048576
#define GIGA 1073741824

ssize_t parseNumberOfBytesFromFormat(char* dataAmount);

#endif
