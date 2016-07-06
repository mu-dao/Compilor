#include"mips.h"

int offset = 8;
var varHead = NULL;
extern void printInterCode(InterCode c,FILE* fp);
extern void delete(char* fname);
extern void compute(Operand *result,Operand *op1,Operand *op2,InterCode c,FILE* fp);

void printMipsCode(char* fname){
	FILE *fp=fopen(fname,"w");
	if(fp==NULL){
		printf("open file error\n");
		return;
	}
	fputs(".data\n_prompt: .asciiz \"Enter an integer:\"\n",fp);
	fputs("_ret: .asciiz \"\\n\"\n.globl main\n.text\n",fp);
	
	fputs("read:\n",fp);
	fputs("  li $v0, 4\n",fp);
	fputs("  la $a0, _prompt\n",fp);
	fputs("  syscall\n",fp);
	fputs("  li $v0, 5\n",fp);
	fputs("  syscall\n",fp);
	fputs("  jr $ra\n",fp);
	
	fputs("\nwrite:\n",fp);
	fputs("  li $v0, 1\n",fp);
	fputs("  syscall\n",fp);
	fputs("  li $v0, 4\n",fp);
	fputs("  la $a0, _ret\n",fp);
	fputs("  syscall\n",fp);
	fputs("  move $v0, $0\n",fp);
	fputs("  jr $ra\n",fp);

	clean_label();
	InterCode c = head;
	while(c != NULL){
		printInterCode(c,fp);
		c = c->next;
	}
	fclose(fp);
	delete(fname);
}

void delete(char* fname){
	FILE* q = fopen(fname,"r");
	char file[64];
	strcpy(file,fname);
	for(int i = 0; i < strlen(file); i++){
		if(file[i+1] == '.')	file[i]++;
	}
	FILE* new = fopen(file,"w");
	if(new == NULL){
		printf("Error when opening file!\n");
		exit(0);
	}
	char* ps;
	bool flag = false;
	char str[64],newline[64],reg[2];
	strcpy(str,"");
	while(fgets(newline,64,q)){
		if(strstr(newline,"lw")){
			if(flag){
				ps = strstr(newline,"$");
				if(reg[0] == ps[1] && reg[1] == ps[2]){
					strcpy(str,newline);
				}else{
					if(strcmp(str,"") != 0)	fputs(str,new);
					strcpy(str,newline);
					ps = strstr(newline,"$");
					reg[0] = ps[1];
					reg[1] = ps[2];
				}
			}else{
				if(strcmp(str,"") != 0)	fputs(str,new);
				strcpy(str,newline);
				flag = true;
				ps = strstr(newline,"$");
				reg[0] = ps[1];
				reg[1] = ps[2];
			}
		}else{
			reg[0] = reg[1] = 'q';
			flag = false;
			if(strcmp(str,"") != 0)	fputs(str,new);
			strcpy(str,newline);
		}
	}
	if(strcmp(str,"") != 0) fputs(str,new);
	fclose(q);
	fclose(new);
	rename(file,fname);
}

int varNum(InterCode c){
	c = c->next;
	int count = 0;
	while(c != NULL && c->kind != FUNCTION_I){
		count++;
		c = c->next;
	}
	return count * 3;
}
void setVar(Operand op,char* reg,FILE* fp){
	if(op == NULL)	return;
	var list = varHead;
	char temp[32];
	if(op->kind == TEMP){
		while(list != NULL){
			if(list->type == 0 && list->var_no == op->u.var_no)	break;
			list = list->next;
		}
		if(list == NULL){
			offset += 4;
			list = malloc(sizeof(struct _var));
			list->type = 0;
			list->var_no = op->u.var_no;
			list->offset = offset;
			list->next = varHead;
			varHead = list;
		}
		sprintf(temp,"  sw %s, %d($fp)\n",reg , -list->offset);
		fputs(temp,fp);
	}
	else if(op->kind == VARIABLE){
		while(list != NULL){
			if(list->type == 1 && strcmp(list->name,op->u.name) == 0) break;
			list = list->next;
		}
		if(list == NULL){
			offset += 4;
			list = malloc(sizeof(struct _var));
			list->type = 1;
			strcpy(list->name,op->u.name);
			list->offset = offset;
			list->next = varHead;
			varHead = list;
		}
		sprintf(temp,"  sw %s, %d($fp)\n",reg , -list->offset);
		fputs(temp,fp);
	}
	else if(op->kind == CONSTANT_I){
		printf("Assignment to constant.\n");
		exit(0);
	}
	else if(op->kind == ADDRESS){
		while(list != NULL){
			if(list->type == 0 && list->var_no == op->u.var_no)	break;
			list = list->next;
		}
		if(list == NULL){
			printf("Addr not found.\n");
		}
		sprintf(temp,"  lw $t5, %d($fp)\n",-list->offset);
		fputs(temp,fp);
		sprintf(temp,"  sw %s, 0($t5)\n",reg);
		fputs(temp,fp);
	}
	else{
		printf("Unkown type %d.\n",op->kind);
		exit(0);
	}
}

