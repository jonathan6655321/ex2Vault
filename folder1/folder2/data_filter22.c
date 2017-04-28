///*
// * data_filter.c
// *  Created on: Apr 5, 2017
// *      Author: Jonathan
// *     used stack overflow and the linux docs
// */
//
//#include <stdio.h>
//#include <sys/types.h>
//#include <sys/stat.h>
//#include <fcntl.h>
//#include <string.h>
//#include <errno.h>
//#include <stdlib.h>
//#include <unistd.h>
//#include <regex.h>
//
//#define KILO 1024
//#define MEGA 1048576
//#define GIGA 1073741824
//// 10 MB and up
//#define BIG_REQUEST 10485760
//
//size_t bytesFromFormat(char *dataAmount)
//{
//	size_t num;
//	char c;
//
//	const char *forRegex = dataAmount;
//	regex_t regex;
//	int reti;
//
//	/* Compile regular expression */
//	reti = regcomp(&regex, "^[1-9]{1}[0-9]*[a-zA-Z]$", REG_EXTENDED);
//	if (reti) {
//		fprintf(stderr, "Could not compile regex\n");
//		exit(1);
//	}
//
//	/* Execute regular expression */
//	reti = regexec(&regex, forRegex, 0, NULL, 0);
//	if (!reti) {
//		sscanf(dataAmount, "%zd %c", &num, &c);
//	}
//	else
//	{
//		printf("Error: data amount argument in wrong format or regex execution error\n\n");
//		return 0;
//	}
//
//	/* Free memory allocated to the pattern buffer by regcomp() */
//	regfree(&regex);
//
//	// printf("Data amount result: number is: %d and type is: %c\n\n", num, c);
//
//	if (c == 'K')
//	{
//		num*=KILO;
//	} else if (c == 'M')
//	{
//		num*=MEGA;
//	} else if (c=='G')
//	{
//		num*=GIGA;
//	} else if (c=='B')
//	{
//
//	} else
//	{
//		num = 0;
//	}
//
//	// printf("total number of bytes requested is: %d\n\n", num);
//
//	if(num==0)
//		printf("Error: data amount argument in wrong format\n\n");
//
//	return num;
//}
//
//int filter(char *buffer, int bufferSize)
//{
//	int numInvalidChars = 0;
//	int i;
//	for(i=0; i< bufferSize; i++)
//	{
//		if(buffer[i] > 126 || buffer[i] < 26)
//		{
//			numInvalidChars++;
//			continue;
//		} else
//		{
//			buffer[i-numInvalidChars] = buffer[i];
//		}
//	}
//	return bufferSize - numInvalidChars;
//}
//
//size_t min(int a, int b)
//{
//	size_t min = (a < b) ? a : b;
//	return min;
//}
//
//int main(int argc, char **argv) {
//
//	if (argc != 4)
//	{
//		printf("Error: invalid number of command line arguments \n\n");
//		return -1;
//	}
//
//	const char *inputFile = argv[2];
//	const char *outputFile = argv[3];
//
//	int inputFD = open( inputFile , O_RDONLY);
//	// printf("input file's filedes is: %d\n\n", inputFD);
//	if( inputFD < 0 )
//	  {
//	    printf( "Error opening input file: %s\n", strerror( errno ) );
//	    return errno;
//	  }
//
//	int outputFD = open( outputFile , O_WRONLY | O_CREAT, 0777);
//	if( outputFD < 0 )
//	  {
//	    printf( "Error opening output file: %s\n", strerror( errno ) );
//	    return errno;
//	  }
//
//	char *dataAmount = argv[1];
//	size_t maxBytesToRead = bytesFromFormat(dataAmount);
//	if (maxBytesToRead == 0)
//		return -1;
//
//	// set buffer size according to request size
//	int bufferSize;
//	if (maxBytesToRead > BIG_REQUEST){
//		bufferSize = 4*KILO;
//	} else {
//		bufferSize = 1*KILO;
//	}
//	char *readBuffer = malloc(bufferSize);
//	char *writeBuffer = malloc(bufferSize);
//	int locationInWriteBuffer = 0;
//
//	size_t totalBytesRead = 0;
//	size_t printedChars = 0;
//
//	while(totalBytesRead < maxBytesToRead)
//	{
//		// fill read buffer once:
//		int locationInReadBuffer = 0;
//		ssize_t bytesRead = 0;
//		while(locationInReadBuffer < bufferSize-1 && totalBytesRead < maxBytesToRead)
//		{
//			bytesRead = read(inputFD, readBuffer+locationInReadBuffer, min(bufferSize-locationInReadBuffer,maxBytesToRead-totalBytesRead));
//			if(bytesRead == -1)
//			{
//				printf("Error: read from file failed\n\n");
//				return -1;
//			}
//			locationInReadBuffer+=bytesRead;
//			totalBytesRead+=bytesRead;
//			if(bytesRead == 0)
//			{
//				lseek(inputFD, 0, SEEK_SET);
//			}
//		}
//		locationInReadBuffer = filter(readBuffer, locationInReadBuffer);
//
//		int i;
//		for(i = 0; i < locationInReadBuffer; i++)
//		{
//			writeBuffer[locationInWriteBuffer] = readBuffer[i];
//			locationInWriteBuffer++;
//			printedChars++;
//			if(locationInWriteBuffer == bufferSize)
//			{
//				write(outputFD, writeBuffer, bufferSize);
//				locationInWriteBuffer = 0;
//			}
//		}
//	}
//	if(locationInWriteBuffer != 0)
//	{
//		write(outputFD, writeBuffer, locationInWriteBuffer);
//	}
//
//	if (ftruncate(outputFD, printedChars) == -1)
//		printf("Error: truncate output file failed");
//
//
//	close(inputFD);
//	close(outputFD);
//	free(readBuffer);
//	free(writeBuffer);
//
//	printf("%zd characters requested, %zd characters read, %zd are printable\n", maxBytesToRead, totalBytesRead, printedChars);
//}
