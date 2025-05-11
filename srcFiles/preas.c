/*the pre processor level*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "preas.h"
#include "myTools.h"
#include "guardian.h"
#include "data.h"

/*macro shall be used in a separate line. if macro is used and there are extra characters in its line, the pre assembler wont print it.*/

int preProcess(char* fileName, char* fileAM)
{
	char* newFile = NULL;
	FILE* former;
	FILE* latter;
	/*ptr *macroTable = (ptr*)malloc(sizeof(ptr));*/
	ptr *macroTable = (ptr*)malloc(sizeof(ptr));
	char* word = NULL;
	char* line = NULL;
	char* word2 = NULL;
	char* macroName = NULL;
	int wordIndex = 0;

	*macroTable=NULL;
	/* TODO: check macrotable alloc */

	if(!(former = fopen(fileName, "r+")))
	{
		fprintf(stderr, "error: cannot open %s file\n", fileName);
		return -1;
	}
	rewind(former);
	if(lineLengthGuard(former)==-1)
		return -1;
	rewind(former);
	if(preAsGuard(former)==-1)
		return -1;
	newFile = nameBuilder(fileName, ".am");
	if(!(latter = fopen(newFile, "a")))
	{
		free(newFile);
		fclose(former);
		fprintf(stderr, "error: cannot open file\n");
		return -1;
	}
	strcpy(fileAM, newFile);
	/*scanning the formrer file, filling the macro table and printing the used properly macros.*/

	while((line = makeLine(&former)) != NULL)
	{
		word = nextWord(line, &wordIndex);
		/*new added code------------------------------*/
		if(word != NULL)
			if(word[0]==';')
			{
				free(word);
				free(line);
				wordIndex=0;
				continue;
			}
		/*-------------------------------------------*/
		if((word != NULL)&&(strcmp(word, "macr") == 0))
		{
			macroName = nextWord(line, &wordIndex);
			if (macroName != NULL)
			{
				addMac(macroTable, macroName, former);
				free(macroName);
			}
		}
		else if(word != NULL)
		{
			word2 = nextWord(line, &wordIndex);
			if(word2 == NULL)
			{
				if(*macroTable!=NULL)
				{
					if (printmc(*macroTable, word, latter) != 1)
					{
						if (fputs(line, latter) == EOF)
							perror("Error writing to file");
					}
				}
				else if (fputs(line, latter) == EOF)
					perror("Error writing to file");
				
			}
			else
			{
				if (fputs(line, latter) == EOF)
					perror("Error writing to file");
			}
		}
		wordIndex = 0;
		if(word2 != NULL){	
			free(word2);
			word2 = NULL;
		}
			
		if(word != NULL){
			free(word);
			word = NULL;
		}
		free(line);
	}
	printf(".am completed\n");
	rewind(former);
	fclose(former);
	rewind(latter);
	fclose(latter);
	if(macroTable != NULL)
		delMac(macroTable);
	free(macroTable);
	free(line);
	return 1;
}

/*macro table functions*/

/*adds one line*/
int addLine(coLine ** head, char * codeLine)
{
	coLine *l1 = *head;
	size_t lineLength = strlen(codeLine)+1;
	coLine * newLine = (coLine *)malloc(sizeof(coLine));
	if(!newLine)
	{
		fprintf(stderr, "error: memory capacity is exceeded: cannot add another line of code\n");
        	return -1;
	}
	newLine->codeLine = (char *)malloc(lineLength * sizeof(char));
	if(!newLine->codeLine)
	{
		free(newLine);
		fprintf(stderr, "error: this line cannot be added\n");
		return -1;
	}
	strcpy(newLine->codeLine, codeLine);
	newLine->next = NULL;
	if(*head == NULL)
	{
		*head = newLine;
		return 1;
	}
	while(l1->next != NULL)
		l1 = l1->next;
	l1->next = newLine;
	return 1;
}

/*deletes the entire list of lines*/
void delLine(coLine ** head)
{
	coLine* current;
	while(*head != NULL)
	{
		current = *head;
		*head = (*head)->next;
		free(current->codeLine);
		free(current);
	}
	free(head);
}

/*prints the lines on a given file*/
void priLine(coLine * cl, FILE* fd)
{
	while(cl != NULL)
	{
		fputs(cl->codeLine, fd);
		fputc('\n', fd);
		cl = cl->next;
	}
}

