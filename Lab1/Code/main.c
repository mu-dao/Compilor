#include "stdio.h"
#include "syntax.tab.h"

extern void yyrestart(FILE *);
extern int is_error;

int main(int argc,char **argv){
	is_error = 0;
	if(argc<=1) return 1;
	FILE* f = fopen(argv[1],"r");
	if(!f){
		perror(argv[1]);
		return 1;
	}
	yyrestart(f);
	yyparse();
	return 0;
}