void getVar(Operand op,char* reg,FILE* fp){
	if(op == NULL)	return;
	var list = varHead;
	char temp[32];
	if(op->kind == TEMP){
		while(list != NULL){
			if(list->type == 0 && list->var_no == op->u.var_no) break;
			list = list->next;
		}
		if(list == NULL){
			printf("TempVar not found.\n");
			exit(0);
		}
		sprintf(temp,"  lw %s,%d($fp)\n", reg, -list->offset);
		fputs(temp,fp);
	}
	else if(op->kind == VARIABLE){
		while(list != NULL){
			if(list->type == 1 && strcmp(list->name,op->u.name) == 0) break;
			list = list->next;
		}
		if(list == NULL){
			printf("Variable %s not found.\n",op->u.name);
			exit(0);
		}
		sprintf(temp,"  lw %s, %d($fp)\n", reg, -list->offset);
		fputs(temp,fp);
	}
	else if(op->kind == CONSTANT_I){
		sprintf(temp,"  li %s, %d\n", reg, op->u.int_value);
		fputs(temp,fp);
	}
	else if(op->kind == ADDRESS){
		while(list != NULL){
			if(list->type == 0 && list->var_no == op->u.var_no)	break;
			list = list->next;
		}
		if(list == NULL){
			printf("Addr not found.\n");
		}
		sprintf(temp,"  lw $t5, %d($fp)\n",-list->offset);
		fputs(temp,fp);
		sprintf(temp,"  lw %s, 0($t5)\n", reg);
		fputs(temp,fp);
	}
	else{
		printf("Unkown type %d.\n",op->kind);
		exit(0);
	}
}