/*once the reader encounters the word "macr" as the first word in the line and is activated after the name of the macro*/
void createCode(coLine ** head, FILE* fd)
{
	
	char line[80];
	/*newly added code------------------------------*/
	char* word=NULL;
	int lineIndex=0;
	/*----------------------------------------------*/
	memset(line, 0, 80 * sizeof(char));
	fgets(line, 80, fd);
	line[strcspn(line, "\n")] = 0;
	while(strcmp(line, "endmacr") != 0)
	{
		printf("%s\n", line);
		/*newly added code------------------------------*/
		word=nextWord(line, &lineIndex);
		if(word==NULL)
		{
			free(word);
			lineIndex=0;
			memset(line, 0, 80 * sizeof(char));
			fgets(line, 80, fd);
			line[strcspn(line, "\n")] = 0;
			continue;
		}
		if(word[0]==';')
		{
			free(word);
			lineIndex=0;
			memset(line, 0, 80 * sizeof(char));
			fgets(line, 80, fd);
			line[strcspn(line, "\n")] = 0;
			continue;
		}
		/*----------------------------------------------*/
		addLine(head, line);
		memset(line, 0, 80 * sizeof(char));
		fgets(line, 80, fd);
		line[strcspn(line, "\n")] = 0;
		if(word!=NULL)
			free(word);
	}
}
/*-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -*/
/* Macro node structure */

/*creates a new macro with a given name up until 'endmacr keyword is encountered'*/
int addMac(ptr * head, char * macName, FILE * fd)
{
	size_t nameLength = strlen(macName)+1;
	item * it1 = *head;
	/*creating the macro node before adding to the linked list*/
	item * newMac = (item *)malloc(sizeof(item));
	if(!newMac)
	{
		fprintf(stderr,"error: memory capacity is exceeded: cannot add another macro\n");
		return -1;
	}
	newMac->macName = (char *)malloc(nameLength * sizeof(char));
	if(!newMac->macName)
	{
		free(newMac);
		fprintf(stderr, "error: this macro cannot be added\n");
		return -1;
	}
	strcpy(newMac->macName, macName);
	newMac->macCon = (coLine **)malloc(sizeof(coLine *));
	if (!newMac->macCon)
	{
		free(newMac->macName);
		free(newMac);
		fprintf(stderr, "error: memory capacity is exceeded: cannot add another macro\n");
		return -1;
	}
	*newMac->macCon = NULL;
	createCode(newMac->macCon, fd);
	newMac->next = NULL;
	/*finishing the creation of the node now finding where to add it in the list*/
	if(*head == NULL)
	{
		*head = newMac;
		return 1;
	}
	if(locatemc(*head, macName)==1)
	{
		fprintf(stderr, "error: macro is already creted, cannot create macros with the same name.\n");
		free(newMac->macName);
		delLine(newMac->macCon);
		free(newMac);
		return -1;
	}
	while(it1->next != NULL)
		it1 = it1->next;
	it1->next = newMac;
	return 1;
}


/*deletes an entire macro from the list, its name and content included*/
void delMac(ptr* mac)
{
	ptr p = *mac;
	ptr temp = NULL;
	while(p != NULL)
	{
		temp = p;
		p = p->next;		
		delLine(temp->macCon);
		free(temp->macName);
		free(temp);	
	}
	*mac = NULL;
}

/*prints the macro on the given file*/
int printmc(ptr head, char* name, FILE* fd)
{
	ptr mac = head;
	if(locatemc(head, name)==0)
	{
		return 1;
	}
	/*finds the macro in the macro table*/
	while ((mac != NULL) && (strcmp(mac->macName, name) != 0))
	{
		mac = mac->next;
		/*if we scanned the whole table and did not find the macro- the macro do not exist*/
		if (mac == NULL)
			return 0;
	}
	priLine(*(mac->macCon), fd);
	return 1;
	
}

int locatemc(ptr head, char* name)
{
	ptr mac = head;
	while ((mac != NULL) && (strcmp(mac->macName, name) != 0))
	{
		mac = mac->next;
		if (mac == NULL)
			return 0;/*if not found*/
	}
	return 1;/*if found*/
}
