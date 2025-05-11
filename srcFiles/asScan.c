#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "myTools.h"
#include "data.h"
#include "asScan.h"
/*asScan source file*/
/*recieves the code line without a symbol and a pointer to an empty array where the new word will be created (must be the size of 16).*/

void determineAREfield(char*, int, int);/*determines A, R, E fields*/
void firstWordBuild(char*, char*);/*activated if its a command sentance*/
void wordForTwoOps(char*, char*, char*);/*if we encounter two registers*/
void wordForOneOp(char*, char*, int);/*activated if its a command sentance*/
void dataWordBuild(char*);/*activated if a data sentance*/
void inputAddMet(char*, int, int);/*recieves addressing method and the number of the operand and inputs it in the binary word*/
void inputBinRegister(char*, char*, int);/*recieves a register and inputs it in the binary word*/
void wordForStr(char*, char);/*builds a data word for an intager*/
void wordForInt(char*, char*);/*builds a data word for a char*/
void addWordToEnd(binLine**, binLine*);/*adding the node to the endo of the list*/
int buildFirstBinLine(binLine**, char*, int);/*constructs the first command binLine structure, returns 1 if success and -1 otherwise*/
int buildOpBinLine(binLine** bl, char* line, int decAd);/*constructs binLine structure for operands, returns the last operands address if success and -1 otherwise*/
int buildDataOpLine(binLine**, char*, int* );/*builds binary words for data*/
int buildStringOpLine(binLine**, char, int*);/*builds binary words for string*/
int lookForFlag(binLine*);/*indicating if a flag of missing word is rised*/
int locateExen(exenTable, char*);/*locates if ginen name exists in the exen table*/
void takeString(char*, char*);/*recieves a line with .string data ang returns the string content in use in: 370*/


/*assembler table linked list----------------------------------------------*/
int addSrc(assemblyTable* at, char* src, int decimalAddress)
{
	int lastDecAd=0;
	assemblyTable temp = NULL;
	assemblyTable newAT = (assemblyTable)malloc(sizeof(table));
	if(!newAT)
	{
		fprintf(stderr, "cannot add src code due to allocation failure\n");
		return -1;
	}
	/*putting the src code*/
	strcpy(newAT->srcCode, src);
	/*initalizing the structure and creating the binary words*/
	newAT->inBinary = (binLine **)malloc(sizeof(binLine *));
	if (!newAT->inBinary)
	{
		fprintf(stderr, "cannot add src code due to allocation failure\n");
		free(newAT);
		return -1;
	}
	*newAT->inBinary = NULL;
	lastDecAd = addBinary(newAT->inBinary, src, decimalAddress);
	if(lastDecAd == -1)
	{
		free(newAT);
		return -1;
	}
	/*finding out if there is a missing translation to binary (if it is a symbol in the first scan)*/
	newAT->misFlag = lookForFlag(*(newAT->inBinary));
	/*setting the next*/
	newAT->next = NULL;
	/*locating the symbol to the end of the list*/
	if(*at == NULL)
		*at = newAT;
	else
	{
		temp=*at;
		while(temp->next != NULL)
			temp = temp->next;
		temp->next = newAT;
	}
	return lastDecAd;
}
int lookForFlag(binLine* node)
{
	while(node != NULL)
	{
		if(node->misFlag == 1)
			return 1;
		node = node->next;
	}
	return 0;
}

void printAsTab(assemblyTable at)
{
	if (at == NULL)
	{
		printf("Symbol table is empty.\n");
		return;
	}
	while(at!=NULL)
	{
		printf("************************************************\n");
		printf("srcCode is: %s\n", at->srcCode);
		printf("misFlag is: %d\n", at->misFlag);
		printBinaryWordList(*(at->inBinary));
		printf("************************************************\n");
		at = at->next;
	}
}

void delSrc(assemblyTable* at)
{
	assemblyTable temp;
	while(*at != NULL)
	{
		temp = *at;
		*at = (*at)->next;
		delBinary(temp->inBinary);
		free(temp->inBinary);
		free(temp);
	}
	*at = NULL;
}

