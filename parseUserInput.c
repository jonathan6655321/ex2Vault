#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <regex.h>

#include "parseUserInput.h"

// returns -1 if wrong format or zero size.
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
//		fprintf(stderr, "Could not compile regex\n\n");
		return -1;
	}

	/* Execute regular expression */
	reti = regexec(&regex, forRegex, 0, NULL, 0);
	if (!reti) {
		sscanf(dataAmount, "%zd %c", &num, &c);
	}
	else
	{
//		printf("Error: data amount argument in wrong format\n\n");
		return -1;
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
		return -1;
	}

//	 printf("total number of bytes requested is: %zd\n\n", num);

//	if(num==0)
//		printf("Error: data amount argument in wrong format\n\n");

	return num;
}
