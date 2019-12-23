#ifndef __MEDIATE_CODE
#define __MEDIATE_CODE
#include "node.h"
#include "string.h"
#include "stdlib.h"
#include "exp.tab.h"
#include "stdio.h"


#define FUNCTION 100
#define LABEL 101
#define GOTO 102
#define ARG 103 //实参
#define CALL 104
#define PARAM 105   //形参
#define EQU 106 // ==
#define NE 107  // !=
#define GT 108  // >
#define NG 109  // <=
#define GTE 110 // >=
#define NGE 111 // <

struct codenode *insert_label(struct codenode *tail);
struct codenode *insert(struct codenode *tail, int op, char *name);
struct codenode *insert_goto(struct codenode *tail, char label_name[32]);
void display_code(struct codenode *head, FILE *file);
struct codenode *insert_params(struct codenode *tail, char *params_name);
struct codenode *insert_const(struct codenode *tail, struct ASTNode *T);
struct codenode *insert_assignop(struct codenode *tail, struct ASTNode *T, struct symbol_table *table);
struct codenode *insert_basic_operation(struct codenode *tail, struct ASTNode *T);
struct codenode *insert_relop(struct codenode *tail, struct ASTNode *T);
struct codenode *insert_call(struct codenode *tail, int fun_type, char *fun_name);
struct codenode *insert_return(struct codenode *tail, int return_type, struct ASTNode *T);
struct codenode *insert1(struct codenode *tail, int op, struct ASTNode *T);
#endif