/*new*****************************************************************/
void editSrcBinary(assemblyTable at, char* newBinaryCode, int opNum)
{
	int i;
	binLine* current = *(at->inBinary);
	for (i=1; i<opNum; i++)
	{
		if (current->next == NULL)
		{
			fprintf(stderr, "Error: The binary list has less than %d nodes.\n", opNum);
			return;
		}
		current = current->next;
	}
	strcpy(current->binarycode, newBinaryCode);
}

int getDecAdd(assemblyTable at)
{
	return (*(at->inBinary))->decimalAddress;
}

/*new*****************************************************************/

/*end of assembler table linked list---------------------------------------*/

/*exen table linked list---------------------------------------------------*/

int checkEntries(exenTable* et, symbolTable st)
{
	exenTable temp = *et;
	int address;
	while(temp != NULL)
	{
		address = locateSym(st, temp->exenName);/*recieves the address*/
		if(address==0)/*if address is not found*/
		{
			fprintf(stderr, "error: entry symbol: %s is initalized but not defined\n", temp->exenName);
			return -1;
		}
		if(address==1)
		{
			fprintf(stderr, "error: entry symbol: %s defined as extern\n", temp->exenName);
			return -1;
		}
		temp->decimalAddress=address;
		temp=temp->next;
	}
	return 1;
}

int inputEntry(exenTable* exen, char* entryName, int lineNum)
{
	exenTable temp;
	exenTable newExen = (exenTable)malloc(sizeof(*newExen));
	if(!newExen)
	{
		fprintf(stderr, "error: cannot add src code due to allocation failure\n");
		return -1;
	}
	/*indicating its an entry*/
	newExen->exenType=2;
	/*copying the name*/
	strcpy(newExen->exenName, entryName);
	/*putting an address that will be changed when the time is due*/	
	newExen->decimalAddress=-100;
	/*setting the next to null*/
	newExen->next = NULL;

	/*adding to the end of the list*/
	if(*exen == NULL)
		*exen = newExen;
	else
	{
		temp=*exen;
		while(temp->next != NULL)
			temp = temp->next;
		temp->next = newExen;
	}
	return 1;
}

int inputExtern(exenTable* exen, char* entryName, int address)
{
	exenTable temp;
	exenTable newExen = (exenTable)malloc(sizeof(*newExen));
	if(!newExen)
	{
		fprintf(stderr, "error: cannot add src code due to allocation failure\n");
		return -1;
	}
	/*indicating its an extern*/
	newExen->exenType=1;
	/*copying the name*/
	strcpy(newExen->exenName, entryName);
	/*putting an address*/
	newExen->decimalAddress=address;
	/*setting the next to null*/
	newExen->next = NULL;

	/*adding to the end of the list*/
	if(*exen == NULL)
		*exen = newExen;
	else
	{
		temp=*exen;
		while(temp->next != NULL)
			temp = temp->next;
		temp->next = newExen;
	}
	return 1;
}

void delExenTab(exenTable* exen)
{
	exenTable temp;
	while (*exen != NULL)
	{
		temp = *exen;
		*exen = (*exen)->next;
		free(temp);
	}
}

void printExenTable(exenTable exen)
{
	exenTable temp = exen;
	printf("Exen Table:\n");
	printf("----------------------------\n");
	while (temp != NULL)
	{
		printf("Name: %s\n", temp->exenName);
		printf("Type: %s\n", (temp->exenType == 1) ? "Extern" : "Entry");
		printf("Address: %d\n", temp->decimalAddress);
		printf("----------------------------\n");
		temp = temp->next;
	}
}

int locateExen(exenTable exen, char* exeName)
{
	exenTable temp=exen;
	while((temp!=NULL)&&(strcmp(temp->exenName, exeName)!=0))
	{
		temp = temp->next;
		if(temp==NULL)
			return -1;/*if not found*/
	}
	return 1;/*if found*/
}

/*end ofexen table linked list---------------------------------------------*/

/*binary words linked list-------------------------------------------------*/