void printInterCode(InterCode c,FILE* fp){
	int kind = c->kind;
	int memSize = 0;
	char temp[32];
	int i;
	Operand left,right,result,op1,op2,label;
	InterCode ic;

	switch(kind){
		case LABEL_I:
			printOp(c->u.one.op,fp);
			fputs(":\n",fp);
			break;
		case FUNCTION_I:
			fputs("\n",fp);
			printOp(c->u.one.op,fp);
			fputs(":\n",fp);
			offset = 8;
			
			fputs("  sw $ra, -4($sp)\n",fp);
			fputs("  sw $fp, -8($sp)\n",fp);
			fputs("  move $fp, $sp\n",fp);

			memSize = varNum(head) * 4 + offset;
			sprintf(temp,"  subu $sp,$sp,%d\n",memSize);
			fputs(temp,fp);
			ic = c->next;
			for(i = 0;i < 4 && ic->kind == PARAM;i++){
				if(i == 0)	setVar(ic->u.one.op,"$a0",fp);
				if(i == 1)	setVar(ic->u.one.op,"$a1",fp);
				if(i == 2)	setVar(ic->u.one.op,"$a2",fp);
				if(i == 3)	setVar(ic->u.one.op,"$a3",fp);
				ic = ic->next;
			}
			break;
		case ASSIGN:
			left=c->u.assign.left;
			right=c->u.assign.right;
			getVar(right,"$t0",fp);
			setVar(left,"$t0",fp);
			break;
		case ADD:
			compute(&result,&op1,&op2,c,fp);
			fputs("  add $t0, $t0, $t1\n",fp);
			setVar(result,"$t0",fp);
			break;
		case SUB:
			compute(&result,&op1,&op2,c,fp);
			fputs("  sub $t0, $t0, $t1\n",fp);
			setVar(result,"$t0",fp);
			break;
		case MUL:
			compute(&result,&op1,&op2,c,fp);
			fputs("  mul $t0, $t0, $t1\n",fp);
			setVar(result,"$t0",fp);
			break;
		case DIV:
			compute(&result,&op1,&op2,c,fp);
			fputs("  div $t0, $t0, $t1\n",fp);
			setVar(result,"$t0",fp);
			break;
		case READ:
			fputs("  jal read\n",fp);
			setVar(c->u.one.op,"$v0",fp);
			break;
		case WRITE:
			getVar(c->u.one.op,"$a0",fp);
			fputs("  jal write\n",fp);
			break;
		case GOTO:
			fputs("  j ",fp);
			printOp(c->u.one.op,fp);
			fputs("\n",fp);
			break;
		case IF:
			op1 = c->u.four.op1;
			op2 = c->u.four.op2;
			label = c->u.four.label;
			char relop[16];
			strcpy(relop,c->u.four.relop);
			getVar(op1,"$t0",fp);
			getVar(op2,"$t1",fp);
			if(strcmp(relop,"==") == 0)	fputs("  beq ",fp);
			if(strcmp(relop,"!=") == 0)	fputs("  bne ",fp);
			if(strcmp(relop,">") == 0)	fputs("  bgt ",fp);
			if(strcmp(relop,"<") == 0)	fputs("  blt ",fp);
			if(strcmp(relop,">=") == 0)	fputs("  bge ",fp);
			if(strcmp(relop,"<=") == 0)	fputs("  ble ",fp);
			fputs("$t0, $t1, ",fp);
			printOp(label,fp);
			fputs("\n",fp);
			break;
		case RETURN:
			getVar(c->u.one.op,"$v0",fp);
			fputs("  lw $sp, 0($fp)\n",fp);
			fputs("  move $sp, $fp\n",fp);
			fputs("  lw $ra, -4($fp)\n",fp);
			fputs("  lw $fp, -8($fp)\n",fp);
			fputs("  jr $ra\n",fp);
			break;
		case CALL:
			ic = c->prev;
			for(i = 0;i < 4 && ic->kind == ARG;i++){
				if(i == 0)	getVar(ic->u.one.op,"$a0",fp);
				if(i == 1)	getVar(ic->u.one.op,"$a1",fp);
				if(i == 2)	getVar(ic->u.one.op,"$a2",fp);
				if(i == 3)	getVar(ic->u.one.op,"$a3",fp);
				ic = ic->prev;
			}
			if(ic->kind == ARG){
				printf("Can't handle over 4 arguments.\n");
				exit(0);
			}
			fputs("  addi $sp, $sp, -4\n",fp);
			fputs("  sw $ra, 0($sp)\n",fp);
			fputs("  jal ",fp);
			printOp(c->u.assign.right,fp);
			fputs("\n",fp);
			fputs("  lw $ra, 0($sp)\n",fp);
			fputs("  addi $sp, $sp, 4\n",fp);
			if(c->u.assign.left != NULL)	setVar(c->u.assign.left,"$v0",fp);
			break;
		case ADDR:
			ic = c->prev;
			if(ic->kind != DEC){
				printf("DEC missing.\n");
				exit(0);
			}
			fputs("  subu $sp, $sp, ",fp);
			sprintf(temp,"%d\n",ic->u.dec.size);
			fputs(temp,fp);
			offset += ic->u.dec.size;
			setVar(c->u.assign.left,"$sp",fp);
			break;
		default:break;
	}
}
void compute(Operand *result, Operand *op1, Operand *op2, InterCode c, FILE* fp){
	*result = c->u.binop.result;
	*op1 = c->u.binop.op1;
	*op2 = c->u.binop.op2;
	getVar(*op1,"$t0",fp);
	getVar(*op2,"$t1",fp);
}
