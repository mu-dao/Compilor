#include "semantic.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#pragma warning(disable:4996)

extern void ExtDefList(tree* extdeflist);
extern void ExtDef(tree*);
extern void ExtDecList(tree* extdeclist);
extern void Specifier(tree*);
extern void StructSpecifier(tree* structspecifier);
extern void OptTag(tree* opttag);
extern void Tag(tree* tag);
extern void VarDec(tree* vardec);
extern void FunDec(tree* fundec);
extern void VarList(tree* varlist);
extern void ParamDec(tree* paramdec);
extern void CompSt(tree* compst);
extern void StmtList(tree* stmtlist);
extern void Stmt(tree* stmt);
extern void DefList(tree* deflist);
extern void Def(tree* def);
extern void DecList(tree* declist);
extern void Dec(tree* dec);
extern void Exp(tree* exp,Operand op);
extern void Args(tree* args,FieldList fl,bool isWrite);
extern void Cond(tree* cond,Operand op1,Operand op2);

char anonyST[16] = "0a";
int dep = 0;
int depth = -1;		//for array
char arrayName[16];	//store array name

void initTable(){
	int i = 0;
	for(;i < 1000;i++){
		symbol[i] = NULL;
	}
}

unsigned int hash(char* name){
	unsigned int val = 0, i;
	for(; *name; ++name){
		val = (val << 2) + *name;
		if(i = val & ~1000)
			val = (val ^ (i >> 12)) & 1000;
	}
	return val;
}

bool checkTable(char* id){
	unsigned int pos = hash(id);
	if(symbol[pos]  == NULL)
		return false;
	SymbolTable temp = symbol[pos];
	while(temp != NULL && strcmp(temp->name,id) != 0){
		temp = temp->next;
	}
	if(temp == NULL)
		return false;
	else if((temp->dep != dep) && (temp->mark != STRUCT)){
		return false;
	}
	return true;
}

bool checkID(char* id){
	unsigned int pos = hash(id);
	if(symbol[pos]  == NULL)
		return false;
	SymbolTable temp = symbol[pos];
	while(temp != NULL && strcmp(temp->name,id) != 0){
		temp = temp->next;
	}
	if(temp == NULL)
		return false;
	else if((temp->mark == FUNCTION)||(temp->dep > dep))
		return false;
	return true;
}

bool isInStruct(char* st,char* id){
	unsigned int pos = hash(st);
	SymbolTable temp = symbol[pos];
	while(strcmp(temp->name,st) != 0 && temp != NULL){
		temp = temp->next;
	}
	if(temp == NULL)
		return false;
	FieldList fl = temp->type->structure;
	while(fl != NULL){
		if(strcmp(fl->name,id) == 0)
			return true;
		fl = fl->tail;
	}
	return false;
}

FieldList getField(char* id){
	unsigned int pos = hash(id);
	SymbolTable temp = symbol[pos];
	while(strcmp(temp->name,id) != 0 && temp != NULL){
		temp = temp->next;
	}
	return temp->type->structure;
}

int getType(char* id){
	unsigned int pos = hash(id);
	SymbolTable temp = symbol[pos];
	while(strcmp(temp->name,id) != 0 && temp != NULL){
		temp = temp->next;
	}
	return temp->type->kind;
}

int getArrayType(char* id,int depth){
//	printf("getArrayType begin\n");
	unsigned int pos = hash(id);
	SymbolTable temp = symbol[pos];
	while(temp != NULL && strcmp(temp->name,id) != 0){
		temp = temp->next;
	}
	if(temp == NULL)
		return 0;
	Type tp = temp->type;
	for(; depth >= 0; depth--){
		if(tp->array.elem != NULL)
			tp = tp->array.elem;
	}
//	printf("getArrayType finish!\n");
	return tp->kind;
}
bool isinArray(char *id){
	unsigned int pos = hash(id);
	SymbolTable temp = symbol[pos];
	while(strcmp(temp->name,id) != 0 && temp != NULL){
		temp = temp->next;
	}
	Type tp = temp->type;
	if(tp->kind == ARRAY)
		return true;
	else return false;
}
bool checkFunc(char* id){
//	printf("checkFunc begin!\n");
	unsigned int pos = hash(id);
	if(symbol[pos] == NULL){
		return false;
	}
	SymbolTable temp = symbol[pos];
	while(temp != NULL && strcmp(temp->name,id) != 0){
		temp = temp->next;
	}
	if(temp == NULL){
		return false;
	}
	else if(temp->mark != FUNCTION){
		return false;
	}
	return true;
}

bool checkLeft(tree* p){
//	printf("checkLeft begin!\n");
	if(strcmp(p->name,"ID") == 0 && p->next == NULL)
		return true;
	else if(p->next != NULL && strcmp(p->next->name,"LB") == 0)
		return true;
	else if(p->next != NULL && strcmp(p->next->name,"DOT") == 0)
		return true;
	return false;
}

bool name_equal(tree* p,tree* q){
//	printf("name_equal begin\n");
	unsigned int pos = hash(p->id);
	SymbolTable temp1 = symbol[pos];
	while(temp1 != NULL && strcmp(temp1->name,p->id) != 0){
		temp1 = temp1->next;
	}
	pos = hash(q->id);
	SymbolTable temp2 = symbol[pos];
	while(temp2 != NULL && strcmp(temp2->name,q->id) != 0){
		temp2 = temp2->next;
	}
	if(p->kind == STRUCT){
		if(!strcmp(temp1->father,temp2->father))
			return true;
		return false;
	}
	else{
		Type tp1 = temp1->type->array.elem;
		Type tp2 = temp2->type->array.elem;
		for(; tp1->kind == ARRAY; tp1 = tp1->array.elem)
			p->depth--;
		for(; tp2->kind == ARRAY; tp2 = tp2->array.elem)
			q->depth--;
		if(p->depth == q->depth && tp1->kind == tp2->kind)
			return true;
		return false;
	}
}

