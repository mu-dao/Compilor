#include"tree.h"

typedef struct Type_* Type;
typedef struct FieldList_* FieldList;
typedef struct SymbolTable_* SymbolTable;

struct Type_ {
	int kind;
	FieldList structure;
	union {
		struct { Type elem; int size; } array;
	};
};

struct FieldList_ {
	char name[16];
	char struct_name[16];
	Type type;
	FieldList tail;
};

struct SymbolTable_ {
	char name[16];
	Type type;
	int mark;
	char father[16];//struct name
	SymbolTable next;
	int dep;
};
SymbolTable symbol[1000];

enum { INT, FLOAT, ARRAY, STRUCT, FUNCTION, ASSIGNMENT };

extern void Program(tree* program);
