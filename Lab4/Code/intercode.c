#include"intercode.h"
extern void printOp(Operand op,FILE* fp);
InterCode head = NULL;
InterCode tail = NULL;

int varCount = 1;
int labCount = 1;

void insertCode(InterCode c){
	c->prev = NULL;
	c->next = NULL;
	if(head == NULL){
		head = c;
		tail = c;
	}
	else{
		c->prev = tail;
		tail->next = c;
		tail = c;
	}
}
void clean_label(){
	InterCode c = head;
	InterCode cn = c->next;
	while(cn != NULL){
		if(c->kind == LABEL_I && cn->kind == LABEL_I){
			Operand cop1 = c->u.one.op;
			Operand cop2 = cn->u.one.op;
			InterCode d = head;
			while(d != NULL){
				if(d->kind == GOTO){
					Operand dop = d->u.one.op;
					if(cop2->u.var_no == dop->u.var_no){
						dop->u.var_no = cop1->u.var_no;
						c->next = cn->next;
					}
				}
				d = d->next;
			}
		}
		c = c->next;
		cn = cn->next;
	}
}

void printCode(char* fname){
	FILE *fp = fopen(fname,"w");
	if(fp == NULL){
		printf("open file error\n");
		return;
	}
	clean_label();
	InterCode c = head;
	while(c != NULL){
		if(c->kind == LABEL_I){
			fputs("LABEL ",fp);
			printOp(c->u.one.op,fp);
			fputs(": ",fp);
		}
		else if(c->kind == FUNCTION_I){
			fputs("FUNCTION ",fp);
			printOp(c->u.one.op,fp);
			fputs(": ",fp);
		}
		else if(c->kind == ASSIGN){
			if(c->u.assign.left != NULL){
				printOp(c->u.assign.left,fp);
				fputs(":= ",fp);
				printOp(c->u.assign.right,fp);
			}
		}
		else if(c->kind == ADD){
			printOp(c->u.binop.result,fp);
			fputs(":= ",fp);
			printOp(c->u.binop.op1,fp);
			fputs("+ ",fp);
			printOp(c->u.binop.op2,fp);
		}
		else if(c->kind == SUB){
			printOp(c->u.binop.result,fp);
			fputs(":= ",fp);
			printOp(c->u.binop.op1,fp);
			fputs("- ",fp);
			printOp(c->u.binop.op2,fp);
		}
		else if(c->kind == MUL){
			printOp(c->u.binop.result,fp);
			fputs(":= ",fp);
			printOp(c->u.binop.op1,fp);
			fputs("* ",fp);
			printOp(c->u.binop.op2,fp);
		}
		else if(c->kind == DIV){
			printOp(c->u.binop.result,fp);
			fputs(":= ",fp);
			printOp(c->u.binop.op1,fp);
			fputs("/ ",fp);
			printOp(c->u.binop.op2,fp);
		}
		else if(c->kind == READ){
			fputs("READ ",fp);
			printOp(c->u.one.op,fp);
		}
		else if(c->kind == WRITE){
			fputs("WRITE ",fp);
			printOp(c->u.one.op,fp);
		}
		else if(c->kind == GOTO){
			fputs("GOTO ",fp);
			printOp(c->u.one.op,fp);
		}
		else if(c->kind == IF){
			fputs("IF ",fp);
			printOp(c->u.four.op1,fp);
			fputs(c->u.four.relop,fp);
			fputs(" ",fp);
			printOp(c->u.four.op2,fp);
			fputs("GOTO ",fp);
			printOp(c->u.four.label,fp);
		}
		else if(c->kind == RETURN){
			fputs("RETURN ",fp);
			printOp(c->u.one.op,fp);
		}
		else if(c->kind == DEC){
			if(c->next->kind == ADDR){
				InterCode d = c->next;
				if(d->next->kind == PARAM){
					c = d->next;
					continue;
				}
			}
			fputs("DEC ",fp);
			printOp(c->u.dec.op,fp);
			char size[32];
			sprintf(size,"%d",c->u.dec.size);
			fputs(size,fp);
		}
		else if(c->kind == ARG){
			fputs("ARG ",fp);
			printOp(c->u.one.op,fp);
		}
		else if(c->kind == CALL){
			printOp(c->u.assign.left,fp);
			fputs(":= CALL ",fp);
			printOp(c->u.assign.right,fp);
		}
		else if(c->kind == PARAM){
			fputs("PARAM ",fp);
			printOp(c->u.one.op,fp);
		}
		else if(c->kind == ADDR){
			printOp(c->u.assign.left,fp);
			fputs(":= &",fp);
			printOp(c->u.assign.right,fp);
		}
		fputs("\n",fp);
		c = c->next;
	}
	fclose(fp);
}

void printOp(Operand op,FILE* fp){
	char var[32] = "";
	if(op->kind == TEMP){
		fputs("t",fp);
		sprintf(var,"%d",op->u.var_no);
		fputs(var,fp);
	}
	else if(op->kind == VARIABLE){
		fputs(op->u.name,fp);
	}
	else if(op->kind == CONSTANT_I){
		fputs("#",fp);
		sprintf(var,"%d",op->u.int_value);
		fputs(var,fp);
	}
	else if(op->kind == CONSTANT_F){
		fputs("#",fp);
		sprintf(var,"%f",op->u.float_value);
		fputs(var,fp);
	}
	else if(op->kind == LABEL){
		fputs("label",fp);
		sprintf(var,"%d",op->u.var_no);
		fputs(var,fp);
	}
	else if(op->kind == FUNCTION_O){
		if(strcmp(op->u.name,"main") != 0){
			sprintf(op->u.name,"%s_1",op->u.name);
		}
		fputs(op->u.name,fp);
	}
	else if(op->kind == ADDRESS){
		fputs("*t",fp);
		sprintf(var,"%d",op->u.var_no);
		fputs(var,fp);
	}
	fputs(" ",fp);
}
