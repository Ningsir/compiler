#ifndef __TOOL
#define __TOOL

#include "Node.h"
#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "exp.tab.h"
#include "stdbool.h"

//表达式类型：int、float、char
#define TYPE_VOID 0
#define TYPE_INT 1
#define TYPE_FLOAT 2
#define TYPE_CHAR 3
#define ERROR -1
#define SUCCESS 0

static int cur_fun_pos; //当前函数名在符号表中的位置
static int jump = 0;    //jump=1表示语义分析进入循环体中，此时可以使用break、continue语句
static bool has_return = false;
static bool in_if = false;
static bool in_while = false;

struct symbol_table * init_table();
char *newAlias();
char *newTemp();
char *newLabel();
struct symbol create_symbol(char name[32], int level, char type[20], int paramnum, char alias[20], char flag);
int assignType(struct ASTNode *T, struct symbol_table *table);
void insert_symbol(struct symbol s, struct symbol_table *table);
void insert_var_to_table(struct ASTNode *T, struct symbol_table *table, char var_type[20], char var_name[32], int level);
void display_table(struct symbol_table *table);
int search(char name[32], struct symbol_table *table);
int reverse_search(char name[32], struct symbol_table *table);
int getType(char type[20]);
int getExpType(struct ASTNode *T, struct symbol_table *table);
int getVarType(int index, struct symbol_table *table);
int getCurFunType(struct symbol_table *table);
int fun_def(struct ASTNode *T, struct symbol_table *table, int level);
#endif