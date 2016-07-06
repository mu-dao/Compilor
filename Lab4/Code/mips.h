#ifndef _CODE_H
#define _CODE_H
#include"intercode.h"
typedef struct _var* var;

struct _var{
	char name[16];
	int var_no;
	int offset;
	int type;//0 for TEMP or ADDRESS,1 for VARIABLE
	var next;
};
extern void printMipsCode(char* fname);
#endif
