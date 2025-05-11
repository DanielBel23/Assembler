/*myTools.c header file general functions which help to execute the code*/

/*functions helping with working on lines-------------------------------*/
char* makeLine(FILE**);
char* nextWord(const char*, int*);/*recieves a sentance and returns the next word since the recieved index without white spaces*/
int isWhiteSpace(char ch);/*identifies a white space*/
char * nameBuilder(char*, char*);/*builds a name of file*/
int skipBlankLines(FILE**);/*skips blank lines*/
void inputString(char*, char*, int);/*recieves two arrays and inputs the smaller one from the given index to the bigger array*/
char * buildString(int);/*builds string in the size of given intager*/
/*----------------------------------------------------------------------*/

/*functions that operate recieved words (usually by nextWord)-----------*/
void cleanWordsChar(char*, char);/*cleans the first and the last char of the recieved word from the stated char*/
int identifyCharType(char*);/*recieves a string and identifies if it is a number or a word(word can include numbers as well as letters) or none. returns 1 for word 2 for number and -1 for illegal combination of chars.*/
void separateIncomma(char**, int*);/*recieves a word, and if there is a comma, separates the word to two words, inputs the first word and sets the index to hold comma's index*/
/*----------------------------------------------------------------------*/

/*functions for translation---------------------------------------------*/
void decimalToBinary(char*, char*, int);/*transletes decimal numbers to binary numbers*/
int binToOc(char* binary);/*recieves a binary number and proccesses it to octal number and returns it to int*/
/*----------------------------------------------------------------------*/

