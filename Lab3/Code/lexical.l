%{
#include "stdio.h"
#include "syntax.tab.h"
#include "tree.h"
extern int is_error;
void comment_line();
void comment_block();
%}

%option yylineno

COMMENT_LINE \/\/[^\n]*
COMMENT_BLOCK "/*"
SEMI ";"
COMMA ","
ASSIGNOP "="
RELOP >|<|>=|<=|==|!=
PLUS "+"
MINUS "-"
STAR "*"
DIV "/"
AND "&&"
OR "||"
DOT "."
NOT "!"
TYPE int|float
LP "("
RP ")"
LB "["
RB "]"
LC "{"
RC "}"
STRUCT "struct"
RETURN "return"
IF "if"
ELSE "else"
WHILE "while"
INT 0|[1-9][0-9]*
FLOAT (0|[1-9][0-9]*)\.[0-9]+
ID [_a-zA-Z][_a-zA-Z0-9]*

%%
\t {}
\n {}
" " {}
{COMMENT_LINE} {comment_line();}
{COMMENT_BLOCK} {comment_block();}
{STRUCT} {yylval.tree=newtree("STRUCT",1,0,yylineno);return STRUCT;}
{RETURN} {yylval.tree=newtree("RETURN",1,0,yylineno);return RETURN;}
{IF} {yylval.tree=newtree("IF",1,0,yylineno);return IF;}
{ELSE} {yylval.tree=newtree("ELSE",1,0,yylineno);return ELSE;}
{WHILE} {yylval.tree=newtree("WHILE",1,0,yylineno);return WHILE;}
{TYPE} {yylval.tree=newtree("TYPE",1,0,yylineno);return TYPE;}

{INT} {yylval.tree=newtree("INT",1,0,yylineno);return INT;}

{FLOAT} {yylval.tree=newtree("FLOAT",1,0,yylineno);return FLOAT;}

{ID} {yylval.tree=newtree("ID",1,0,yylineno);return ID;}

{SEMI} {yylval.tree=newtree("SEMI",1,0,yylineno);return SEMI;}
{COMMA} {yylval.tree=newtree("COMMA",1,0,yylineno);return COMMA;}
{ASSIGNOP} {yylval.tree=newtree("ASSIGNOP",1,0,yylineno);return ASSIGNOP;}
{RELOP} {yylval.tree=newtree("RELOP",1,0,yylineno);strcpy(yylval.tree->id,yytext);return RELOP;}
{PLUS} {yylval.tree=newtree("PLUS",1,0,yylineno);return PLUS;}
{MINUS} {yylval.tree=newtree("MINUS",1,0,yylineno);return MINUS;}
{STAR} {yylval.tree=newtree("STAR",1,0,yylineno);return STAR;}
{DIV} {yylval.tree=newtree("DIV",1,0,yylineno);return DIV;}
{AND} {yylval.tree=newtree("AND",1,0,yylineno);return AND;}
{OR} {yylval.tree=newtree("OR",1,0,yylineno);return OR;}
{NOT} {yylval.tree=newtree("NOT",1,0,yylineno);return NOT;}
{LP} {yylval.tree=newtree("LP",1,0,yylineno);return LP;}
{RP} {yylval.tree=newtree("RP",1,0,yylineno);return RP;}
{LB} {yylval.tree=newtree("LB",1,0,yylineno);return LB;}
{RB} {yylval.tree=newtree("RB",1,0,yylineno);return RB;}
{LC} {yylval.tree=newtree("LC",1,0,yylineno);return LC;}
{RC} {yylval.tree=newtree("RC",1,0,yylineno);return RC;}
{DOT} {yylval.tree=newtree("DOT",1,0,yylineno);return DOT;}
. {printf("Error type A at line %d: Mysterious characters \'%s\'.\n",yylineno,yytext);is_error = 1;} 

%%
//("/*"(([^\*]*(\*[^\/])?)*)"*/")

void comment_line(){
	char c = input();
	while(c != '\n'){ 
		c = input();
	}
}

void comment_block(){
	char c = input();
	while(1){
		while(c != '*' && c != EOF){
			c = input();
		}
		if(c == EOF){
			printf("Error type B at line %d: The comment hasn't been matched!\n",yylineno-1);
			exit(-1);
		}
		char c1 = input();
		if(c1 != '/'){
			c = input();
			continue;
		}
			break;
	}
}