void insertFunc(char* id,int kind,char* s){
//	printf("insertFunc begin!\n");
	unsigned int pos = hash(id);
	SymbolTable temp = malloc(sizeof(struct SymbolTable_));
	strcpy(temp->name,id);
	temp->type = malloc(sizeof(struct Type_));
	temp->type->kind = kind;
	strcpy(temp->type->father,s);
	temp->mark = FUNCTION;
	temp->dep = dep;
	temp->next = symbol[pos];
	symbol[pos] = temp;
//	printf("insertFunc finish!\n");
}

void changeType(tree* p,int col,int kind){
//	printf("changeType begin!\n");
	unsigned int pos = hash(p->id);
	SymbolTable temp = symbol[pos];
	while(temp != NULL && strcmp(temp->name,p->id) != 0){
		temp = temp->next;
	}
	Type tp = temp->type;
	while(tp->kind == ARRAY)
		tp = tp->array.elem;
	tp->kind = ARRAY;
	tp->array.size = col;
	tp->array.elem = malloc(sizeof(struct Type_));
	tp->array.elem->kind = kind;
//	printf("changeType finish!\n");
}

void insertField(char* id,char* stofunc,int kind){
//	printf("insertField begin!\n");
	unsigned int pos = hash(stofunc);
	SymbolTable temp = symbol[pos];
	while(temp != NULL && strcmp(temp->name,stofunc) != 0){
		temp = temp->next;
	}
	FieldList fl = malloc(sizeof(struct FieldList_));
	strcpy(fl->name,id);
	fl->type = malloc(sizeof(struct Type_));
	fl->type->kind = kind;
	if(temp->type->structure == NULL){
		fl->tail = NULL;
		temp->type->structure = fl;
	}
	else{
		FieldList last = temp->type->structure;
		while(last->tail != NULL)
			last = last->tail;
		last->tail = fl;
	}
//	printf("insertField finish!\n");
}

void insertFieldST(char* id,char* stofunc,char* stid){
//	printf("insertFieldST begin!\n");
	unsigned int pos = hash(stofunc);
	SymbolTable temp = symbol[pos];
	while(temp != NULL && strcmp(temp->name,stofunc) != 0){
		temp = temp->next;
	}
	FieldList fl = malloc(sizeof(struct FieldList_));
	strcpy(fl->name,id);
	fl->type = malloc(sizeof(struct Type_));
	fl->type->kind = STRUCT;
	
	unsigned int fatherPos = hash(stid);
	SymbolTable father = symbol[fatherPos];
	while(father != NULL && strcmp(father->name,stid) != 0){
		father = father->next;
	}
	fl->type->structure = father->type->structure;
	strcpy(fl->struct_name,father->name);
	if(temp->type->structure == NULL){
		fl->tail = NULL;
		temp->type->structure = fl;
	}
	else{
		FieldList last = temp->type->structure;
		while(last->tail != NULL)
			last = last->tail;
		last->tail = fl;
	}
//	printf("insertFieldST finish!\n");
}

void insertID(char* id,int kind){
//	printf("insertID begin!\n");
	unsigned int pos = hash(id);
	SymbolTable temp = malloc(sizeof(struct SymbolTable_));
	strcpy(temp->name,id);
	temp->type = malloc(sizeof(struct Type_));
	temp->type->kind = kind;
	temp->mark = kind;
	temp->dep = dep;
	temp->next = symbol[pos];
	symbol[pos] = temp;
//	printf("insertID finish!\n");
}
void insertST(char* id,char* stid){
//	printf("insertST begin!\n");
	unsigned int pos = hash(id);
	SymbolTable temp = malloc(sizeof(struct SymbolTable_));
	strcpy(temp->name,id);
	temp->type = malloc(sizeof(struct Type_));
	temp->type->kind = STRUCT;
	temp->type->structure = malloc(sizeof(struct FieldList_));

	unsigned int fatherPos = hash(stid);
	SymbolTable father = symbol[fatherPos];
	while(father != NULL && strcmp(father->name,stid) != 0){
		father = father->next;
	}
	if(father == NULL){
		return;
	}
	temp->type->structure = father->type->structure;
	temp->mark = -1;
	temp->dep = dep;
	strcpy(temp->father,stid);
	temp->next = symbol[pos];
	symbol[pos] = temp;

//	printf("insertST finish!\n");
}
void deleteID(int depth){
	int i = 0;
	SymbolTable temp1,temp2;
	for(;i < 1000;i++){
		if(symbol[i] == NULL)
			continue;
		else{
			temp1 = symbol[i];
			temp2 = symbol[i]->next;
			while(temp2 != NULL){
				if(temp2->dep == depth){
					temp1->next = temp2->next;
				}
				temp2 = temp2->next;
			}
			if(temp1->dep == depth){
				symbol[i] = symbol[i]->next;
			}
		}
	}
}

bool if_struct_match(tree* p,char* name2){
	unsigned int pos = hash(p->id);
	if(symbol[pos]  == NULL)
		return false;
	SymbolTable temp = symbol[pos];
	while(temp != NULL && strcmp(temp->name,p->id) != 0){
		temp = temp->next;
	}
	if(temp == NULL)
		return false;
	else{
		char *s = temp->father;
		if(strcmp(s,name2) == 0)
			return true;
	}
	return false;
}