int addBinary(binLine** bl, char* line, int deccimalAdd)
{
	int Kword, lineIndex=0, countDec=0, decadd, i, tempI=0;
	char* word = nextWord(line, &lineIndex);
	char tempBin[16];
	char stringBuff[82];
	/*this allocation is memory leaking-------------------------------*/
	binLine *newBL = (binLine *)malloc(sizeof(binLine));/*allocation for the new node*/
	memset(tempBin, '0', 16*sizeof(char));
	tempBin[15] = '\0';
	if(!newBL)
	{
		free(word);
		fprintf(stderr, "cannot add to assembler table due to allocation failure\n");
		return -1;
	}
	newBL->binarycode[15] = '\0';
	Kword = identifyCommand(word);
	/*since its a line without symbol, first word needs to be command or label*/
	if((0<=Kword)&&(Kword<16))/*if instruction code*/
	{
		decadd = buildFirstBinLine(bl, line, deccimalAdd+countDec);
		if(decadd==-1)
		{
			free(word);
			free(newBL);
			return -1;
		}
		free(word);
		/*newcode-------------------------------------------*/
		tempI=lineIndex;
		word = nextWord(line, &tempI);
		if(word==NULL)
		{
			free(newBL);
			return decadd+1;
		}
		free(word);
		/*--------------------------------------------------*/
		countDec++;
		decadd = buildOpBinLine(bl, line+lineIndex, deccimalAdd+countDec);
		free(newBL);/*new*/
		return decadd+1;
	}
	if(Kword==101)/*if data*/
	{
		free(word);
		word = nextWord(line, &lineIndex);
		if(word == NULL)
			return deccimalAdd;
		if((word[0]==',')&&(strlen(word)==1))/*if its an independant comma, skip it*/
		{
			free(word);
			word=nextWord(line, &lineIndex);
		}
		separateIncomma(&word, &lineIndex);
		cleanWordsChar(word, ',');
		while(word!=NULL)
		{
			decadd = buildDataOpLine(bl, word, &deccimalAdd);
			if(decadd==-1)
			{
				free(word);
				free(newBL);
				return -1;
			}
			free(word);
			word = nextWord(line, &lineIndex);
			if(word==NULL)
			{
				free(newBL);/*new*/
				return deccimalAdd;
			}
			if((word[0]==',')&&(strlen(word)==1))/*if its an independant comma, skip it*/
			{
				free(word);
				word=nextWord(line, &lineIndex);
			}
			separateIncomma(&word, &lineIndex);
			cleanWordsChar(word, ',');
		}
	}
	if(Kword==102)
	{
		memset(stringBuff, '0', sizeof(stringBuff));
		takeString(line, stringBuff);
		for(i=0; i<strlen(stringBuff); i++)
		{
			decadd = buildStringOpLine(bl, stringBuff[i], &deccimalAdd);
			if(decadd == -1)
			{
				free(newBL);
				return -1;
			}
		}
		decadd = buildStringOpLine(bl, 0, &deccimalAdd);
		free(newBL);
		free(word);
		return decadd;
	}
	else
	{
		free(word);
		free(newBL);
	}
	return -1;
}

int buildStringOpLine(binLine** bl, char operand, int *decAd)
{
	char tempBin[16];
	binLine *newBL = (binLine *)malloc(sizeof(binLine));/*allocation for the new node*/
	if(!newBL)
	{
		fprintf(stderr, "cannot add to assembler table due to allocation failure\n");
		return -1;
	}
	memset(tempBin, '0', 16*sizeof(char));
	tempBin[15] = '\0';
	newBL->binarycode[15] = '\0';
	/*set to instruction code*/
	newBL->IcDc=2;
	/*marking that the word is not missing*/
	newBL->misFlag=0;
	/*inputing decimal address*/
	newBL->decimalAddress=*decAd;
	*decAd=*decAd+1;
	/*building the binary machine code*/
	wordForStr(newBL->binarycode, operand);
	/*setting to null since it is the first word*/
	newBL->next=NULL;
	/*adding the new word to the end of the list*/
	addWordToEnd(bl, newBL);
	/*return newBL->decimalAddress;*/
	return *decAd;
}

