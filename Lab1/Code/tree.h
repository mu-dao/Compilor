extern int yylineno;
extern char* yytext;

struct tree{
	int line;
	char *name;
	int is_token;
	struct tree *l; //left child
	struct tree *r; //right child
	union{
		char* type;
		int int_value;
		float float_value;
	};
};

struct tree *newtree(char* name,int is_token,int num,...);

void print_tree(struct tree*,int level);