void initial_depth_arrayName(){
	depth = -1;
	strcpy(arrayName," ");
}
int get_struct_size(char* id){
	unsigned int pos = hash(id);
	SymbolTable temp = symbol[pos];
	while(strcmp(temp->name,id) != 0 && temp != NULL){
		temp = temp->next;
	}
	int size = 0;
	FieldList fl = temp->type->structure;
	while(fl != NULL){
		size++;
		fl = fl->tail;
	}
	return size * 4;
}
int get_struct_offset(char* id,char* field){
	unsigned int pos = hash(id);
	SymbolTable temp = symbol[pos];
	while(strcmp(temp->name,id) != 0 && temp != NULL){
		temp = temp->next;
	}
	char st[32];
	strcpy(st,temp->father);
	pos = hash(st);
	temp = symbol[pos];
	while(strcmp(temp->name,st) != 0 && temp != NULL){
		temp = temp->next;
	}
	int size = 0;
	FieldList fl = temp->type->structure;
	while(strcmp(fl->name,field) != 0){
		fl = fl->tail;
		size++;
	}
	return size * 4;
}
int get_array_size(char* id){
	unsigned int pos = hash(id);
	SymbolTable temp = symbol[pos];
	while(strcmp(temp->name,id) != 0 && temp != NULL){
		temp = temp->next;
	}
	return 4 * temp->type->array.size;
}
void Program(tree* program){
	initTable();
	
	insertFunc("read",INT,"");
	insertFunc("write",INT,"");
	insertID("writeParam",INT);
	insertField("writeParam","write",INT);
	
	ExtDefList(program->children);
}

void ExtDefList(tree* extdeflist){
//	printf("ExtDefList begin!\n");
	if(extdeflist->line != -1){
		tree* p = extdeflist->children;
		ExtDef(p);
		ExtDefList(p->next);
	}
//	printf("ExtDefList finish!\n");
}

void ExtDef(tree* extdef){
//	printf("ExtDef begin!\n");
	tree* p = extdef->children;
	Specifier(p);
	if(strcmp(p->next->name,"ExtDecList") == 0){	//ExtDef->Specifier ExtDecList SEMI
		p->next->kind = p->kind;
		strcpy(p->next->id,p->id);
		ExtDecList(p->next);
	}
	else if(strcmp(p->next->name,"FunDec") == 0){	//ExtDef->Specifier FunDec CompSt
		p->next->kind = p->kind;
		strcpy(p->next->id,p->id);	
		//FunDec(p->next);
		FunDec(p);
		p->next->next->kind = p->kind;
		CompSt(p->next->next);
	}
//	printf("ExtDef finish!\n");
}

void ExtDecList(tree* extdeclist){
//	printf("ExtDecList begin!\n");
	tree* p = extdeclist->children;
	p->kind = extdeclist->kind;
	strcpy(p->id,extdeclist->id);
	VarDec(p);										//ExtDecList->VarDec

	printf("p->id = %s\np->kind = %d\n",p->id,p->kind);
	if(p->next != NULL){							//ExtDecList->VarDec COMMA ExtDecList
		p->next->next->kind = extdeclist->kind;
		strcpy(p->next->next->id,extdeclist->id);
		ExtDecList(p->next->next);
	}
//	printf("ExtDecList finish!\n");
}

void Specifier(tree* specifier){
//	printf("Specifier begin!\n");
	tree* p = specifier->children;
	if(strcmp(p->name,"TYPE") == 0){		//Specifier->TYPE
		if(strcmp(p->id,"int") == 0)
			specifier->kind = INT;
		else
			specifier->kind = FLOAT;
	}
	else{								//Specifier->StructSpecifier
		StructSpecifier(p);
		specifier->kind = STRUCT;
		strcpy(specifier->id,p->id);
	}
//	printf("Specifier finish!\n");
}

void StructSpecifier(tree* structspecifier){
//	printf("StructSpecifier begin!\n");
	tree* p = structspecifier->children->next;
	if(p->next != NULL){						//StructSpecifier->STRUCT OptTag LC DefList RC
		if(strcmp(p->name,"OptTag") == 0){			
			if(p->line == -1){
				strcpy(p->id,anonyST);
				anonyST[0]++;
			}
			OptTag(p);
			strcpy(structspecifier->id,p->id);
		}
		if(!strcmp(p->next->name,"LC") && !strcmp(p->next->next->name,"DefList")){
			strcpy(p->next->next->stofunc,p->id);
			DefList(p->next->next);
		}
		else if(!strcmp(p->next->name,"DefList")){
			strcpy(p->next->stofunc,p->id);
			DefList(p->next);
		}
	}
	else{										//StructSpecifier->STRUCT Tag
		Tag(p);
		strcpy(structspecifier->id,p->id);
	}
//	printf("StructSpecifier finish!\n");
}

void OptTag(tree* opttag){
//	printf("OptTag begin!\n");
	if(opttag != NULL){
		if(opttag->children!= NULL){
			if(checkTable(opttag->children->id)){
				printf("Error type 16 at Line %d: Redefined structure \"%s\".\n",opttag->children->line,opttag->children->id);
				return;
			}
			strcpy(opttag->id,opttag->children->id);	
			insertID(opttag->children->id,STRUCT);
		}
		else{
			insertID(opttag->id,STRUCT);
		}
	}
//	printf("OptTag finish!\n");
}

void Tag(tree* tag){
//	printf("Tag begin!\n");
	strcpy(tag->id,tag->children->id);
	if(!checkID(tag->children->id)){
		printf("Error type 17 at Line %d: Undefined structure \"%s\".\n",tag->children->line,tag->children->id);
		return;
	}
//	printf("Tag finish!\n");
}

