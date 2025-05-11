#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "myTools.h"
#include "data.h"
#include "creator.h"
#include "guardian.h"
#include "asScan.h"
#include "preas.h"

int fistAsScan(FILE*, int*, int*, assemblyTable*, symbolTable*, exenTable*);/*first assembler scan*/
/*-------------------------------------------------------------------------*/
int secondAsScan(assemblyTable*, symbolTable*, exenTable*);/*recieves the structures and performs the second scan*/
int fillUp(assemblyTable*, symbolTable, exenTable*);/*fills up the assembler table and exen table in the second scan*/
/*-------------------------------------------------------------------------*/
int createObj(assemblyTable, char*, int, int);/*builds the .obj file for given assembly table returns -1 if fails*/
void printOnObj(assemblyTable, FILE*);/*traverse the assembler table*/
void printWords(binLine* bl, FILE*);/*prints the binary words of a code*/
/*-------------------------------------------------------------------------*/
int createEnt(exenTable, char*);/*builds the .ent file for given exen table returns -1 if fails*/
/*-------------------------------------------------------------------------*/
int createExt(exenTable, char*);

int assemble(char* fileName)
{
	/*initializing variables*/
	FILE* fd;
	int IC=0, DC=0;
	assemblyTable at = NULL;
	symbolTable st = NULL;
	exenTable et = NULL;
	char* tempFile = NULL;
	char fileAM[82];
	memset(fileAM, 0, sizeof(fileAM));
	
	/*building the .am file & performing pre - assembler and updating 		the .am file. if an error was found - deleting the file, freeing the 		memory and returning*/
	if(preProcess(fileName, fileAM)==-1)
	{
		remove(fileAM);
		/*need to free allocated memory*/
		return -1;
	}

	/*opening .am file*/
	if(!(fd = fopen(fileAM, "r")))
	{
		fprintf(stderr, "cannot open file: %s\n", fileAM);
		return -1;
	}
	
	/*performing first assembler scan and creating the assembler table 		with the .am file.*/
	if(fistAsScan(fd, &IC, &DC, &at, &st, &et)==-1)
	{
		remove(fileAM);
		delSrc(&at);
		delSymTab(&st);
		delExenTab(&et);
		fclose(fd);
		return -1;
	}
	if(IC+DC > 4096)
	{
		fprintf(stderr, "warning: memory image exceeds intended memory image of 4096. thus there is a danger of loosing content or unfinishing the program the way intended by user\n");
		remove(fileAM);
		delSrc(&at);
		delSymTab(&st);
		delExenTab(&et);
		fclose(fd);
		return -1;
	}
	printf("first scan finished\n");
	/*performing second assembler scan and finishing the assembler 		table*/
	if(secondAsScan(&at, &st, &et)==-1)
	{
		remove(fileAM);
		delSrc(&at);
		delSymTab(&st);
		delExenTab(&et);
		fclose(fd);
		return -1;
	}
	printf("second scan finished\n");
	
	/*for printing the structures*/
	/*printAsTab(at);
	printSymbolTable(st);
	printExenTable(et);
	*/

	/*checking if there are .extern declarations and creates .ext file accordingly*/
	if(createObj(at, fileName, IC, DC)==(-1))
	{
		remove(fileAM);
		delSrc(&at);
		delSymTab(&st);
		delExenTab(&et);
		fclose(fd);
		return -1;
	}
	printf(".ob completed\n");

	/*checking if the exen table was filled*/
	if(et != NULL)
	{
		/*checking if there are .entry declarations and creates .ent file accordingly*/
		if((et->exenType) == 2)
		{
			if(createEnt(et, fileName)==-1)
			{
				remove(fileAM);
				tempFile = nameBuilder(fileName, ".ob");
				remove(tempFile);
				free(tempFile);
				delSrc(&at);
				delSymTab(&st);
				delExenTab(&et);
				fclose(fd);
				return -1;
			}
		}
		printf(".ent completed\n");
		/*printing the assembler table with translation to octal digit base*/
		if(createExt(et, fileName)==-1)
		{
			remove(fileAM);
			tempFile = nameBuilder(fileName, ".ob");
			remove(tempFile);
			free(tempFile);
			tempFile = nameBuilder(fileName, ".ent");
			remove(tempFile);
			free(tempFile);
			delSrc(&at);
			delSymTab(&st);
			delExenTab(&et);
			fclose(fd);
			return -1;
		}
	}
	printf(".ext completed\n");
	/*freeing memory*/
	fclose(fd);
	delSrc(&at);
	delSymTab(&st);
	delExenTab(&et);
	return 1;
}

