/*here the main program will be written*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "myTools.h"
#include "preas.h"
#include "guardian.h"
#include "data.h"
#include "asScan.h"
#include "creator.h"

int cpyArg(char** dest, const char* src);
void printArgs(int argc, char* argv[]);

int main(int argc, char* argv[])
{
	char* fileName = NULL;
	char* temp = NULL;
	FILE *fd;
	int i;

	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
		return EXIT_FAILURE;
	}

	/*the rules say file shall be opened without ending*/
	for(i=1; i<argc; i++)
	{
		printf("-------------------------------------------------\n");
		cpyArg(&temp, argv[i]);
		printf("program %d of %d: %s\n", i, argc-1, temp);
		fileName = nameBuilder(temp, ".as");
		if(!(fd = fopen(fileName, "r+")))
		{
			free(temp);
			fprintf(stderr, "cannot open file: %s\n", fileName);
			printf("-------------------------------------------------\n");
			continue;
		}
		printf("file: %s successfully opened\n", fileName);
		assemble(fileName);
		fileName = NULL;
		rewind(fd);
		fclose(fd);
		free(temp);
		temp = NULL;
		printf("finished compiling\n");
		printf("-------------------------------------------------\n");
	}
	return 1;
}

int cpyArg(char** dest, const char* src)
{
	*dest = (char*)malloc((strlen(src)+5)*sizeof(char));
	if(*dest == NULL)
	{
		printf("Memory allocation failed\n");
		return -1;
	}
	strcpy(*dest, src);
	return 1;
}

void printArgs(int argc, char* argv[])
{
	int i;
	for(i=0; i<argc; i++)
		printf("argument: %s\n", argv[i]);
}
