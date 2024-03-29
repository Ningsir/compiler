#ifndef __NODE__
#define __NODE__
#include "stdarg.h"
#define MAXLENGTH 200
typedef struct ASTNode {
	int kind;//节点类型
	int type;
	char type_id[32];//由标识符生成的AST结点
	char compassign_id[32];//复合赋值符号
	int pos;//该节点的位置，用行数表示
	int num;//子节点数量
    char alias[32];
    int offset;
    int table_index;
	struct ASTNode *ptr[4];
    struct codenode *code;
	union {		
		//节点表示的值
		int int_value;
		float float_value;
		char char_value;
		char comment[200];
	};
}* AST;

struct symbol
{                 
    char name[32];  //变量或函数名
    int level;      //层号,0表示全局变量
    char type[20];       //变量类型或函数返回值类型
    int paramnum;   //对函数适用，记录形式参数个数
    char alias[10]; //别名，为解决嵌套层次使用
    char flag;      //符号标记，函数：'F'    变量：'V'      参数：'P'    临时变量：'T', 数组：'A'
    int offset;    //外部变量和局部变量在其静态数据区或活动记录中的偏移量，
                    //或记录函数活动记录大小，目标代码生成时使用
    //函数入口等实验可能会用到的属性...
};
//符号表
struct symbol_table
{
    struct symbol symbols[MAXLENGTH];
    int index;
} symbol_table;

struct opn
{
    int kind; //标识联合成员的属性
    int type; //标识操作数的数据类型
    union {
        int const_int;     //整常数值，立即数
        float const_float; //浮点常数值，立即数
        char const_char;   //字符常数值，立即数
        char id[32];       //变量或临时变量的别名或标号字符串
    };
    int offset;
};

struct codenode
{ //三地址 TAC 代码结点
    int op;
    struct opn opn1, opn2, result;
    struct codenode *next;
};

struct ASTNode *mknode(int num, int kind, int pos, ...);
void semantic_analysis0(struct ASTNode *T);
void semantic_analysis(struct ASTNode *T, struct symbol_table *table, int level);


#endif

