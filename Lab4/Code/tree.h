#include <stdio.h>
extern int yylineno;
extern char* yytext;

typedef struct tree{
	int line;
	char name[16];
	int is_token;
	struct tree *children; //left child
	struct tree *next; //right child
	union{
		char id[16];
		int int_value;
		float float_value;
	};
	int depth;
	int kind;
	char stofunc[16];
}tree;
tree* root;

/*struct*/tree *newtree(char* name,int is_token,int num,...);
void print_tree(struct tree*,int level);
