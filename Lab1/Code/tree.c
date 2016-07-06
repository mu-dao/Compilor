#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "tree.h"
/*
int get_int(char *s){
	char *p=s;
	if(strlen(p) <= 1){
		return atoi(p);
	}

	else if(p[0] == '0'){
		if(p[1] == 'x' || p[1] == 'X'){
			return (int)strtoul(p+2,0,16);
		}
		else{
			return (int)strtoul(p+1,0,8);
		}
	}
	else return atoi(p);
}
float get_float(char *s){
	char *p=s;
	int i=0;
	while(p[i] != '\0'){
		if((p[i] == 'e')||(p[i] == 'E'))
			return 1.0;//float number is not what i asked to do
		i++;
	}
	return atof(s);
}
*/
struct tree *newtree(char* name,int is_token,int num,...){
	va_list valist;
	struct tree *father = (struct tree*)malloc(sizeof(struct tree));
	struct tree *temp = (struct tree*)malloc(sizeof(struct tree));

	father->name = name;
	father->is_token=is_token;
	va_start(valist,num);

	if(num>0){
		temp = va_arg(valist,struct tree*);
		father->l = temp;
		father->line = temp->line;

		int i;
		for(i=0;i<num-1;i++){
			temp->r = va_arg(valist,struct tree*);
			temp = temp->r;
		}
	}
	else{
		int lineno = va_arg(valist,int);
		father->line = lineno;

		if((!strcmp(father->name,"ID"))||(!strcmp(father->name,"TYPE"))){
			char *t = (char*)malloc(sizeof(char*)*40);
			strcpy(t,yytext);
			father->type = t;
		}
		else if(!strcmp(father->name,"INT")){
			father->int_value = atoi(yytext);  //get_int() can get the value of base8 or base16 INT number;
		}
		else if(!strcmp(father->name,"FLOAT")){
			father->float_value = atof(yytext);
		}
	}
	
	return father;
}

void print_tree(struct tree *father,int level){
	if(father != NULL){
		if(father->line != -1){
			int i;
			for(i = 0;i < level;i++){
				printf("  ");
			}
			printf("%s ",father->name);
			if((!strcmp(father->name,"ID"))||(!strcmp(father->name,"TYPE"))){
				printf(": %s",father->type);
			}
			else if(!strcmp(father->name,"INT")){
				printf(": %d",father->int_value);
			}
			else if(!strcmp(father->name,"FLOAT")){
				printf(": %f",father->float_value);
			}
			else if(father->is_token == 0){
				printf("(%d)",father->line);
			}
			printf("\n");
		}
		print_tree(father->l,level+1);
		print_tree(father->r,level);
	}
}