int buildDataOpLine(binLine** bl, char* operand, int *decAd)
{
	char tempBin[16];
	binLine *newBL = (binLine *)malloc(sizeof(binLine));/*allocation for the new node*/
	if(!newBL)
	{
		fprintf(stderr, "cannot add to assembler table due to allocation failure\n");
		return -1;
	}
	memset(tempBin, '0', 16*sizeof(char));
	tempBin[15] = '\0';
	newBL->binarycode[15] = '\0';
	/*set to instruction code*/
	newBL->IcDc=2;
	/*marking that the word is not missing*/
	newBL->misFlag=0;
	/*inputing decimal address*/
	newBL->decimalAddress=*decAd;
	*decAd=*decAd+1;
	/*building the binary machine code*/
	wordForInt(newBL->binarycode, operand);
	/*setting to null since it is the first word*/
	newBL->next=NULL;
	/*adding the new word to the end of the list*/
	addWordToEnd(bl, newBL);
	/*return newBL->decimalAddress;*/
	return *decAd;
}

int buildFirstBinLine(binLine** bl, char* line, int decAd)
{
	int lineIndex=0;
	char* word = nextWord(line, &lineIndex);
	char tempBin[16];
	binLine *newBL = (binLine *)malloc(sizeof(binLine));/*allocation for the new node*/
	if(!newBL)
	{
		fprintf(stderr, "cannot add to assembler table due to allocation failure\n");
		free(word);
		return -1;
	}
	if(word==NULL)/*if word is null we reached the end of line*/
	{
		free(newBL);
		return -1;
	}
	/*setting the arrays*/
	memset(tempBin, '0', 16*sizeof(char));
	tempBin[15] = '\0';
	newBL->binarycode[15] = '\0';
	/*set to instruction code*/
	newBL->IcDc=1;
	/*marking that the word is not missing*/
	newBL->misFlag=0;
	/*inputing decimal address*/
	newBL->decimalAddress=decAd;
	/*building the binary machine code*/
	firstWordBuild(line, newBL->binarycode);
	/*strcpy(newBL->binarycode, tempBin);*/
	/*setting to null since it is the first word*/
	newBL->next=NULL;
	/*adding the new word to the end of the list*/
	addWordToEnd(bl, newBL);
	free(word);
	return newBL->decimalAddress;
}
int buildOpBinLine(binLine** bl, char* line, int decAd)
{
	int lineIndex=0;
	char* word = NULL;
	char* word2 = NULL;
	char tempBin[16];
	/*checking allocated memories*/
	binLine *newBL = NULL;
	binLine *newBL2 = NULL;
	word = nextWord(line, &lineIndex);
	if(word==NULL)/*if first operand is null we reached the end of line*/
	{
		return decAd;
	}
	separateIncomma(&word, &lineIndex);
	cleanWordsChar(word, ',');
	newBL = (binLine *)malloc(sizeof(binLine));
	if(!newBL)
	{
		fprintf(stderr, "cannot add to assembler table due to allocation failure\n");
		free(word);
		return -1;
	}
	/*setting the arrays*/
	memset(tempBin, '0', 16*sizeof(char));
	tempBin[15] = '\0';
	newBL->binarycode[15] = '\0';
	
	/*if second operand is null we reached the end of line- meaning it is a command with one operand*/
	word2 = nextWord(line, &lineIndex);
	if(word2==NULL)
	{
	/*building binary word for one operanded commands*/
		/*set to instruction code*/
		newBL->IcDc=1;
		/*inputing decimal address*/
		newBL->decimalAddress=decAd;
		/*building the binary machine code*/
		wordForOneOp(tempBin, word, 2);
		strcpy(newBL->binarycode, tempBin);
		/*setting to null since it is the first word*/
		newBL->next=NULL;
		/*marking if word is missing or not*/
		if(newBL->binarycode[0]=='?')
			newBL->misFlag=1;
		else
			newBL->misFlag=0;
		/*adding the new word to the end of the list*/
		addWordToEnd(bl, newBL);
		free(word);
		return newBL->decimalAddress;
	}
	if((word2[0]==',')&&(strlen(word2)==1))/*if its an independant comma, skip it*/
	{
		free(word2);
		word2=nextWord(line, &lineIndex);
	}
	cleanWordsChar(word2, ',');

	/*building binary words for two operands*/
	/*if both of the operands are registers*/
	if((identifyAddressingMethod(word)==2)||(identifyAddressingMethod(word)==3))
	{
		if((identifyAddressingMethod(word2)==2)||(identifyAddressingMethod(word2)==3))
		{
		/*building binary word for one operanded commands*/
			/*set to instruction code*/
			newBL->IcDc=1;
			/*inputing decimal address*/
			newBL->decimalAddress=decAd;
			/*building the binary machine code*/
			wordForTwoOps(tempBin, word, word2);
			strcpy(newBL->binarycode, tempBin);
			/*setting to null since it is the first word*/
			newBL->next=NULL;
			/*marking word is not missing*/
			newBL->misFlag=0;
			/*adding the new word to the end of the list*/
			addWordToEnd(bl, newBL);
			free(word);
			free(word2);
			return newBL->decimalAddress;
		}
	}
	/*otherwise the two operands are not registers and we need to allocate enother word*/
	newBL2 = (binLine *)malloc(sizeof(binLine));
	if(!newBL2)
	{
		fprintf(stderr, "cannot add to assembler table due to allocation failure\n");
		free(word);
		free(word2);
		free(newBL);
		return -1;
	}
	
	/*building word for origin operand*/
	/*set to instruction code*/
	newBL->IcDc=1;
	/*inputing decimal address*/
	newBL->decimalAddress=decAd;
	/*building the binary machine code*/
	wordForOneOp(tempBin, word, 1);
	strcpy(newBL->binarycode, tempBin);
	/*setting to null since it is the first word*/
	newBL->next=NULL;
	/*marking if word is missing or not*/
	if(newBL->binarycode[0]=='?')
		newBL->misFlag=1;
	else
		newBL->misFlag=0;
	/*adding the new word to the end of the list*/
	addWordToEnd(bl, newBL);
	free(word);

	/*building word for destination operand*/
	
	/*setting the arrays*/
	memset(tempBin, '0', 16*sizeof(char));
	tempBin[15] = '\0';
	newBL2->binarycode[15] = '\0';
	/*set to instruction code*/
	newBL2->IcDc=1;
	/*inputing decimal address*/
	newBL2->decimalAddress=decAd+1;
	/*building the binary machine code*/
	wordForOneOp(tempBin, word2, 2);
	strcpy(newBL2->binarycode, tempBin);
	/*setting to null since it is the first word*/
	newBL2->next=NULL;
	/*marking if word is missing or not*/
	if(newBL2->binarycode[0]=='?')
		newBL2->misFlag=1;
	else
		newBL2->misFlag=0;
	/*adding the new word to the end of the list*/
	addWordToEnd(bl, newBL2);
	free(word2);
	return newBL2->decimalAddress;
}