void VarDec(tree* vardec){
//	printf("VarDec begin!\n");
	tree* p = vardec->children;
	//VarDec->ID
	if(p->next == NULL){					
		if(checkTable(p->id) || checkFunc(p->id)){
			if(strcmp(vardec->stofunc,"") != 0){
				printf("Error type 15 at Line %d: Redefined field \"%s\".\n",p->line,p->id);
			}
			else{
				printf("Error type 3 at Line %d: Redefined variable \"%s\".\n",p->line,p->id);
			}
			return;
		}
		if(vardec->kind != STRUCT){
			insertID(p->id,vardec->kind);
		}
		else{
			insertST(p->id,vardec->id);
			
			Operand op = malloc(sizeof(struct Operand_));
			op->kind = TEMP;
			op->u.var_no = varCount++;
			InterCode dec = malloc(sizeof(struct InterCode_));
			dec->kind = DEC;
			dec->u.dec.op = op;
			dec->u.dec.size = get_struct_size(p->id);
			insertCode(dec);

			Operand var = malloc(sizeof(struct Operand_));
			var->kind = VARIABLE;
			strcpy(var->u.name,p->id);
			InterCode addr = malloc(sizeof(struct InterCode_));
			addr->kind = ADDR;
			addr->u.assign.left = var;
			addr->u.assign.right = op;
			insertCode(addr);

		}
		if(strcmp(vardec->stofunc,"") != 0){
			if(vardec->kind != STRUCT){
				insertField(p->id,vardec->stofunc,vardec->kind);
			}
			else{
				insertFieldST(p->id,vardec->stofunc,vardec->id);
			}
		}
		strcpy(vardec->id,p->id);
	}
	//VarDec->VarDec LB INT RB
	else{
		p->kind = vardec->kind;
		strcpy(p->id,vardec->id);
		strcpy(p->stofunc,vardec->stofunc);
		if(strcmp(p->children->name,"VarDec") == 0){
			printf("Cannot translate: Code contains variables of multi-dimensional array type or parameters of array type!\n");
			exit(0);
		}
		VarDec(p);
		strcpy(vardec->id,p->id);
		changeType(p,p->next->next->int_value,vardec->kind);
	}
//	printf("VarDec finish!\n");
}

void FunDec(tree* fundec){
//	printf("FunDec begin!\n");
	tree* p = fundec->next->children;
	if(checkTable(p->id) || checkFunc(p->id)){
		printf("Error type 4 at Line %d: Redefined function \"%s\".\n",p->line,p->id);
		strcpy(p->id,"Redefined");
	}
	char* s;
	if(fundec->kind == STRUCT)
		s = fundec->id;
	else s = "";
	insertFunc(p->id,fundec->next->kind,s);

	Operand func = malloc(sizeof(struct Operand_));
	func->kind = FUNCTION_O;
	strcpy(func->u.name,p->id);
	InterCode code = malloc(sizeof(struct InterCode_));
	code->kind = FUNCTION_I;
	code->u.one.op = func;
	insertCode(code);
	
	//FunDec->ID LP VarList RP
	if(p->next->next->next != NULL){
		strcpy(p->next->next->stofunc,p->id);
		VarList(p->next->next);
	}
//	printf("FunDec finish!\n");
}

void VarList(tree* varlist){
//	printf("VarList begin!\n");
	dep++;
	tree* p = varlist->children;
	strcpy(p->stofunc,varlist->stofunc);
	ParamDec(p);
	dep--;
	//VarList->ParamDec COMMA VarList
	if(p->next != NULL){
		strcpy(p->next->next->stofunc,varlist->stofunc);
		VarList(p->next->next);
	}
//	printf("VarList finish!\n");
}

void ParamDec(tree* paramdec){
//	printf("ParamDec begin!\n");
	tree* p = paramdec->children;
	Specifier(p);
	p->next->kind = p->kind;
	strcpy(p->next->id,p->id);
	strcpy(p->next->stofunc,paramdec->stofunc);
	VarDec(p->next);
	
	Operand param = malloc(sizeof(struct Operand_));
	param->kind = VARIABLE;
	strcpy(param->u.name,p->next->id);
	InterCode paramCode = malloc(sizeof(struct InterCode_));
	paramCode->kind = PARAM;
	paramCode->u.one.op = param;
	insertCode(paramCode);
//	printf("ParamDec finish!\n");
}

void CompSt(tree* compst){
//	printf("CompSt begin!\n");
	dep++;
	tree* p = compst->children;
	if(strcmp(p->next->name,"DefList") == 0)
		DefList(p->next);
	if(p->next->next->line != -1){
		if(p->next->next->next->line != -1){
			p->next->next->kind = compst->kind;
			StmtList(p->next->next);
		}
		else if(!strcmp(p->next->name,"StmtList")){
			p->next->kind = compst->kind;
			StmtList(p->next);
		}
	}
	deleteID(dep);
	dep--;
//	printf("CompSt finish!\n");
}

void StmtList(tree* stmtlist){
//	printf("StmtList begin!\n");
	if(stmtlist->line != -1){
		tree* p = stmtlist->children;
		p->kind = stmtlist->kind;
		Stmt(p);
		if(p->next->line != -1){
			p->next->kind = stmtlist->kind;
			StmtList(p->next);
		}
	}
//	printf("StmtList finish!\n");
}

