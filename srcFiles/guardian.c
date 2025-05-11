#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "guardian.h"
#include "myTools.h"
#include "preas.h"
#include "asScan.h"
#include "data.h"

int labelOpGuard(char*, int, int);/*if we identify code 101-104*/
int commandOpGuard(char*, int, char*, int);/*if we identify code 0-15*/
int operandGuard(char*, int);/*if we dont identify any code*/
int nonSymGuard(char*, int, int);/*recieves a sentance without a symbol (consists of labelOpGuard and nonSymGuard)*/
int validateString(char*, int);/*recieves a string and validates it*/
int validateStrCommas(char*, int);/*in use in: 186*/
int locateIdentify(char*, int*);
int locateIdentify(char*, int*);

/*a line in the assembly file cannot exceed 80 characters including white spaces. this function makes sure that the file follows this pattern*/
int lineLengthGuard(FILE* fd)
{
	char* line = buildString(85);
	int lineNum = 1;
	size_t len;
	memset(line, 0, sizeof(char)*85);
	/*scaning each line in the file*/
	while(1)
	{
		if(fgets(line, 85, fd) == NULL)
			break;
		len = strlen(line);
		if(len>=82)
		{
			
				fprintf(stderr, "error: cannot exceed 80 characters per line, problem occured in line: %d\n", lineNum);
				free(line);
				return -1;
			
		}
		memset(line, 0, sizeof(char)*85);
		lineNum++;
	}
	rewind(fd);
	free(line);
	return 1;
}

int preAsGuard(FILE* fd)
{

	int lineI = 0, lineNum = 1;
	char* word = NULL;
	char* line = makeLine(&fd);
	if (line == NULL)
	{
		fprintf(stderr, "error: Failed to read the initial line\n");
		return -1;
	}
	word = nextWord(line, &lineI);
	while (line != NULL)
	{
		if (word != NULL && strcmp(word, "macr") == 0)
		{
			free(word);
			word = nextWord(line, &lineI);
			if (word == NULL)
			{
				fprintf(stderr, "error: In line: %d, unnamed macro\n", lineNum);
				free(line);
				return-1;
			}
			if (identifyCommand(word) != -1)
			{
				fprintf(stderr, "error: In line: %d, cannot name a macro with keyword: %s\n", lineNum, word);
				free(word);
				free(line);
				return -1;
			}
			free(word);
			word = nextWord(line, &lineI);
			if (word != NULL)
			{
				fprintf(stderr, "error: In line: %d, cannot add characters after macro name\n", lineNum);
				free(word);
				free(line);
				return -1;
			}
		}
		else if (word != NULL && strcmp(word, "endmacr") == 0)
		{
			free(word);
			word = nextWord(line, &lineI);
			if (word != NULL)
			{
				fprintf(stderr, "error: In line: %d, cannot add characters after 'endmacr' keyword\n", lineNum);
				free(word);
				free(line);
				return -1;
			}
		}
        	free(word);
        	word = NULL;
		free(line);
		line = NULL;
		lineI = 0;
		line = makeLine(&fd);
		if(line == NULL)
		{
			break;
		}
		if((skipBlankLines(&fd) == 0) && (line == NULL))
		{
			break;
		}
		lineNum++;
		word = nextWord(line, &lineI);
	}
	free(line);
	rewind(fd);
	return 1;
}


