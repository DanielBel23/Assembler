#include "data.h"
#include <string.h>
/*data codes are here*/
command comTable[16] = {
	{"mov", 2, 0, "0000"},
	{"cmp", 2, 1, "0001"},
	{"add", 2, 2, "0010"},
	{"sub", 2, 3, "0011"},
	{"lea", 2, 4, "0100"},
	{"clr", 1, 5, "0101"},
	{"not", 1, 6, "0110"},
	{"inc", 1, 7, "0111"},
	{"dec", 1, 8, "1000"},
	{"jmp", 1, 9, "1001"},
	{"bne", 1, 10, "1010"},
	{"red", 1, 11, "1011"},
	{"prn", 1, 12, "1100"},
	{"jsr", 1, 13, "1101"},
	{"rts", 0, 14, "1110"},
	{"stop", 0, 15, "1111"}
};

registers regTable[8] = {
	{"r0", "000"},
	{"r1", "001"},
	{"r2", "010"},
	{"r3", "011"},
	{"r4", "100"},
	{"r5", "101"},
	{"r6", "110"},
	{"r7", "111"}
};

int identifyCommand(char* word)
{
	int i;
	for(i=0; i<16; i++)
	{
		if(strcmp(comTable[i].comName, word)==0)
			return i;
	}
	for(i=0; i<8; i++)
	{
		if(strcmp(regTable[i].regName, word)==0)
			return i+200;
	}
	if (strcmp(word, ".data") == 0) return 101;
	else if (strcmp(word, ".string") == 0) return 102;
	else if (strcmp(word, ".entry") == 0) return 103;
	else if (strcmp(word, ".extern") == 0) return 104;
	else if (strcmp(word, "macr")==0) return 300;
	else if (strcmp(word, "endmacr")==0) return 301;
	else return -1;
}

int inputBinary(char* binary, char* command)
{
	int i;
	for(i=0; i<16; i++)
	{
		if(strcmp(comTable[i].comName, command)==0)
		{
			strcpy(binary, comTable[i].binary);
			return 1;
		}
	}
	return -1;
}
