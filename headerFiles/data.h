/*here are the data sources for the assembler*/
/*commad table initialization*/
typedef struct
{
	char comName[5]; /*longest word is 'stop' and it consists of 4 binary digits and +1 for the null terminator*/
	int operands;	
	int num;
	char binary[5]; /*4 binary digits and +1 for the null terminator*/
} command;

typedef struct
{
	char regName[3];
	char binary[4];
} registers;

int identifyCommand(char*);
int inputBinary(char*, char*);