int commaGuard(char* line, int lineNum)
{
	int i=0, j=strlen(line)-1, temp=0;
	int lineindex=0;
	int commaCount=0; /*counts the amout of commas between two fields*/	
	char *word = NULL;
	word = nextWord(line, &lineindex);
	if(word[0] == ',')
	{
		fprintf(stderr, "error: in line: %d, unnecessary comma in the beginig of the code\n", lineNum);
		free(word);
		return -1;
	}
	if(word[strlen(word)-1] == ':')/*checking if the first word is a symbol. symbol can contain a comma (as long as it is part of its name)*/
	{
		free(word);
		word = nextWord(line, &lineindex); /*moving to the next word*/
	}
	if(word==NULL)/*if sentance is empty*/
	{
		free(word);
		return 1;
	}
	if(word[0] == ',')/*if there is a comma before the first word*/
	{
		fprintf(stderr, "error: in line: %d, comma is located improperly\n", lineNum);
		free(word);
		return -1;
	}
	free(word);
	i = lineindex;
	word = nextWord(line, &lineindex);/*checking whats coming as second word (or third, if symbol is envolved)*/
	if(word==NULL)
	{
		free(word);
		return 1;
	}
	if(word[0] == ',')
	{
		fprintf(stderr, "error: in line: %d, comma is located improperly\n", lineNum);
		free(word);
		return -1;
	}
	free(word);
	/*start of line finished*/
	while((isWhiteSpace(line[j]) == 1)||(line[j]==','))
	{
		if(line[j]==',')
		{
			fprintf(stderr, "error: in line: %d, unnecessary commas\n", lineNum);
			return -1;
		}
		j--;
	}
	while((isWhiteSpace(line[j]) == 0)&&(line[j]!=',')&&(line[j]!='\0'))
	{
		j--;
	}
	/*end of line finished*/

	/*if stumbeled upon a .string*/
	if(locateIdentify(line, &temp)==102)
		return 1;
	/*end of .string procession*/
	
	while(i<j)
	{
		while((isWhiteSpace(line[i]) == 1)&&(i<=j))/*skip white chars*/
			i++;
		while((isWhiteSpace(line[i]) == 0)&&(line[i]!=',')&&(line[i]!='\0')&&(i<j))/*skip chars until whitespace or comma*/
			i++;
		commaCount = 0;
		if(i<j)
		{
			while((isWhiteSpace(line[i]) == 1)||(line[i]==','))/*scanning white spaces and comma untill we encounter a letter*/
			{
				if(line[i] == '\0')
				{
					if(commaCount != 0)
					{
						fprintf(stderr, "error: in line: %d, unnecessary commas\n", lineNum);
						return -1;
					}
				}
				if(line[i]==',')
					commaCount++;
				i++;
			}
			if(commaCount>1)
			{
				fprintf(stderr, "error: in line: %d, unnecessary commas\n", lineNum);
				return -1;
			}
			if(commaCount == 0)
			{
				fprintf(stderr, "error: in line: %d, missing a comma between two opperands\n", lineNum);
				return -1;
			}
		}
		
	}
	return 1;
}

int lineSyntaxGuard(char* line, int lineNum)/*not complete*/
{
	int lineIndex=0, validation=1, symbolflag=0, saveIndex=0, val2, val3=1;
	char buff[80];
	char* word = nextWord(line, &lineIndex);
	if(word==NULL)
	{
		free(word);
		return 1;
	}
	val3=commaGuard(line, lineNum);
	if(word[strlen(word)-1] == ':')
	{
		cleanWordsChar(word, ':');
		if(identifyCharType(word)!=1)
		{
			fprintf(stderr, "error: in line: %d, invalid symbol: %s\n", lineNum, word);
			validation=-1;
		}
		symbolflag=1;
		free(word);
		saveIndex=lineIndex;
		word = nextWord(line, &lineIndex);
		if(word==NULL)
		{
			fprintf(stderr, "error: in line: %d, empty symbol\n", lineNum);
			free(word);
			return -1;
		}
		free(word);
		word = NULL;
	}
	if(word != NULL)
		free(word);
	
	strcpy(buff, line+saveIndex);
	val2=nonSymGuard(buff, lineNum, symbolflag);
	if(validation==-1)
		return validation;
	if(val3==-1)
		return val3;
	return val2;
}

int nonSymGuard(char* line, int lineNum, int afterSym)/*if it is activated after a symbol, afterSym=1*/
{
	int lineIndex=0, id, validation=1, tempIndex;
	char buff[80];
	char* tempword=NULL;
	char* word = nextWord(line, &lineIndex);
	id=identifyCommand(word);
	if((-1<id)&&(id<16))
	{
		strcpy(buff, line+lineIndex);
		validation=commandOpGuard(buff, lineNum, word, id);
	}
	else if((100<id)&&(id<105))
	{
		if((id==103)||(id==104))
		{
			if(afterSym==1)
			{
				fprintf(stderr, "warning: in line: %d, symbol is located before: %s keyword\n", lineNum, word);
			}
		}
		tempIndex=lineIndex;
		tempword=nextWord(line, &tempIndex);
		if(tempword==NULL)
		{
			fprintf(stderr, "error: in line: %d, symbol without initialization\n", lineNum);
			free(tempword);
			return -1;
		}
		free(tempword);
		strcpy(buff, line+lineIndex);
		validation=labelOpGuard(buff, lineNum, id);
	}
	else
	{
		fprintf(stderr, "error: in line: %d, unidentifiable command: %s\n", lineNum, word);
		strcpy(buff, line+lineIndex);
		operandGuard(buff, lineNum);
		validation = -1;
	}
	free(word);
	return validation;
}

