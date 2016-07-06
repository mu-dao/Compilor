typedef struct Operand_* Operand;
typedef struct InterCode_* InterCode;

struct Operand_{
	enum{ TEMP, VARIABLE, CONSTANT_I, CONSTANT_F, ADDRESS, LABEL, FUNCTION_O } kind;
	union{
		int var_no;
		int int_value;
		float float_value;
		char name[32];
	} u;
};

struct InterCode_{
	enum{ LABEL_I, FUNCTION_I, ASSIGN, ADD, SUB, MUL, DIV, READ, WRITE, GOTO, IF, RETURN, DEC, ARG, CALL, PARAM, ADDR } kind;
	union{
		struct{
			Operand op;
		} one;
		struct{
			Operand left,right;
		} assign;
		struct{
			Operand result,op1,op2;
		} binop;
		struct{
			Operand op1,op2,label;
			char relop[32];
		} four;
		struct{
			Operand op;
			int size;
		} dec;
	} u;
	InterCode prev, next;
};

extern InterCode head;
extern int varCount;
extern int labCount;
extern void printCode(char* fname);
extern void insertCode(InterCode c);
