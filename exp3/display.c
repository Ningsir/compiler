#include "stdio.h"
#include "Node.h"
#include "stdlib.h"
#include "exp.tab.h"
/*
num: 不定参数的长度
kind：节点种类
pos: 位置
*/
struct ASTNode *mknode(int num, int kind, int pos, ...)
{
    struct ASTNode *T = (struct ASTNode *)malloc(sizeof(struct ASTNode));
    int i = 0;
    T->kind = kind;
	T->num = num;
    T->pos = pos;
    va_list pArgs = NULL;
    va_start(pArgs, pos);
    for (i = 0; i < num; i++)
        T->ptr[i] = va_arg(pArgs, struct ASTNode *);
    while (i < 4)
        T->ptr[i++] = NULL;
    va_end(pArgs);
    return T;
}

void display(struct ASTNode *T, int indent)
{
		switch (T->kind)
		{
		case EXT_DEF_LIST:
			display(T->ptr[0], indent);
			if (T->ptr[1] != NULL)
				display(T->ptr[1], indent);
			break;
		case EXT_VAR_DEF:
			printf("%*c外部变量声明： \n", indent, ' ');
			display(T->ptr[0], indent + 3);//类型
			display(T->ptr[1], indent + 6);//变量列表
			break;
		case FUNC_CALL:
			printf("%*c函数调用： \n", indent, ' ');
			printf("%*c函数名称： %s\n", indent + 3, ' ', T->type_id);
			//参数列表
			if (T->num == 0){//参数为空	
				printf("%*c参数： 无\n", indent + 3, ' ');	
			}else{//参数不为空
				printf("%*c参数： \n", indent + 3, ' ');	
				display(T->ptr[0], indent + 6);
			}
			break;
		case ARGS:
			if (T->num == 1){	
				display(T->ptr[0], indent);
			}else{
				display(T->ptr[0], indent);
				display(T->ptr[1], indent);
			}
			break;
		case FUNC_DEC:
			printf("%*c外部函数声明： \n", indent, ' ');
			display(T->ptr[0], indent + 3);//返回值类型
			display(T->ptr[1], indent + 3);//函数名称及参数列表
			break;
		case FUNC_DEF:
			printf("%*c函数定义： \n", indent, ' ');
			display(T->ptr[0], indent + 3);//返回值类型
			display(T->ptr[1], indent + 3);//函数名称及参数列表
			printf("%*c函数体： \n", indent + 3, ' ');
			display(T->ptr[2], indent + 6);//函数体
			break;
		case FUN_ID:
			printf("%*c函数名称： %s\n", indent, ' ', T->type_id);
			if (T->num != 0){
				printf("%*c参数列表： \n", indent, ' ');
				display(T->ptr[0], indent + 3);//参数列表
			}else{
				printf("%*c参数列表： 无参数\n", indent, ' ');
			}
			 
			break;
		case PARAMS_LIST:		
			if (T->num == 1){
				display(T->ptr[0], indent);
			}else{
				display(T->ptr[0], indent);
				display(T->ptr[1], indent);
			}
			break;
		case PARAMS:
			display(T->ptr[0], indent);//参数类型
			printf("%*c参数名称： %s\n", indent, ' ', T->type_id);//参数名称
			break;
		case STM_LIST:
		    if(T != NULL){
				display(T->ptr[0], indent);
				if(T->ptr[1] != NULL){
				display(T->ptr[1], indent);
				}
			}
			break;
		case STM:
			display(T->ptr[0], indent);
			break;
		case STMT_VAR_DEF:
			printf("%*c函数体变量声明： \n", indent, ' ');
			display(T->ptr[0], indent + 3);//类型
			display(T->ptr[1], indent + 6);//变量列表
			break;
		case RETURN:
			printf("%*cRETURN:  \n", indent, ' ');
			display(T->ptr[0], indent + 3);
			break;
		
		case VAR_LIST:
			if (T->num == 1){
				display(T->ptr[0], indent);
			}else{
				display(T->ptr[0], indent);
				display(T->ptr[1], indent);
			}
			break;
		case VAR_ID:
			printf("%*c变量名： %s\n", indent, ' ', T->type_id);
			break;
		case VAR_INIT:
			printf("%*c变量名： %s\n", indent, ' ', T->type_id);
			printf("%*cEQU：\n", indent + 3, ' ');
			display(T->ptr[0], indent + 3);//表达式
			break;
		case INIT_VALUE:
			break; 
		case IF:
			printf("%*cIF：  \n", indent, ' ');
			printf("%*c条件：  \n", indent + 3, ' ');
			display(T->ptr[0], indent + 6);//条件
			printf("%*cIF语句：  \n", indent + 3, ' ');
			display(T->ptr[1], indent + 6);
			break;
		case IF_ELSE:
			printf("%*cIF-ELSE： \n", indent, ' ');
			printf("%*c条件：  \n", indent + 3, ' ');
			display(T->ptr[0], indent + 6);//条件
			printf("%*cIF语句：  \n", indent + 3, ' ');
			display(T->ptr[1], indent + 6);//显示IF语句
			printf("%*cELSE语句：  \n", indent + 3, ' ');
			display(T->ptr[2], indent + 6);//显示ELSE语句
			break;
		case WHILE:
			printf("%*cWHILE： \n", indent, ' ');
			printf("%*c条件：  \n", indent + 3, ' ');
			display(T->ptr[0], indent + 6);//条件
			printf("%*cWHILE语句：  \n", indent + 3, ' ');
			display(T->ptr[1], indent + 6);//显示WHILE语句
			break;
		case CONTINUE:
			printf("%*cCONTINUE \n", indent, ' ');
			break;
		case BREAK:
			printf("%*cBREAK \n", indent, ' ');
			 break;
		case ARRAY_ID:
			printf("%*c数组名： %s\n", indent, ' ', T->type_id);
			printf("%*c数组长度： %d\n", indent, ' ', T->int_value);
			break;
		case ARRAY_INIT:
			printf("%*c数组名： %s\n", indent, ' ', T->type_id);
			printf("%*c数组长度： %d\n", indent, ' ', T->int_value);
			printf("%*c数组初始化： \n", indent, ' ');
			display(T->ptr[0], indent + 3);//初始化
			break;
		case ARRAY_ELE_ASSIGN:
			printf("%*c数组元素赋值： \n", indent, ' ');
			//printf("%*c%s[%d]:\n", indent + 3, ' ', T->type_id, T->int_value);
			display(T->ptr[0], indent + 3);
			display(T->ptr[1], indent + 3);
			break;
		case ARRAY_ASSIGN:
			printf("%*c数组赋值： \n", indent, ' ');
			printf("%*c数组初始化：\n", indent, ' ');
			display(T->ptr[0], indent + 3);
			break;
		case VALUE_LIST:
			if (T->num == 1){
				display(T->ptr[0], indent);
			}else{
				display(T->ptr[0], indent);
				display(T->ptr[1], indent);
			}
			break;
		case ID:
			printf("%*cID： %s\n", indent, ' ', T->type_id);
			break;
		//类型，比如int，float
		case TYPE:
			printf("%*cTYPE： %s\n", indent, ' ', T->type_id);
			break;
		case CONST_INT:
			printf("%*cINT： %d\n", indent, ' ', T->int_value);
			break;
		case CONST_FLOAT:
			printf("%*cFLOAT： %f\n", indent, ' ', T->float_value);
			break;
		case CONST_CHAR:
			printf("%*cCHAR： %c\n", indent, ' ', T->char_value);
			break;
		//赋值语句
		case ASSIGNOP:
			printf("%*c赋值语句\n", indent, ' ');
			printf("%*c变量名：%s\n", indent + 3, ' ', T->type_id);
			printf("%*cEQU\n", indent + 3, ' ');
			display(T->ptr[0], indent + 3);
			break;
		//复合赋值语句
		case COMPASSIGN:
			printf("%*c复合赋值语句:\n", indent, ' ');
			printf("%*c变量名：%s\n", indent + 3, ' ', T->type_id);
			printf("%*c%s\n", indent + 3, ' ', T->compassign_id);
			display(T->ptr[0], indent + 3);//exp
		 	break;
		case COMPASSIGN_ID:
			printf("%*c%s\n", indent, ' ', T->type_id);
			break;
		case COMMENT:
			printf("%*cCOMMENT：%s\n", indent, ' ', T->comment);
			break;
		//二目运算符
		case AND:
		case OR:
		case RELOP:
		case PLUS:
		case MINUS:
		case STAR:
		case DIV:
			printf("%*c%s\n", indent, ' ', T->type_id);
			display(T->ptr[0], indent + 3);
			display(T->ptr[1], indent + 3);
			break;
		//一元运算符
		case UMINUS:
		case NOT:
			printf("%*c%s\n", indent, ' ', T->type_id);
			display(T->ptr[0], indent + 3);
		case DPLUS_L:
			printf("%*c%s (left)\n", indent, ' ', T->type_id);
			display(T->ptr[0], indent + 3);
			break;
		case DMINUS_L:
			printf("%*c%s (left)\n", indent, ' ', T->type_id);
			display(T->ptr[0], indent + 3);
			break;
		case DPLUS_R:
			printf("%*c%s (right)\n", indent, ' ', T->type_id);
			display(T->ptr[0], indent + 3);
			break;
		case DMINUS_R:
			printf("%*c%s (right)\n", indent, ' ', T->type_id);
			display(T->ptr[0], indent + 3);
			break;
		}
}
