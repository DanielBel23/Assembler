/*pre - assembler header file
note: macro shall be called only in a separate sentence*/
/*the pre assembler level scan. returns 1 if the level is successfully complete and 0 otherwise*/
int preProcess(char*, char*);


/*linked list structure which helps to store a macro*/
typedef struct maCode
{
	struct maCode* next;
	char * codeLine;
} coLine;
/*adds a line of code*/
int addLine(coLine**, char*);
/*deletes all the lines of code*/
void delLine(coLine**);
/*prints the lines of code on a given file*/
void priLine(coLine*, FILE*);
/*fills the macro lines of code*/
void createCode(coLine**, FILE*);
/*-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -*/

/* Macro node structure */
typedef struct macNode * ptr;
typedef struct macNode
{
	ptr next;
	coLine ** macCon;
	char * macName;
} item;
/*creates a macro and fills its name and content returns 1 if the proccess is completed and 0 otherwise*/
int addMac(ptr*, char*, FILE*);
/*deletes the macro*/
void delMac(ptr*);
/*finds the macro by its name and prints on file*/
int printmc(ptr, char*, FILE*);
/*locates macro in the macro table by its name*/
int locatemc(ptr, char*);
/*-------------------------------------------------------*/
