/*here is the proograms which check the errors in the code*/

/*what should we check about the macro:
1. if the macro name is proper
2. if in the first line and the last line (the lines which declare and end the macro) there are no extra characters
note: if there was an error in the pre processor level program shall be terminated immediatly*/
int lineLengthGuard(FILE*); /*makes sure the is no line longer than 80 characters in the source file*/
int preAsGuard(FILE*);/*makes sure there are no macro syntax mistakes*/
int commaGuard(char*, int);/*racieves a line of command and makes sure there are no comma mistakes. if there are mistakes, function will return -1 and */
int lineSyntaxGuard(char*, int);/*recieves a line and makes sure the command is written right*/