void delBinary(binLine** head)
{
	binLine* current = *head;
	binLine* nextNode;
	while (current != NULL)
	{
		nextNode = current->next;
		free(current);
		current = nextNode;
	}
	*head = NULL;
}

void printBinaryWord(binLine* node)
{
	if (node == NULL)
	{
		return;
	}
	printf("Decimal Address: %d\n", node->decimalAddress);
	printf("IcDc: %d\n", node->IcDc);
	printf("MisFlag: %d\n", node->misFlag);
	printf("Binary Code: %s\n", node->binarycode);
	if(node->next!=NULL)
		printf("next is: %d\n", node->next->decimalAddress);
	else
		printf("next is: NULL\n");
	printf("------------------------------------\n");
}

void printBinaryWordList(binLine* head)
{
	binLine* current = head;
	while (current != NULL)
	{
		printBinaryWord(current);
		current = current->next;
	}
}

void addWordToEnd(binLine** head, binLine* newWord)
{
	/*would be needed in case we need to traverse the list*/
	binLine* temp = *head;
	/*if the node is empty*/
	if(*head==NULL)/*added '*' to head*/
	{
		*head = newWord;
		return;
	}
	/*traversing to the end of the list*/
	while(temp->next != NULL)
	{
		temp = temp->next;
	}
	temp->next=newWord;

}

/*end of binary words linked list------------------------------------------*/

