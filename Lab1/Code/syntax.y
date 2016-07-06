%{
#include <unistd.h>
#include <stdio.h>
#include "lex.yy.c"
#define YYERROR_VERBOSE

int is_error;

%}


%union{
	struct tree* tree;
}
%token <tree>SEMI COMMA ASSIGNOP RELOP PLUS MINUS STAR DIV AND OR DOT NOT TYPE LP RP LB RB LC RC STRUCT RETURN IF ELSE WHILE
%token <tree>INT FLOAT ID

%type <tree>ExtDecList Program ExtDefList ExtDef Specifier StructSpecifier DefList ParamDec VarList CompSt StmtList Stmt DecList Exp OptTag Tag VarDec FunDec Def Args Dec 
%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT
%left DOT LB RB LP RP


%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE
%%
Program			:		ExtDefList					{$$ = newtree("Program",0,1,$1);if(is_error == 0) print_tree($$,0);}
				;
ExtDefList		:		/*empty*/					{$$ = newtree("ExtDefList",0,0,-1);}
				|		ExtDef ExtDefList			{$$ = newtree("ExtDefList",0,2,$1,$2);}
				;
ExtDef			:		Specifier ExtDecList SEMI	{$$ = newtree("ExtDef",0,3,$1,$2,$3);}
				|		Specifier SEMI				{$$ = newtree("ExtDef",0,2,$1,$2);}
				|		Specifier FunDec CompSt		{$$ = newtree("ExtDef",0,3,$1,$2,$3);}
				|		error SEMI					{is_error = 1;}
				;
ExtDecList		:		VarDec						{$$ = newtree("ExtDecList",0,1,$1);}
				|		VarDec COMMA ExtDecList		{$$ = newtree("ExtDecList",0,3,$1,$2,$3);}
				;


Specifier		:		TYPE						{$$ = newtree("Specifier",0,1,$1);}
				|		StructSpecifier				{$$ = newtree("Specifier",0,1,$1);}
				;
StructSpecifier	:		STRUCT OptTag LC DefList RC	{$$ = newtree("StructSpecifier",0,5,$1,$2,$3,$4,$5);}
				|		STRUCT Tag					{$$ = newtree("StructSpecifier",0,2,$1,$2);}
				;
OptTag			:		ID							{$$ = newtree("OptTag",0,1,$1);}
				|		/*empty*/					{$$ = newtree("OptTag",0,0,-1);}
				;
Tag				:		ID							{$$ = newtree("Tag",0,1,$1);}
				;


VarDec			:		ID							{$$ = newtree("VarDec",0,1,$1);}
				|		VarDec LB INT RB			{$$ = newtree("VarDec",0,4,$1,$2,$3,$4);}
				;
FunDec			:		ID LP VarList RP			{$$ = newtree("FunDec",0,4,$1,$2,$3,$4);}
				|		ID LP RP					{$$ = newtree("FunDec",0,3,$1,$2,$3);}
				|		error RP					{is_error = 1;}
				;
VarList			:		ParamDec COMMA VarList		{$$ = newtree("VarList",0,3,$1,$2,$3);}
				|		ParamDec					{$$ = newtree("VarList",0,1,$1);}
				;
ParamDec		:		Specifier VarDec			{$$ = newtree("ParamDec",0,2,$1,$2);}
				|		error COMMA					{is_error = 1;}
				|		error RB					{is_error = 1;}
				;


CompSt			:		LC DefList StmtList RC		{$$ = newtree("CompSt",0,4,$1,$2,$3,$4);}
				|		error RC					{is_error = 1;}
				;
StmtList		:		Stmt StmtList				{$$ = newtree("StmtList",0,2,$1,$2);}
				|		/*empty*/					{$$ = newtree("StmtList",0,0,-1);}
				;
Stmt			:		Exp SEMI					{$$ = newtree("Stmt",0,2,$1,$2);}
				|		CompSt						{$$ = newtree("Stmt",0,1,$1);}
				|		RETURN Exp SEMI				{$$ = newtree("Stmt",0,3,$1,$2,$3);}
				|		IF LP Exp RP Stmt %prec LOWER_THAN_ELSE {$$ = newtree("Stmt",0,5,$1,$2,$3,$4,$5);} 
				|		IF LP Exp RP Stmt ELSE Stmt	{$$ = newtree("Stmt",0,7,$1,$2,$3,$4,$5,$6,$7);}
				|		WHILE LP Exp RP Stmt		{$$ = newtree("Stmt",0,5,$1,$2,$3,$4,$5);}
				|		error SEMI					{is_error = 1;}
				|		error RP                    {is_error = 1;}				
				;



DefList			:		Def DefList					{$$ = newtree("DefList",0,2,$1,$2);}
				|		/*empty*/					{$$ = newtree("DefList",0,0,-1);}
				;
Def				:		Specifier DecList SEMI		{$$ = newtree("Def",0,3,$1,$2,$3);}
/*				|		error SEMI					{is_error = 1;}*/
				;
DecList			:		Dec							{$$ = newtree("DecList",0,1,$1);}
				|		Dec COMMA DecList			{$$ = newtree("DecList",0,3,$1,$2,$3);}
				;
Dec				:		VarDec						{$$ = newtree("Dec",0,1,$1);}
				|		VarDec ASSIGNOP Exp			{$$ = newtree("Dec",0,3,$1,$2,$3);}
				;


Exp				:		Exp ASSIGNOP Exp			{$$ = newtree("Exp",0,3,$1,$2,$3);}
				|		Exp AND Exp					{$$ = newtree("Exp",0,3,$1,$2,$3);}
				|		Exp OR Exp					{$$ = newtree("Exp",0,3,$1,$2,$3);}
				|		Exp RELOP Exp				{$$ = newtree("Exp",0,3,$1,$2,$3);}
				|		Exp PLUS Exp				{$$ = newtree("Exp",0,3,$1,$2,$3);}
				|		Exp MINUS Exp				{$$ = newtree("Exp",0,3,$1,$2,$3);}
				|		Exp STAR Exp				{$$ = newtree("Exp",0,3,$1,$2,$3);}
				|		Exp DIV Exp					{$$ = newtree("Exp",0,3,$1,$2,$3);}
				|		LP Exp RP					{$$ = newtree("Exp",0,3,$1,$2,$3);}
				|		MINUS Exp					{$$ = newtree("Exp",0,2,$1,$2);}
				|		NOT Exp						{$$ = newtree("Exp",0,2,$1,$2);}
				|		ID LP Args RP				{$$ = newtree("Exp",0,4,$1,$2,$3,$4);}
				|		ID LP RP					{$$ = newtree("Exp",0,3,$1,$2,$3);}
				|		Exp LB Exp RB				{$$ = newtree("Exp",0,4,$1,$2,$3,$4);}
				|		Exp DOT ID					{$$ = newtree("Exp",0,3,$1,$2,$3);}
				|		ID							{$$ = newtree("Exp",0,1,$1);}
				|		INT							{$$ = newtree("Exp",0,1,$1);}
				|		FLOAT						{$$ = newtree("Exp",0,1,$1);}
				;
Args			:		Exp COMMA Args				{$$ = newtree("Args",0,3,$1,$2,$3);}
				|		Exp							{$$ = newtree("Args",0,1,$1);}
				;


%%
yyerror(char *msg){
	int len = strlen("Syntax error, ");
	fprintf(stderr,"Error type B at Line %d:%s\n",yylineno,msg+len);
}

