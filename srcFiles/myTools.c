/*functions which help the code are located here*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "myTools.h"
#include "data.h"

/*
void addWord(char* line, char* word);
void getWord(char* word, FILE* fd);
*/
void makeNegative(char*, int);/*turns a binary number to negative*/

char * buildString(int size)
{
	char *arr = (char *)calloc((size+1),sizeof(char));
	if(arr == NULL)
	{
		fprintf(stderr, "Memory allocation failed\n");
        	exit(1);
	}
	return arr;
}

char * nameBuilder(char* fileName, char* filEnding)
{
	int i, temp=0;
	char* dot = strrchr(fileName, '.');
	if(dot == NULL)
	{
		return strcat(fileName, filEnding);
	}

	/*deleting the chars after the dot*/
	i = dot-fileName;
	temp = i;
	while(fileName[i] != '\0')
	{
		fileName[i] = '0';
		i++;
	}
	fileName[i] = '0';
	fileName[temp] = '\0';
	return strcat(fileName, filEnding);
}

char* makeLine(FILE ** fd)
{
	char* str = buildString(82);
	if(fgets(str, 82, *fd) != NULL)
		return str;
	free(str);
	return NULL;
}

char* nextWord(const char *line, int *index)
{
	int i = *index, j;
	char *word = NULL;
	int start;

	if(i>80)
	{
		printf("next word does not exist\n");
		return NULL;
	}

	/*Skip initial whitespace characters*/
	while (isWhiteSpace(line[i])==1)
	{
		i++;
	}

	/*If end of line or end of string, return NULL*/
	if ((line[i] == '\n') || (line[i] == '\0'))
	{
		return NULL;
	}

	/*Calculate the length of the next word*/
	start = i;
	while ((!isWhiteSpace(line[i])) && (line[i] != '\0') && (line[i] != '\n'))
	{
		i++;
	}

	/*Allocate memory for the word and copy it*/
	word = buildString(i - start + 1);
	for (j = 0; j < i - start; j++) {
		word[j] = line[start + j];
	}
	word[j] = '\0';

	/*Update the index to point to the next character after the current word*/
	*index = i;

	return word;
}

int isWhiteSpace(char ch)
{
	if((ch == ' ')||(ch == '\t'))
		return 1;
	return 0;
}

/*skips the blank lines and moves the pointer after them. returns 0 if it reached eof otherwise returns 1*/
int skipBlankLines(FILE** fd)
{
	int i;
	char buffer[100]; /*Buffer to store line from the file*/
	int isBlank = 1;
	if (fd == NULL || *fd == NULL)
	{
        	return 0; /*Return 0 if the file pointer is NULL*/
	}

	/*Check if the line is blank (only contains whitespace characters)*/
	while (fgets(buffer, sizeof(buffer), *fd) != NULL)
	{
        	for (i = 0; buffer[i] != '\0'; i++)
		{
			if (buffer[i] != ' ' && buffer[i] != '\t' && buffer[i] != '\n' && buffer[i] != '\r')
			{
                		isBlank = 0;
                		break;
			}
        	}

        /*If the line is not blank, move the file pointer back to the beginning of this line and return 1*/
		if (!isBlank)
		{
			/*Move file pointer back to the beginning of the current line*/
			fseek(*fd, -strlen(buffer), SEEK_CUR);
			return 1;
		}
	}

	/*If we reach here, we have reached EOF*/
	return 0;
}

void cleanWordsChar(char* word, char ch)
{
	int i, len=strlen(word);
	if(len==0)
		return;
	if(word[0] == ch)
	{
		for(i=0; i<len; i++)
		{
			word[i] = word[i+1];
		}
		len = len-1;
	}
	if((len>0)&&(word[len-1] == ch))
	{
		word[len-1] = '\0';
	}
	return;
}

int identifyCharType(char* str)
{
	int numflag=0;
	int wordflag=0;
	int nonflag=0;
	int negflag=0;
	if(*str=='-')
	{
		if(strlen(str)==1)
			return -1;
		negflag=1;
		str++;
	}
	while (*str)
	{
		if(isalpha((char)*str))/*if the string has at least one letter - its a word*/
		{
			if(negflag==1)
				nonflag=1;
			if(nonflag==0)
			{
			/*if untill now we had only numbers - we turn the flag off. since it can not be a number from now on*/
				if(numflag==1)
					numflag=0;
				wordflag=1;
			}
		}
		else if(isdigit((char)*str))
		{/*a number cannot consist of nothing but numbers*/
			if((wordflag==0)&&(nonflag==0))
			{
				numflag=1;
			}
		}
		else
		{
			nonflag=1;
		}
		str++;
	}
	if(nonflag==1)
		return -1;
	if(wordflag==1)
		return 1;
	return 2;
}
void decimalToBinary(char* decimal, char* binary, int bits)
{
	int num, i, negFlag=0, lineIndex=0;
	char* word=nextWord(decimal, &lineIndex);/*copying the word so we wont chage the original one*/
	if(word[0]=='-')/*if the number is negative*/
	{
		negFlag=1;
		cleanWordsChar(word, '-');
	}
	num=atoi(word);
	memset(binary, '0', bits*sizeof(char));
	i=bits-1;
	/*if the number is 0*/
	if(num==0)
	{
		free(word);
		return;
	}

	while((num>0)&&(i>=0))
	{
		binary[i--] = (num%2)+'0';
		num = num/2;
	}

	if(negFlag==1)
		makeNegative(binary, bits);
	free(word);
}

void makeNegative(char* binary, int bits)
{
	int i, oneFlag=0;
	for(i=bits-1; i>-1; i--)
	{
		if(oneFlag==0)/*locating the first 1 (from right to left)*/
		{
			if(binary[i]=='1')
				oneFlag=1;
		}
		else if(oneFlag==1)/*if we located the first 1 then we nned to swap the bits*/
		{
			if(binary[i]=='1')
				binary[i]='0';
			else if(binary[i]=='0')
				binary[i]='1';
		}
	}
}

void inputString(char* bigger, char* smaller, int index)
{
	int biglen = strlen(bigger);
	int smallen = strlen(smaller);
	if((index>=0)&&(index+smallen<=biglen))
	{
		memcpy(bigger+index, smaller, smallen);
	}
}

void separateIncomma(char** word, int* lineIndex)/*can be paired only with an allocated array!*/
{
	char* wordptr = *word;
	char* temp = NULL;
	int i=1, j=strlen(wordptr);
	/*comma can be at the first cell or at the last cell*/
	while(wordptr[i]!='\0')
	{
		if(wordptr[i]==',')
		{
			break;
		}
		i++;
	}
	if(i<(j-1))/*if comma is located in the middle of the word*/
	{
		temp = buildString(i);
		memcpy(temp, wordptr, i*sizeof(char));
		free(wordptr);
		*word = temp;
		*lineIndex = *lineIndex-(j-i);
	}
	return;
}

int binToOc(char* binary)
{
	int i, octal=0, value=0;
	int pos = 1; /*To handle the position in the octal number*/
	/*Process binary string from right to left in groups of 3*/
	for (i=14; i>=0; i-=3)
	{
		value = 0;
		/*Convert each group of up to 3 binary digits to a single octal digit*/
		if (i >= 2)
			value += (binary[i-2]-'0')*4;
		if (i >= 1)
			value += (binary[i-1]-'0')*2;
		value += (binary[i]-'0');

		/*Combine the calculated octal digit into the final octal number*/
		octal += value*pos;
		pos = pos*10;
	}
	return octal;
}