int secondAsScan(assemblyTable* at, symbolTable* st, exenTable* et)
{
	/*initiating a pointer to traverse the list*/
	assemblyTable tempAT = *at;
	/*symbolTable tempST;*/
	while(tempAT!=NULL)
	{
		if((tempAT->misFlag)==1)
		{
			if(fillUp(&tempAT, *st, et)==-1)
				return -1;
		}
		tempAT = tempAT->next;
	}
	return 1;
}

int fistAsScan(FILE* fd, int* IC, int* DC, assemblyTable* at, symbolTable* st, exenTable* et)
{
	int lineNum=1, lineIndex=0, validation=0, decimalAddress=100, idCom, progressedAddress, tempIndex;
	char* word = NULL;
	char* wordTemp = NULL;
	char line[82];
	while(fgets(line, sizeof(line), fd)!=NULL)
	{
		/*if a syntax error is identified, the function will check only syntax from now on*/
		if(validation==-1)
		{
			lineSyntaxGuard(line, lineNum);
			lineNum++;
			continue;
		}
		validation = lineSyntaxGuard(line, lineNum);
		if(validation==-1)
		{
			if(at != NULL)
				delSrc(at);
			lineNum++;
			continue;
		}
		/*if no sytax error identified, we can proceed loading the assembler strucutres*/
		word = nextWord(line, &lineIndex);
		idCom = identifyCommand(word);
		/*if word starts with label*/
		if((100<idCom)&&(idCom<105))
		{
			/*if .extern*/
			if(idCom==104)
			{
				free(word);
				/*finding the name of the symbol*/
				word = nextWord(line, &lineIndex);
				/*add symbol and rise external flag*/
				if(addSymbol(st, word, decimalAddress, 1)==-1)
				{
					validation = -1;
				}
				/*setting the variables for the next interation*/
				free(word);
				lineNum++;
				lineIndex = 0;
				/*added the extern symbol, now moving on*/
				continue;
			}
			/*if .entry-------------------------------------*/
			if(idCom==103)
			{
				free(word);
				/*finding the name of the symbol*/
				word = nextWord(line, &lineIndex);
				/*add symbol to exen table*/
				inputEntry(et, word, lineNum);
				/*setting the variables for the next interation*/
				free(word);
				lineNum++;
				lineIndex = 0;
				/*added the extern symbol, now moving on*/
				continue;
			}
			/*----------------------------------------------*/
			/*if .string*/
			if(idCom==102)
			{
				progressedAddress = decimalAddress;
				decimalAddress = addSrc(at, line, progressedAddress);
				*DC=*DC+(decimalAddress-progressedAddress);
				/*setting the variables for the next interation*/
				lineNum++;
				lineIndex = 0;
				free(word);
				continue;
			}
			/*if .data*/
			if(idCom==101)
			{
				progressedAddress = decimalAddress;
				decimalAddress = addSrc(at, line, progressedAddress);
				*DC=*DC+(decimalAddress-progressedAddress);
				/*setting the variables for the next interation*/
				lineNum++;
				lineIndex = 0;
				free(word);
				continue;
			}
		}
		tempIndex = lineIndex;
		/*checking if it is a symbol*/
		if(word[strlen(word)-1] == ':')
		{
			wordTemp = nextWord(line, &lineIndex);/*moving on to the next word to check if there is an extern or entry label*/
			idCom = identifyCommand(wordTemp);
			if((103==idCom)||(idCom==104))
			{
				/*if extern*/
				if(idCom==104)
				{
					free(wordTemp);
					/*finding the name of the symbol*/
					wordTemp = nextWord(line, &lineIndex);
					/*add symbol and rise external flag*/
					if(addSymbol(st, wordTemp, decimalAddress, 1)==-1)
					{
						validation = -1;
					}
					/*setting the variables for the next interation*/
					free(word);
					free(wordTemp);
					lineNum++;
					lineIndex = 0;
					/*added the extern symbol, now moving on*/
					continue;
				}
				/*if entry---------------------------*/
				if(idCom==103)
				{
					free(wordTemp);
					/*finding the name of the symbol*/
					wordTemp = nextWord(line, &lineIndex);
					/*add symbol to exen table*/
					inputEntry(et, wordTemp, lineNum);
					/*setting the variables for the next interation*/
					free(word);
					free(wordTemp);
					lineNum++;
					lineIndex = 0;
					/*added the extern symbol, now moving on*/
					continue;
				}
				/*-----------------------------------*/	
			}
			free(wordTemp);
			cleanWordsChar(word, ':');
			if(addSymbol(st, word, decimalAddress, 0)==-1)
			{
				validation = -1;
			}
			progressedAddress = decimalAddress;
			decimalAddress = addSrc(at, line+tempIndex, progressedAddress);
		}
		else
		{
			progressedAddress = decimalAddress;
			decimalAddress = addSrc(at, line, progressedAddress);
		}
		if((0<=idCom)&&(idCom<16))
		{
			*IC=*IC+(decimalAddress-progressedAddress);
		}
		else
		{
			*DC=*DC+(decimalAddress-progressedAddress);
		}
		free(word);
		lineNum++;
		lineIndex = 0;
		
	}
	if(validation==-1)
		return -1;
	if(checkEntries(et, *st)==-1)
		return -1;
	return 1;
}

