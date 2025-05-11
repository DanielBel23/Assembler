/*asScan header file*/

/*structure that allows to write numerous words in binary*/
typedef struct binaryWord
{
	struct binaryWord* next;
	int decimalAddress;/*stores the decimal address*/
	int IcDc;/*indicates if its an instruction or data: 1 for IC, 2 for DC*/
	int misFlag;/*indicates if a symbol is used and a word is missing*/
	char binarycode[15];/*stores the binary machine code*/
} binLine;

int addBinary(binLine**, char*, int);/*adds binary words and returns the last decimalAddress*/
void delBinary(binLine**);/*deletes the binary words*/
void printBinaryWordList(binLine*);/*print binary words list*/

/*-------------------------------------------------------------*/

typedef struct asTab * assemblyTable;
typedef struct asTab
{
	assemblyTable next;
	int misFlag;/*indicates if a symbol is used and a word is missing*/
	char srcCode[82];/*the source code*/
	binLine ** inBinary;/*the linked list where the words are stored*/
} table;
int addSrc(assemblyTable*, char*, int);/*adds src code to the table and returns the last decimal address*/
void editSrcBinary(assemblyTable, char*, int);/*recieves a binary string and inputs it on the bin line structure in the src*/
int getDecAdd(assemblyTable);
void printAsTab(assemblyTable);/*print the assembly table*/
void delSrc(assemblyTable*);/*deletes the entire table*/

/*-------------------------------------------------------------*/

typedef struct symTab * symbolTable;
typedef struct symTab
{
	symbolTable next;/*ponter to the next value*/
	char symbolName[82];/*name of the symbol*/
	int symValue;/*the value of the symbol (number of address)*/
	int externFlag;/*if raised, 1 is an extern symbol 0 otherwise*/
	char binaryvalue[16];/*stores the binary word that would be used when the label will be used*/
} symbols;

int addSymbol(symbolTable*, char*, int, int);
int locateSym(symbolTable, char*);
int locateSymAdd(symbolTable, char*, char*);
void delSymTab(symbolTable*);
void printSymbolTable(symbolTable);
/*-------------------------------------------------------------*/

typedef struct exenTab * exenTable;
typedef struct exenTab
{
	exenTable next;
	int exenType;/*1 for extern 2 for entry*/
	char exenName[82];
	int decimalAddress;
} exen;

int inputEntry(exenTable*, char*, int);/*adds entry type symbol without an address*/
int inputExtern(exenTable*, char*, int);/*adds extern type symbol with an address*/
int checkEntries(exenTable*, symbolTable);/*checks if the entry was initalized somewhere in the symbol table and inputs an address otherwise notifies for an error and returna -1*/
/*int checkExterns(exenTable*, symbolTable);checks if the extern was initalized somewhere in the symbol table and inputs an address otherwise notifies for an error and returna -1*/
void delExenTab(exenTable*);
void printExenTable(exenTable);

/*-------------------------------------------------------------*/

int identifyAddressingMethod(char*);/*recieves an operand without commas and identifies its addresing method (shitat miun)*/
int validateOperand(char*, int);/*recieves an operand and vildates it then returns its addressing method if its valid, otherwise returns -1*/
