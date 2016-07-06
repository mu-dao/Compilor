#include "semantic.h"
extern void yyparse();
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
	if(is_error == 0){
		Program(root);
		if(argc <= 2) return 0;
		printCode(argv[2]);
	}	
	return 0;
}