void Stmt(tree* stmt){
//	printf("Stmt begin!\n");
	initial_depth_arrayName();
	tree* p = stmt->children;
	if(!strcmp(p->name,"Exp")){
		Exp(p,NULL);
	}
	else if(!strcmp(p->name,"CompSt")){
		p->kind = stmt->kind;
		CompSt(p);
	}
	else if(!strcmp(p->name,"RETURN")){
		Operand op = malloc(sizeof(struct Operand_));
		op->kind = TEMP;
		op->u.var_no = varCount++;

		Exp(p->next,op);
		
		if(p->next->kind != stmt->kind){
			printf("Error type 8 at Line %d: Type mismatched for return.\n",p->line);
			return;
		}
		InterCode code = malloc(sizeof(struct InterCode_));
		code->kind = RETURN;
		code->u.one.op = op;
		insertCode(code);
	}
	else if(strcmp(p->name,"IF") == 0){
		Operand lab1 = malloc(sizeof(struct Operand_));
		lab1->kind = LABEL;
		lab1->u.var_no = labCount++;
		
		Operand lab2 = malloc(sizeof(struct Operand_));
		lab2->kind = LABEL;
		lab2->u.var_no = labCount++;

		p = p->next->next;
		Cond(p,lab1,lab2);
		
		if(p->kind != INT){
			printf("Error type 7 at Line %d: INT type expected.\n",p->line);
			return;
		}

		InterCode code1 = malloc(sizeof(struct InterCode_));
		code1->kind = LABEL_I;
		code1->u.one.op = lab1;
		insertCode(code1);

		p = p->next->next;
		p->kind = stmt->kind;
		Stmt(p);

		InterCode code2 = malloc(sizeof(struct InterCode_));
		code2->kind = LABEL_I;
		code2->u.one.op = lab2;

		if(p->next == NULL){
			insertCode(code2);
		}
		else{
			Operand lab3 = malloc(sizeof(struct Operand_));
			lab3->kind = LABEL;
			lab3->u.var_no = labCount++;
			InterCode code3 = malloc(sizeof(struct InterCode_));
			code3->kind = GOTO;
			code3->u.one.op = lab3;
			insertCode(code3);
			insertCode(code2);

			p = p->next->next;
			p->kind = stmt->kind;
			Stmt(p);

			InterCode code4 = malloc(sizeof(struct InterCode_));
			code4->kind = LABEL_I;
			code4->u.one.op = lab3;
			insertCode(code4);
		}
	}
	else{
		Operand lab1 = malloc(sizeof(struct Operand_));
		lab1->kind = LABEL;
		lab1->u.var_no = labCount++;
		
		Operand lab2 = malloc(sizeof(struct Operand_));
		lab2->kind = LABEL;
		lab2->u.var_no = labCount++;
		
		Operand lab3 = malloc(sizeof(struct Operand_));
		lab3->kind = LABEL;
		lab3->u.var_no = labCount++;
		
		InterCode code1 = malloc(sizeof(struct InterCode_));
		code1->kind = LABEL_I;
		code1->u.one.op = lab1;
		insertCode(code1);

		p = p->next->next;
		Cond(p,lab2,lab3);
		
		if(p->kind != INT){
			printf("Error type 7 at Line %d: INT type expected.\n",p->line);
			return;
		}

		InterCode code2 = malloc(sizeof(struct InterCode_));
		code2->kind = LABEL_I;
		code2->u.one.op = lab2;
		insertCode(code2);

		p = p->next->next;
		p->kind = stmt->kind;
		Stmt(p);

		InterCode gotoCode = malloc(sizeof(struct InterCode_));
		gotoCode->kind = GOTO;
		gotoCode->u.one.op = lab1;
		insertCode(gotoCode);
		
		InterCode code3 = malloc(sizeof(struct InterCode_));
		code3->kind = LABEL_I;
		code3->u.one.op = lab3;
		insertCode(code3);
	}
	/*
	else {
		char temp[16];
		strcpy(temp,p->name);
		p = p->next->next;
		Exp(p,NULL);
		if(p->kind != INT){
			printf("Error type 7 at Line %d: INT type expected.\n",p->line);
			return;
		}
		p = p->next->next;
		p->kind = stmt->kind;
		Stmt(p);
		if(!strcmp(temp,"IF")){
			if(p->next != NULL){
				p = p->next->next;
				p->kind = stmt->kind;
				Stmt(p);
			}
		}
		
	}
	*/
//	printf("Stmt finish!\n");
}

void DefList(tree* deflist){
//	printf("DefList begin!\n");
	if(deflist->line != -1){
		tree* p = deflist->children;
		strcpy(p->stofunc,deflist->stofunc);
		if(p->next->line != -1){
			strcpy(p->next->stofunc,deflist->stofunc);
		}
		Def(p);
		DefList(p->next);
	}
//	printf("DefList finish!\n");
}

void Def(tree* def){
//	printf("Def begin!\n");
	tree* p = def->children;
	Specifier(p);
	p->next->kind = p->kind;
	strcpy(p->next->id,p->id);
	strcpy(p->next->stofunc,def->stofunc);
	DecList(p->next);
//	printf("Def finish!\n");
}

void DecList(tree* declist){
//	printf("DecList begin!\n");
	tree* p = declist->children;
	p->kind = declist->kind;
	strcpy(p->id,declist->id);
	strcpy(p->stofunc,declist->stofunc);
	Dec(p);
	if(p->next != NULL){		//DecList->Dec COMMA DecList
		p->next->next->kind = declist->kind;
		strcpy(p->next->next->id,declist->id);
		strcpy(p->next->next->stofunc,declist->stofunc);
		DecList(p->next->next);
	}
//	printf("DecList finish!\n");
}

void Dec(tree* dec){
//	printf("Dec begin!\n");
	tree* p = dec->children;
	p->kind = dec->kind;
	strcpy(p->id,dec->id);
	strcpy(p->stofunc,dec->stofunc);
	VarDec(p);

	if(getType(p->id) == ARRAY){
		Operand op = malloc(sizeof(struct Operand_));
		op->kind = TEMP;
		op->u.var_no = varCount++;
		InterCode dec = malloc(sizeof(struct InterCode_));
		dec->kind = DEC;
		dec->u.dec.op = op;
		dec->u.dec.size = get_array_size(p->id);
		insertCode(dec);

		Operand var = malloc(sizeof(struct Operand_));
		var->kind = VARIABLE;
		strcpy(var->u.name,p->id);
		InterCode addr = malloc(sizeof(struct InterCode_));
		addr->kind = ADDR;
		addr->u.assign.left = var;
		addr->u.assign.right = op;
		insertCode(addr);
	}

	if(p->next != NULL){								//Dec->VarDec ASSIGNOP Exp
		if(strcmp(dec->stofunc,"") != 0){
			printf("Error type 15 at Line %d: Assignment in struct.\n",p->line);
			return;
		}
		Operand op = malloc(sizeof(struct Operand_));
		op->kind = VARIABLE;
		strcpy(op->u.name,p->id);

		bool flag = false,func_flag = false;
		char struct_name[32];
		if(strcmp(p->next->next->children->name,"ID") == 0){
			if(checkFunc(p->next->next->children->id)){
				unsigned int pos = hash(p->next->next->children->id);
				SymbolTable temp = symbol[pos];
				while(temp != NULL && strcmp(temp->name,p->next->next->children->id) != 0 ){
					temp = temp->next;
				}
				if(temp->type->kind == STRUCT){
					func_flag = true;
					strcpy(struct_name,temp->type->father);
				}
			}
		}

		Exp(p->next->next,op);

		if(!func_flag){
			if(p->kind != p->next->next->kind || (p->kind == p->next->next->kind && ((p->kind == STRUCT)||(p->kind == ARRAY)) && !name_equal(p,p->next->next))){
				flag = true;
			}
		}
		else{
			if(!if_struct_match(p,struct_name)){
				flag = true;
			}
		}
		if(flag){
			printf("Error type 5 at Line %d: Type mismatched for assignment.\n",p->next->line);
			return;
		}

		if(op->kind != VARIABLE || strcmp(op->u.name,p->id) != 0){
			Operand left = malloc(sizeof(struct Operand_));
			left->kind = VARIABLE;
			strcpy(left->u.name,p->id);
			InterCode code = malloc(sizeof(struct InterCode_));
			code->kind = ASSIGN;
			code->u.assign.left = left;
			code->u.assign.right = op;
			insertCode(code);
		}
	}
//	printf("Dec finish!\n");
}

