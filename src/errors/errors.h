/**
 * Error number definitions and it's messages
 **/

#ifndef ERRORS_H_
#define ERRORS_H_   

#define E_NOERROR           0
#define E_CANNOTREADFILE   -1
#define E_CANNOTWRITEFILE  -2
#define E_PIUFILENOTVALID  -3

extern int piu_errno; 

void set_errorno(int);
char *piu_errmsg(int errno);

#endif /* ERRORS_H_ */