int fillUp(assemblyTable* at, symbolTable st, exenTable* et)
{
	int lineIndex = 0;
	char* word = NULL;
	char tempbin[16];
	char src[82];
	memset(src, 0, sizeof(src));
	/*copying the source code without a symbol*/
	strcpy(src, (*at)->srcCode);
	/*at this point, we can be sure that the first word is a command- so we skip it*/
	word = nextWord(src, &lineIndex);
	free(word);
	word = nextWord(src, &lineIndex);
	separateIncomma(&word, &lineIndex);/*****/
	cleanWordsChar(word, ',');
	/*chaecking if the operand is a symbol and editing it*/
	if(identifyAddressingMethod(word)==1)
	{
		if(locateSymAdd(st, word, tempbin)==0)
		{
			fprintf(stderr, "error: symbol: %s not found\n", word);
			free(word);
			return -1;
		}
		editSrcBinary(*at, tempbin, 2);
		/*if E field is highlighted*/
		if(tempbin[14]=='1')
		{
			inputExtern(et, word, getDecAdd(*at)+1);
		}
	}
	free(word);
	word = nextWord(src, &lineIndex);
	/*if there are no more operands*/
	if(word == NULL)
	{
		return 1;
	}
	/*if stumbled upon an independant comma*/
	if((word[0]==',')&&(strlen(word)==1))
	{
		free(word);
		word = nextWord(src, &lineIndex);
	}
	cleanWordsChar(word, ',');
	/*chaecking if the operand is a symbol and editing it*/
	if(identifyAddressingMethod(word)==1)
	{
		if(locateSymAdd(st, word, tempbin)==0)
		{
			fprintf(stderr, "error: symbol: %s not found\n", word);
			free(word);
			return -1;
		}
		editSrcBinary(*at, tempbin, 3);
		/*if E field is highlighted*/
		if(tempbin[14]=='1')
		{
			inputExtern(et, word, getDecAdd(*at)+2);
		}
	}
	free(word);
	return 1;
}

int createObj(assemblyTable at, char* fileName, int IC, int DC)
{
	FILE* fd;
	assemblyTable temp = at;
	char* newFile = nameBuilder(fileName, ".ob");
	if(!(fd = fopen(newFile, "w")))
	{
		fprintf(stderr, "cannot open file: %s\n", newFile);
		return -1;
	}
	/*printing IC and DC on the top of the file*/
	fprintf(fd, "  %d %d\n", IC, DC);
	printOnObj(temp, fd);
	fclose(fd);
	return 1;
}

void printOnObj(assemblyTable at, FILE* fd)
{
	while (at != NULL)
	{
		printWords(*(at->inBinary), fd);
		at = at->next;
	}
}

void printWords(binLine* bl, FILE* fd)
{
	binLine* current = bl;
	while (current != NULL)
	{
		fprintf(fd, "%04d %05d\n", current->decimalAddress, binToOc(current->binarycode));
		current = current->next;
	}
}

int createEnt(exenTable et, char* fileName)
{
	FILE* fd;
	exenTable temp = et;
	char* newFile = nameBuilder(fileName, ".ent");
	if(!(fd = fopen(newFile, "w")))
	{
		fprintf(stderr, "cannot open file: %s\n", newFile);
		return -1;
	}
	while((temp->exenType) == 2)
	{
		fprintf(fd, "%s %04d\n", temp->exenName, temp->decimalAddress);
		temp = temp->next;
	}
	fclose(fd);
	return 1;
}

int createExt(exenTable et, char* fileName)
{
	FILE* fd;
	exenTable temp = et;
	char* newFile = NULL;
	while(temp != NULL)
	{
		if(temp->exenType == 1)
			break;
		temp = temp->next;
	}
	if(temp == NULL)
		return 1;/*no extern found*/
	newFile = nameBuilder(fileName, ".ext");
	if(!(fd = fopen(newFile, "w")))
	{
		fprintf(stderr, "cannot open file: %s\n", newFile);
		return -1;
	}
	while(temp != NULL)
	{
		fprintf(fd, "%s %04d\n", temp->exenName, temp->decimalAddress);
		temp = temp->next;
	}
	fclose(fd);
	return 1;
}