void Exp(tree* exp,Operand temp){
//	printf("Exp begin!\n");
	tree* p = exp->children;
	//Exp->ID	Exp->FLOAT	Exp->ID	
	if(strcmp(p->name,"INT") == 0 && p->next == NULL){
		exp->kind = INT;
		if(temp != NULL){
			temp->kind = CONSTANT_I;
			temp->u.int_value = p->int_value;
		}
	}
	else if(strcmp(p->name,"FLOAT") == 0 && p->next == NULL){
		exp->kind = FLOAT;
		if(temp != NULL){
			temp->kind = CONSTANT_F;
			temp->u.float_value = p->float_value;
		}
	}
	else if(strcmp(p->name,"ID") == 0 && p->next == NULL){
		if(!checkID(p->id)){
			printf("Error type 1 at Line %d: Undefined variable \"%s\".\n",p->line,p->id);
			return;
		}
		strcpy(exp->id,p->id);
		exp->kind = getType(p->id);
		if(temp != NULL){
			temp->kind = VARIABLE;
			strcpy(temp->u.name,p->id);
		}
	}
	//Exp->Exp LB Exp RB
	else if(strcmp(p->next->name,"LB") == 0){
		//printf("cannot translate:Code contains variables of multi-dimensional array type or parameters of array type\n");
		//exit(0);
		
		if(strcmp(p->id,arrayName) == 0)
			depth++;
		else{
			depth = 0;
			strcpy(arrayName,p->id);
		}

		Operand op1 = malloc(sizeof(struct Operand_));
		op1->kind = TEMP;
		op1->u.var_no = varCount++;
		Operand op2 = malloc(sizeof(struct Operand_));
		op2->kind = TEMP;
		op2->u.var_no = varCount++;

		Exp(p,op1);
		p->depth = depth;
		p->next->next->depth = depth;
		Exp(p->next->next,op2);
		
		if(p->kind != ARRAY && checkID(p->id) && !isinArray(p->id)){
			printf("Error type 10 at Line %d: \"%s\" is not an Array.\n",p->line,p->id);
			return;
		}
		if(p->next->next->kind != INT){
			printf("Error type 12 at Line %d: Array position should be INT.\n",p->next->next->line);
			return;
		}
		strcpy(exp->id,p->id);
		exp->kind = getArrayType(p->id,p->depth);

		Operand offset = malloc(sizeof(struct Operand_));
		offset->kind = CONSTANT_I;
		offset->u.int_value = 4 * op2->u.int_value;

		Operand tp = malloc(sizeof(struct Operand_));
		tp->kind = TEMP;
		tp->u.var_no = varCount++;

		InterCode addrcode = malloc(sizeof(struct InterCode_));
		addrcode->kind = ADD;
		temp->kind = ADDRESS;
		temp->u.var_no = tp->u.var_no;
		addrcode->u.binop.result = tp;
		addrcode->u.binop.op1 = op1;
		addrcode->u.binop.op2 = offset;
		insertCode(addrcode);
	}
	//Exp->Exp DOT ID
	else if(strcmp(p->next->name,"DOT") == 0){
		Operand op1 = malloc(sizeof(struct Operand_));
		op1->kind = TEMP;
		op1->u.var_no = varCount++;

		Exp(p,op1);

		if(p->kind != STRUCT && checkID(p->id)){
			printf("Error type 13 at Line %d: \"%s\" is not a struct.\n",p->line,p->id);
			return;
		}
		if(p->kind == STRUCT && !isInStruct(p->id,p->next->next->id)){
			printf("Error type 14 at Line %d: \"%s\" is not in the struct.\n",p->next->next->line,p->next->next->id);
			return;
		}
		strcpy(exp->id,p->next->next->id);
		exp->kind = getType(p->next->next->id);

		Operand offset = malloc(sizeof(struct Operand_));
		offset->kind = CONSTANT_I;
		offset->u.int_value = get_struct_offset(p->id,p->next->next->id);
		
		Operand tp = malloc(sizeof(struct Operand_));
		tp->kind = TEMP;
		tp->u.var_no = varCount++;

		InterCode addrcode = malloc(sizeof(struct InterCode_));
		addrcode->kind = ADD;
		temp->kind = ADDRESS;
		temp->u.var_no = tp->u.var_no;
		addrcode->u.binop.result = tp;
		addrcode->u.binop.op1 = op1;
		addrcode->u.binop.op2 = offset;
		insertCode(addrcode);
	}

	//Exp->ID LP Args RP	Exp->ID LP RP
	else if(strcmp(p->name,"ID") == 0){
		if(!checkFunc(p->id)){
			if(checkID(p->id)){
				printf("Error type 11 at Line %d: \"%s\" is not a function.\n",p->line,p->id);
				return;
			}
			else{
				printf("Error type 2 at Line %d: Funtion is not defined.\n",p->line);
				return;
			}
		}
		if(strcmp(p->next->next->name,"Args") && getField(p->id)->name != NULL){
			printf("Error type 9 at Line %d:Function need at least one argument.\n",p->line);
			return ;
		}
		if(strcmp(p->next->next->name,"Args") == 0){
			//Args(p->next->next,getField(p->id));
			if(strcmp(p->id,"write") == 0){
				Args(p->next->next,getField(p->id),true);
			}
			else{
				Args(p->next->next,getField(p->id),false);
				Operand op = malloc(sizeof(struct Operand_));
				op->kind = FUNCTION_O;
				strcpy(op->u.name,p->id);
				InterCode code = malloc(sizeof(struct InterCode_));
				code->kind = CALL;
				code->u.assign.left = temp;
				code->u.assign.right = op;
				insertCode(code);
			}		
		}
		else{
			if(strcmp(p->id,"read") == 0){
				InterCode rcode = malloc(sizeof(struct InterCode_));
				rcode->kind = READ;
				rcode->u.one.op = temp;
				insertCode(rcode);
			}
			else{
				Operand op = malloc(sizeof(struct Operand_));
				op->kind = FUNCTION_O;
				strcpy(op->u.name,p->id);
				InterCode code = malloc(sizeof(struct InterCode_));
				code->kind = CALL;
				code->u.assign.left = temp;
				code->u.assign.right = op;
				insertCode(code);
			}
		}
		exp->kind = getType(p->id);
	}
	//Exp->LP Exp RP
	else if(strcmp(p->name,"LP") == 0){
		Exp(p->next,temp);
	}
	else if(strcmp(p->name,"MINUS") == 0){
		Operand op1 = malloc(sizeof(struct Operand_));
		op1->kind = TEMP;
		op1->u.var_no = varCount++;

		Exp(p->next,op1);
		//操作数类型可以为INT或FLOAT
		if(p->next->kind != INT && p->next->kind != FLOAT){
			printf("Error type 7 at Line %d: Type mismatched for operands.\n",p->next->line);
			return;
		}
		//0-x instead of -x
		Operand op2 = malloc(sizeof(struct Operand_));
		op2->kind = CONSTANT_I;
		op2->u.int_value = 0;
		InterCode code = malloc(sizeof(struct InterCode_));
		code->kind = SUB;
		code->u.binop.result = temp;
		code->u.binop.op1 = op2;
		code->u.binop.op2 = op1;
		insertCode(code);

		exp->kind = p->next->kind;
	}

	else if(strcmp(p->name,"NOT") == 0 || strcmp(p->next->name,"AND") == 0 || strcmp(p->next->name,"OR") == 0 || strcmp(p->next->name,"RELOP") == 0){
		Operand lab1 = malloc(sizeof(struct Operand_));
		lab1->kind = LABEL;
		lab1->u.var_no = labCount++;
		
		Operand lab2 = malloc(sizeof(struct Operand_));
		lab2->kind = LABEL;
		lab2->u.var_no = labCount++;
		
		InterCode code = malloc(sizeof(struct InterCode_));
		code->kind = ASSIGN;
		code->u.assign.left = temp;
		
		Operand op = malloc(sizeof(struct Operand_));
		op->kind = CONSTANT_I;
		op->u.int_value = 0;
		code->u.assign.right = op;
		insertCode(code);

		Cond(exp,lab1,lab2);

		InterCode code1 = malloc(sizeof(struct InterCode_));
		code1->kind = LABEL_I;
		code1->u.one.op = lab1;
		insertCode(code1);
		
		Operand op2 = malloc(sizeof(struct Operand_));
		op2->kind = CONSTANT_I;
		op2->u.int_value = 1;
		
		InterCode code2 = malloc(sizeof(struct InterCode_));
		code2->kind = ASSIGN;
		code2->u.assign.left = temp;
		code2->u.assign.right = op2;
		insertCode(code2);
		
		InterCode code3 = malloc(sizeof(struct InterCode_));
		code3->kind = LABEL_I;
		code3->u.one.op = lab2;
		insertCode(code3);

		exp->kind = INT;
	}
	/*
	//单操作数逻辑操作符
	else if(strcmp(p->name,"NOT") == 0){
		Exp(p->next);
		//操作数类型只能为INT
		if(p->next->kind != INT){
			printf("Error type 7 at Line %d: Type mismatched for operands.\n",p->next->line);
			return;
		}
		exp->kind = INT;
	}
	//逻辑操作符
	else if(strcmp(p->next->name,"AND") == 0 || strcmp(p->next->name,"OR") == 0 || strcmp(p->next->name,"RELOP") == 0){
		Exp(p);
		Exp(p->next->next);
		//两个操作数均只能为INT
		if(p->kind != INT || p->next->next->kind != INT){
			printf("Error type 7 at Line %d: Type mismatched for operands.\n",p->next->line);
			return;
		}
		exp->kind = INT;
	}
	*/
	//赋值操作
	else if(strcmp(p->next->name,"ASSIGNOP") == 0){
		Operand leftOp = malloc(sizeof(struct Operand_));
		leftOp->kind = TEMP;
		leftOp->u.var_no = varCount++;
		
		Operand rightOp = malloc(sizeof(struct Operand_));
		rightOp->kind = TEMP;
		rightOp->u.var_no = varCount++;
		
		Exp(p,leftOp);
		
		depth = -1;
		strcpy(arrayName," ");
		Exp(p->next->next,rightOp);
		
		//检查赋值号左端是否只有右值
		if(!checkLeft(p->children)){
			printf("Error type 6 at Line %d: The left-hand side of an assignment must be a variable.\n",p->line);
			return;
		}
        
		//两个操作数类型不同或者结构不等价
		else if((p->kind != p->next->next->kind) || (p->kind == p->next->next->kind) && ((p->kind == STRUCT)||(p->kind == ARRAY)) && (!name_equal(p,p->next->next))){
			printf("Error type 5 at Line %d: Type mismatched for assignment.\n",p->next->line);
			return;
		}

		InterCode code1 = malloc(sizeof(struct InterCode_));
		code1->kind = ASSIGN;
		code1->u.assign.left = leftOp;
		code1->u.assign.right = rightOp;
		insertCode(code1);
		
		InterCode code2 = malloc(sizeof(struct InterCode_));
		code2->kind = ASSIGN;
		code2->u.assign.left = temp;
		code2->u.assign.right = rightOp;
		insertCode(code2);

		exp->kind = ASSIGNMENT;
	}
	//剩下的算数操作符
	else{

		Operand op1 = malloc(sizeof(struct Operand_));
		op1->kind = TEMP;
		op1->u.var_no = varCount++;
		
		Operand op2 = malloc(sizeof(struct Operand_));
		op2->kind = TEMP;
		op2->u.var_no = varCount++;

		Exp(p,op1);
		Exp(p->next->next,op2);
		//操作数类型不同或者不是INT或FLOAT
		if(p->kind != p->next->next->kind || (p->kind == p->next->next->kind && p->kind != INT && p->kind != FLOAT)){
			printf("Error type 7 at Line %d: Type mismatched for operands.\n",p->next->line);
			return ;
		}
		exp->kind = p->kind;

		InterCode code = malloc(sizeof(struct InterCode_));
		code->u.binop.result = temp;
		code->u.binop.op1 = op1;
		code->u.binop.op2 = op2;
		if(strcmp(p->next->name,"PLUS") == 0)
			code->kind = ADD;
		else if(strcmp(p->next->name,"MINUS") == 0)
			code->kind = SUB;
		else if(strcmp(p->next->name,"STAR") == 0)
			code->kind = MUL;
		else if(strcmp(p->next->name,"DIV") == 0)
			code->kind = DIV;
		insertCode(code);
	}
//	printf("Exp finish!\n");
}

