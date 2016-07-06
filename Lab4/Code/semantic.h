#include"tree.h"
#include"intercode.h"
typedef struct Type_* Type;
typedef struct FieldList_* FieldList;
typedef struct SymbolTable_* SymbolTable;

struct Type_ {
	int kind;
	char father[32];
	FieldList structure;
	union {
		//int int_value;
		//float float_value;
		struct { Type elem; int size; } array;
	};
};

struct FieldList_ {
	char name[32];
	char struct_name[32];
	Type type;
	FieldList tail;
};

struct SymbolTable_ {
	char name[32];
	Type type;
	int mark;
	char father[32];//struct name
	SymbolTable next;
	int dep;
};
SymbolTable symbol[1000];

enum { INT, FLOAT, ARRAY, STRUCT, FUNCTION, ASSIGNMENT };

extern void Program(tree* program);