int labelOpGuard(char* line, int lineNum, int labelCode)/*recieves a sentance without a symbol (tavit) and label (hanhaya)*/
{
	int num, lineIndex=0;
	char* word = NULL;
	if((labelCode==103)||(labelCode==104))/*if entry or extern*/
	{
		word = nextWord(line, &lineIndex);
		if(identifyCharType(word) != 1)
		{
			fprintf(stderr, "error: in line: %d, invalid name: %s\n", lineNum, word);
			if((labelCode==103)&&(word[strlen(word)-1]==':'))
				fprintf(stderr, "symbol name cannot contain \'%c\' character at the end of it as it used for storing instruction or data\n", word[strlen(word)-1]);
			free(word);
			return -1;
		}
		free(word);
		word = nextWord(line, &lineIndex);
		if(word != NULL)
		{
			fprintf(stderr, "error: in line: %d, unnecesary characters after label\n", lineNum);
			free(word);
			return -1;
		}
		free(word);
		return 1;
	}
	else
	{
		if(labelCode==101)/*if data*/
		{
			while((word=nextWord(line, &lineIndex))!=NULL)
			{
				/*we know for sure that the commas are alright- meaning there is only one comma between two operands*/
				if((strlen(word)==1)&&(word[0]==','))
				{/*if we stumbeled on a sole comma*/
					free(word);
					continue;
				}
				else
				{
					separateIncomma(&word, &lineIndex);
					cleanWordsChar(word, ',');
					if(identifyCharType(word)!=2)
					{
						fprintf(stderr, "error: in line: %d, unmatching data type: %s\n", lineNum, word);
						free(word);
						return -1;
					}
					/*new code-------------------*/
					num = atoi(word);
					if((-16384>num)||(16383<num))
					{
						fprintf(stderr, "error: in line: %d, intager is out of bounds: %d (bounds are: -16384<int<16383)\n", lineNum, num);
						free(word);
						return -1;
					}
					/*---------------------------*/
				}
				free(word);
			}
			return 1;
		}
		else/*meaning its string (code: 102)*/
		{
			if(validateString(line+lineIndex, lineNum) == -1)
				return -1;
			return 1;
		}
	}

	return 1;
}