void Cond(tree* cond,Operand labT,Operand labF){
	tree* p = cond->children;
	if(strcmp(p->next->name,"AND") == 0){
		Operand lb1 = malloc(sizeof(struct Operand_));
		lb1->kind = LABEL;
		lb1->u.var_no = labCount++;
		
		Cond(p,lb1,labF);
		
		InterCode lb1code = malloc(sizeof(struct InterCode_));
		lb1code->kind = LABEL_I;
		lb1code->u.one.op = lb1;
		insertCode(lb1code);
		
		Cond(p->next->next,labT,labF);
	}
	else if(strcmp(p->next->name,"OR") == 0){
		Operand lb1 = malloc(sizeof(struct Operand_));
		lb1->kind = LABEL;
		lb1->u.var_no = labCount++;
		
		Cond(p,labT,lb1);
		
		InterCode lb1code = malloc(sizeof(struct InterCode_));
		lb1code->kind = LABEL_I;
		lb1code->u.one.op = lb1;
		insertCode(lb1code);
		
		Cond(p->next->next,labT,labF);
	}
	else if(strcmp(p->next->name,"RELOP") == 0){
		Operand op1 = malloc(sizeof(struct Operand_));
		op1->kind = TEMP;
		op1->u.var_no = varCount++;
		Operand op2 = malloc(sizeof(struct Operand_));
		op2->kind = TEMP;
		op2->u.var_no = varCount++;
		Exp(p,op1);
		Exp(p->next->next,op2);
		InterCode code3 = malloc(sizeof(struct InterCode_));
		code3->kind = IF;
		code3->u.four.op1 = op1;
		code3->u.four.op2 = op2;
		code3->u.four.label = labT;
		strcpy(code3->u.four.relop,p->next->id);
		insertCode(code3);
		InterCode gotolbf = malloc(sizeof(struct InterCode_));
		gotolbf->kind = GOTO;
		gotolbf->u.one.op = labF;
		insertCode(gotolbf);
	}
	else if(strcmp(p->name,"NOT") == 0){
		Cond(p->next,labF,labT);
	}
	else{
		Operand op1 = malloc(sizeof(struct Operand_));
		op1->kind = TEMP;
		op1->u.var_no = varCount++;
		Exp(cond,op1);
		InterCode code2 = malloc(sizeof(struct InterCode_));
		code2->kind=IF;
		code2->u.four.op1 = op1;
		strcpy(code2->u.four.relop,"!=");
		Operand op2 = malloc(sizeof(struct Operand_));
		op2->kind = CONSTANT_I;
		op2->u.int_value = 0;
		code2->u.four.op2 = op2;
		code2->u.four.label = labT;
		insertCode(code2);
		InterCode gotolbf = malloc(sizeof(struct InterCode_));
		gotolbf->kind = GOTO;
		gotolbf->u.one.op = labF;
		insertCode(gotolbf);
	}
	cond->kind = INT;
}