/*symbol table functions---------------------------------------------------*/

void printSymbolTable(symbolTable st) {
    if (st == NULL) {
        printf("Symbol table is empty.\n");
        return;
    }

    while (st != NULL) {
        printf("Symbol Name: %s\n", st->symbolName);
        printf("Symbol Value: %d\n", st->symValue);
        printf("External Flag: %d\n", st->externFlag);
        printf("Binary Value: %s\n", st->binaryvalue);
        printf("----------------------------\n");
        st = st->next;
    }
}


int addSymbol(symbolTable* st, char* symName, int decAddress, int exFlag)
{
	symbolTable newSymbol, st2;
	char bin[16], str[4];/*the max amout of memory image is 4096*/
	/*setting the array*/
	memset(bin, '0', 16*sizeof(char));
	bin[15]='\0';

	/*dynamically allocating memory*/
	newSymbol = (symbolTable)malloc(sizeof(symbols));
	if(!newSymbol)
	{
		fprintf(stderr, "cannot add symbol due to allocation failure\n");
		return -1;
	}

	/*creating the node with the wanted values*/
	strcpy(newSymbol->symbolName, symName);
	newSymbol->symValue = decAddress;
	newSymbol->externFlag=exFlag;
	newSymbol->next=NULL;
	/*creating the binary word for the symbol*/
	if(exFlag==1)/*extern flag is raised - setting the field accordingly*/
	{
		bin[14]='1';
		bin[13]='0';
		bin[12]='0';
		decAddress = 0;
	}
	else if(exFlag==0)/*extern flag is not raised - setting the field accordingly*/
	{
		bin[14]='0';
		bin[13]='1';
		bin[12]='0';
	}
	sprintf(str, "%d", decAddress);
	decimalToBinary(str, bin, 12);
	strcpy(newSymbol->binaryvalue, bin);

	/*locating the node in the table*/
	if(*st==NULL)
	{
		*st=newSymbol;
		return 1;
	}
	/*checking if the symbol already exists*/
	if(locateSym(*st, newSymbol->symbolName)==1)
	{
		fprintf(stderr, "error: the symbol: %s already exists\n", newSymbol->symbolName);
		free(newSymbol);
		return -1;
	}
	/*locating the symbol to the end of the list*/
	st2=*st;
	while(st2->next != NULL)
		st2 = st2->next;
	st2->next = newSymbol;
	return 1;
}

void delSymTab(symbolTable* st)
{
	symbolTable temp;
	while (*st != NULL)
	{
		temp = *st;
		*st=(*st)->next;
		free(temp);
	}
}

int locateSym(symbolTable st, char* name)
{
	symbolTable tempst=st;
	while((tempst!=NULL)&&(strcmp(tempst->symbolName, name)!=0))
	{
		tempst = tempst->next;
		if(tempst == NULL)
			return 0;/*if not found*/
	}
	if(tempst->externFlag==1)
		return 1;
	return tempst->symValue;/*if found*/
}

int locateSymAdd(symbolTable st, char* name, char* binary)
{
	symbolTable tempst=st;
	while((tempst!=NULL)&&(strcmp(tempst->symbolName, name)!=0))
	{
		tempst = tempst->next;
		if(tempst == NULL)
			return 0;/*if not found*/
	}
	strcpy(binary, tempst->binaryvalue);
	return tempst->symValue;/*if found*/
}

/*end symbol table functions----------------------------------------------*/

void wordForStr(char* binary, char ch)
{
	char str[3];/*the biggest value of ascii table is 127*/
	int num = ch;
	sprintf(str, "%d", num);
	decimalToBinary(str, binary, 15);
}

void wordForInt(char* binary, char* num)
{
	decimalToBinary(num, binary, 15);
}