/*recieves a sentance without command, the command is one of the operands*/
int commandOpGuard(char* line, int lineNum, char* commandName, int commandCode)
{
	int lineIndex=0, errorFlag=0, opValidation;
	char* originOperand = NULL;
	char* destinationOperand = NULL;
	char* extraOpCheck = NULL;
	if((-1<commandCode)&&(commandCode<5))/*commands with two operands*/
	{
		/*checking the origin operrand---------------------------*/
		originOperand = nextWord(line, &lineIndex);
		if(originOperand == NULL)
		{
			fprintf(stderr, "error: in line: %d, not enough operands for command: %s\n", lineNum, commandName);
			free(originOperand);
			return -1;
		}
		separateIncomma(&originOperand, &lineIndex);
		cleanWordsChar(originOperand, ',');
		if(commandCode == 4)
		{
			if(validateOperand(originOperand, lineNum)!=1)
			{
				errorFlag=1;
				fprintf(stderr, "error: in line: %d, invalid operand: %s for command: %s\n", lineNum, originOperand, commandName);
			}
		}
		else
		{
			if(validateOperand(originOperand, lineNum)==-1)
			{
				errorFlag=1;
				fprintf(stderr, "error: in line: %d, invalid operand: %s for command: %s\n", lineNum, originOperand, commandName);
			}
		}
		free(originOperand);
		/*finished checking the origin operrand------------------*/
	}
	/*commands with destination operand*/
	if((-1<commandCode)&&(commandCode<14))
	{
		destinationOperand = nextWord(line, &lineIndex);
		if(destinationOperand==NULL)
		{
			fprintf(stderr, "error: in line: %d, not enough operands for command: %s\n", lineNum, commandName);
			free(destinationOperand);
			return -1;
		}
		if((destinationOperand[0]==',')&&(strlen(destinationOperand)==1))
		{
			free(destinationOperand);
			destinationOperand = nextWord(line, &lineIndex);
		}
		cleanWordsChar(destinationOperand, ',');
		if((commandCode==1)||(commandCode==12))
		{
			if(validateOperand(destinationOperand, lineNum)==-1)
			{
				errorFlag=1;
				fprintf(stderr, "error: in line: %d, invalid operand: %s for command: %s\n", lineNum, destinationOperand, commandName);
			}
		}
		else if((commandCode==9)||(commandCode==10)||(commandCode==13))
		{
			opValidation=validateOperand(destinationOperand, lineNum);
			if((opValidation!=1)&&(opValidation!=2))
			{
				errorFlag=1;
				fprintf(stderr, "in line: %d, invalid operand: %s for command: %s\n", lineNum, destinationOperand, commandName);
			}
		}
		else/*for commands coded: 2-8, 0, 11*/
		{
			opValidation=validateOperand(destinationOperand, lineNum);
			if((opValidation==0)||(opValidation==-1))
			{
				errorFlag=1;
				fprintf(stderr, "error: in line: %d, invalid operand: %s for command: %s\n", lineNum, destinationOperand, commandName);
			}
		}
		free(destinationOperand);
		/*finished checking the destination operrand---------------*/
	}
	extraOpCheck=nextWord(line, &lineIndex);
	if(extraOpCheck!=NULL)
	{
		cleanWordsChar(extraOpCheck, ',');
		fprintf(stderr, "error: in line: %d, unnecesary characters starting from: %s for command: %s\n", lineNum, extraOpCheck, commandName);
		free(extraOpCheck);	
		return -1;
	}
	free(extraOpCheck);
	if(errorFlag==1)
		return -1;
	return 1;
}

int operandGuard(char* line, int lineNum)
{
	int lineIndex=0, va=1;
	char* word = NULL;
	while((word=nextWord(line, &lineIndex))!=NULL)
	{
		cleanWordsChar(word, ',');
		if(va!=-1)
			va=validateOperand(word, lineNum);
		else
			validateOperand(word, lineNum);
		free(word);
	}
	return va;
}

int validateString(char* str, int lineNum)
{
	int temp, i=0;
	/*skips white spaces*/
	while (isWhiteSpace(str[i])==1)
		i++;
	if(str[i]!='"')
	{
		fprintf(stderr, "error: in line: %d, quotation mark is lacking at the begining of the string: %s\n", lineNum, str);
		return -1;
	}
	temp = i;
	i = strlen(str)-1;
	if(str[i]=='\n')
		i--;
	if(str[i] == '"')
	{
		return 1;
	}
	while(str[i]!='"')
	{
		if(isWhiteSpace(str[i])==1)
		{
			i--;
			continue;
		}
		if(str[i]=='"')
		{
			return 1;
		}
		else
			break;
	}
	while(temp != i)
	{
		if(str[i] == '"')
		{
			fprintf(stderr, "error: in line: %d, aditional characters after the end of the quotation mark in: %s\n", lineNum, str);
			return -1;
		}
		i--;
	}
	fprintf(stderr, "error, in line: %d, no quotation mark to mark the end of string in: %s\n", lineNum, str);
	return -1;
}

int locateIdentify(char* str, int* start)
{
	int idCom, i=0;
	char* word = nextWord(str, &i);
	idCom = identifyCommand(word);
	while(!(idCom==102))
	{
		free(word);
		word = nextWord(str, &i);
		if(word == NULL)
		{
			free(word);
			return -1;
		}
		idCom = identifyCommand(word);
	}
	free(word);
	*start = i;
	return 102;
}