void Args(tree* args,FieldList fl,bool isWrite){
//	printf("Args begin!\n");
	Operand op = malloc(sizeof(struct Operand_));
	op->kind = TEMP;
	op->u.var_no = varCount++;

	initial_depth_arrayName();
	tree* p = args->children;
	Exp(p,op);
	FieldList temp = fl;
	int num = 0;
	while(temp){
		temp = temp->tail;
		num++;
	}
	if(fl == NULL){
		printf("Error type 9 at Line %d: Arguments given too much.\n",p->line);
		return ;
	}
	else if((p->kind != fl->type->kind)||(p->kind == fl->type->kind && p->kind == STRUCT && !if_struct_match(p,fl->struct_name))){
		printf("Error type 9 at Line %d: Type mismatched for function arguments.\n",p->line);
		return ;
	}
	num--;
	if(p->next != NULL){
		Args(p->next->next,fl->tail,isWrite);
	}
	else if(num > 0){
		printf("Error type 9 at Line %d: function need more arguments.\n",p->line);
		return ;
	}
	if(isWrite){
		InterCode code = malloc(sizeof(struct InterCode_));
		code->kind = WRITE;
		code->u.one.op = op;
		insertCode(code);
	}
	else{
		InterCode code = malloc(sizeof(struct InterCode_));
		code->kind = ARG;
		code->u.one.op = op;
		insertCode(code);
	}
//	printf("Args finish!\n");
}