void wordForOneOp(char* binary, char* operand, int opLocation)
{
	int lineIndex=0;
	char* op = nextWord(operand, &lineIndex);
	int address = identifyAddressingMethod(op);
	if((address==3)||(address==2))
	{
		cleanWordsChar(op, '*');
		if(opLocation==1)/*if origin operand*/
			inputBinRegister(binary, op, 1);
		else if(opLocation==2)/*if destination operand*/
			inputBinRegister(binary, op, 2);
		/*the field will be 'A'*/
		binary[12] = '1';
		binary[13] = '0';
		binary[14] = '0';
		free(op);
		return;
	}
	if(address==0)
	{
		cleanWordsChar(op, '#');
		decimalToBinary(op, binary, 12);
		/*the field will be 'A'*/
		binary[12] = '1';
		binary[13] = '0';
		binary[14] = '0';
		free(op);
		return;
	}
	if(address==1)
	{
		binary[0] = '?';/*marking that this array is not ready yet*/
		free(op);
		return;
	}
}

void wordForTwoOps(char* binary, char* register1, char* register2)
{
	int lineIndex=0;
	char* reg1 = nextWord(register1, &lineIndex);
	char* reg2 = NULL;
	lineIndex=0;
	reg2 = nextWord(register2, &lineIndex);
	cleanWordsChar(reg1, '*');
	cleanWordsChar(reg2, '*');
	inputBinRegister(binary, reg1, 1);
	inputBinRegister(binary, reg2, 2);
	free(reg1);
	free(reg2);
	binary[12] = '1';
	binary[13] = '0';
	binary[14] = '0';
}

void firstWordBuild(char* line, char* binary)
{
	int idcom, addMet, lineIndex=0;
	char* word = nextWord(line, &lineIndex);
	memset(binary, '0', 15*sizeof(char));
	binary[15] = '\0';
	/*setting A,R,E to A*/
	binary[12] = '1';
	binary[13] = '0';
	binary[14] = '0';
	inputBinary(binary, word);
	/*making sure no NULL terminator is inputed after strcpy*/
	binary[4] = '0';
	/*finished setting the op code*/
	idcom = identifyCommand(word);
	free(word);
	/*identifying the command code to navigate the operands*/
	
	word=nextWord(line, &lineIndex);
	if(word==NULL)
		return;
	/*if there are no more words in line, we exit*/
	if((word[0]==',')&&(strlen(word)==1))/*if its an independant comma, skip it*/
	{
		free(word);
		word=nextWord(line, &lineIndex);
	}
	separateIncomma(&word, &lineIndex);
	cleanWordsChar(word, ',');/*cleaning the words from comma(if there are commas infront or behind it)*/
	addMet = identifyAddressingMethod(word);
	if((-1<idcom)&&(idcom<5))
	{
		inputAddMet(binary, addMet, 1);
	}
	else if((4<idcom)&&(idcom<14))
	{
		inputAddMet(binary, addMet, 2);
		free(word);
		return;
	}
	/*finished operating 1 operand commands*/
	free(word);
	word=nextWord(line, &lineIndex);
	if((word[0]==',')&&(strlen(word)==1))/*if its an independant comma, skip it*/
	{
		free(word);
		word=nextWord(line, &lineIndex);
	}
	cleanWordsChar(word, ',');/*cleaning the words from comma(if there are commas infront or behind it)*/
	addMet = identifyAddressingMethod(word);
	inputAddMet(binary, addMet, 2);
	free(word);
	return;
}

void inputBinRegister(char* binary, char* reg, int operandIndicator)
{
	int index = 6;
	if(operandIndicator==2)/*can be 2 or 1, 2 is dest operand 1 is origin operand*/
	{
		index = 9;
	}
	if(strcmp(reg, "r0") == 0)
	{
		binary[index]='0';
		binary[index+1]='0';
		binary[index+2]='0';
		return;
	}
	else if(strcmp(reg, "r1") == 0)
	{
		binary[index]='0';
		binary[index+1]='0';
		binary[index+2]='1';
		return;
	}
	else if(strcmp(reg, "r2") == 0)
	{
		binary[index]='0';
		binary[index+1]='1';
		binary[index+2]='0';
		return;
	}
	else if(strcmp(reg, "r3") == 0)
	{
		binary[index]='0';
		binary[index+1]='1';
		binary[index+2]='1';
		return;
	}
	else if(strcmp(reg, "r4") == 0)
	{
		binary[index]='1';
		binary[index+1]='0';
		binary[index+2]='0';
		return;
	}
	else if(strcmp(reg, "r5") == 0)
	{
		binary[index]='1';
		binary[index+1]='0';
		binary[index+2]='1';
		return;
	}
	else if(strcmp(reg, "r6") == 0)
	{
		binary[index]='1';
		binary[index+1]='1';
		binary[index+2]='0';
		return;
	}
	else if(strcmp(reg, "r7") == 0)
	{
		binary[index]='1';
		binary[index+1]='1';
		binary[index+2]='1';
		return;
	}
}

void determineAREfield(char* binary, int addMet, int externFlag)
{
	if(addMet==1)/*if its addressing method 1*/
	{
		if(externFlag==1)/*if the address is external*/
		{
			binary[14]='1';/*mark 'E'*/
			return;
		}
		binary[13]='1';/*otherwise marking 'R'*/
		return;
	}
	else/*the rest are using fiekd 'A'*/
	{
		binary[12]='1';/*mark 'A'*/
		return;
	}
}

void inputAddMet(char* word, int addMet, int operand)
{
	int i, j=0;
	if(operand==1)/*origin operand*/
	{
		for(i=7; i>3; i--)
		{
			if(addMet==j)
			{
				word[i]='1';
				return;
			}
			j++;
		}
	}
	else if(operand==2)/*destination operand*/
	{
		for(i=11; i>7; i--)
		{
			if(addMet==j)
			{
				word[i]='1';
				return;
			}
			j++;
		}
	}
}

int identifyAddressingMethod(char* operand)
{
	int id;
	if(operand[0] == '#')/*if its the first addressing method*/
		return 0;
	if(operand[0] == '*')/*if its the third addressing method*/
		return 2;
	id = identifyCommand(operand);/*if its the second addressing method*/
	if((199<id)&&(id<208))
		return 3;
	return 1;/*if none of the methods identified we would call second addressing method and check it later*/
}

int validateOperand(char* operand, int line)
{
	int am, id, num;
	char buffer[80];
	strcpy(buffer, operand);
	am = identifyAddressingMethod(buffer);
	if(am==0)/*if the addressing method is: 0, only a number is a valid operand*/
	{
		cleanWordsChar(buffer, '#');
		if(identifyCharType(buffer)==2)
		{
			/*new code------------------------*/
			num = atoi(buffer);
			if((-2048>num)||(4095<num))
			{
				fprintf(stderr, "error: in line: %d, operand is out of bounds: %d (bounds are: -2048<#operand<4095) and thus an invalid operand\n", line, num);
				return -1;
			}
			/*--------------------------------*/
			return 0;
		}
		fprintf(stderr, "error: in line: %d, invalid operand: %s\n", line, operand);
		return -1;
	}
	if(am==1)/*if the addressing method is: 1, only a word is a valid operand*/
	{
		if(identifyCharType(buffer)==1)
			return 1;
		fprintf(stderr, "error: in line: %d, invalid operand: %s\n", line, operand);
		return -1;
	}
	if(am==2)/*if the addressing method is: 2, only a register with '*' is a valid operand*/
	{
		if((buffer[0]!='*')||(buffer[strlen(operand)-1]=='*'))
		{
			fprintf(stderr, "error: in line: %d, invalid operand: %s\n", line, operand);
			return -1;
		}
		cleanWordsChar(buffer, '*');
		id = identifyCommand(buffer);
		if((199<id)&&(id<208))
			return 2;
		fprintf(stderr, "error: in line: %d, invalid operand: %s\n", line, operand);
		return -1;
	}
	else
	{
		id = identifyCommand(buffer);
		if((199<id)&&(id<208))
			return 3;
		fprintf(stderr, "error: in line: %d, invalid operand: %s\n", line, operand);
		return -1;
	}
}

void takeString(char* line, char* str)
{
	int start=0, end=strlen(line)-1;
	/*finding the first quotation mark*/
	while(line[start] != '"')
		start++;
	start++;
	/*copying*/
	strcpy(str, line+start);
	/*inputing '\0' to all the chars before and including the last quotation mark*/
	while(str[end] != '"')
	{
		str[end] = '\0';
		end--;
	}
	str[end] = '\0';
}
