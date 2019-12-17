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

int cur_fun_pos; //当前函数名在符号表中的位置
int jump = 0;    //jump=1表示语义分析进入循环体中，此时可以使用break、continue语句
bool has_return = false;
bool in_if = false;
bool in_while = false;

struct symbol_table * init_table();
struct symbol create_symbol(char name[32], int level, char type[20], int paramnum, char alias[20], char flag);
int assignType(struct ASTNode *T, struct symbol_table *table);
void insert_symbol(struct symbol s, struct symbol_table *table);
void insert_var(struct ASTNode *T, struct symbol_table *table, char var_type[20], char var_name[32], int level);
void display_table(struct symbol_table *table);
int search(char name[32], struct symbol_table *table);
int getType(char type[20]);
int getExpType(struct ASTNode *T, struct symbol_table *table);
int getVarType(int index, struct symbol_table *table);
int getCurFunType(struct symbol_table *table);

struct symbol_table *init_table()
{
    struct symbol_table *table = (struct symbol_table *)malloc(sizeof(struct symbol_table));
    table->index = -1;
    return table;
}
struct symbol create_symbol(char name[32], int level, char type[20], int paramnum, char alias[20], char flag)
{
    struct symbol s;
    strcpy(s.name, name);
    s.level = level;
    strcpy(s.type, type);
    s.paramnum = paramnum;
    strcpy(s.alias, alias);
    s.flag = flag;
    return s;
}
char *newAlias()
{
    static int no = 0;
    char s[10];
    itoa(no++, s, 10);
    char res[10] = "v";
    return strcat(res, s);
}

char *newTemp()
{
    static int no = 0;
    char s[10];
    itoa(no++, s, 10);
    char res[10] = "temp";
    return strcat(res, s);
}
//判断赋值语句左右类型是否匹配
int assignType(struct ASTNode *T, struct symbol_table *table){
    int pos = search(T->type_id, table);
    int t1 = getVarType(pos, table);
    int t2 = getExpType(T->ptr[0], table);
    if (t2 == -1 || (t1 != t2))
    {
        return -1;
    }
    return 0;
}
void insert_symbol(struct symbol s, struct symbol_table *table)
{
    table->symbols[++table->index] = s;
}

void insert_var(struct ASTNode *T, struct symbol_table *table, char var_type[20], char var_name[32], int level){
    struct symbol s;
    //普通变量
    if (T->kind == VAR_ID)
    {
        s = create_symbol(var_name, level, var_type, -1, newAlias(), 'V');
    }
    //声明变量并进行初始化
    else if(T->kind == VAR_INIT){
        int t1 = getType(var_type);
        int t2 = getExpType(T->ptr[0], table);
        if (t2 == -1 || t1 != t2){
            printf("\033[31m ERROR 14: row %d, 类型不匹配\n\033[0m", T->pos);
        }
        else{
            s = create_symbol(var_name, level, var_type, -1, newAlias(), 'V');
        }
    }
    else
    { //数组
        s = create_symbol(var_name, level, var_type, T->int_value, newAlias(), 'A');
    }
    insert_symbol(s, table);
}
void display_table(struct symbol_table *table)
{
    printf("\033[34m%-20s%-20s%-20s%-20s%-20s%-20s\n\033[0m", "名称", "层号", "类型", "参数数量", "别名", "标记");
    for (int i = 0; i <= table->index; i++)
    {
        printf("%-20s%-15d%-20s%-15d%-20s%-20c\n",
               table->symbols[i].name, table->symbols[i].level, table->symbols[i].type, table->symbols[i].paramnum, table->symbols[i].alias, table->symbols[i].flag);
    }
}

int search(char name[32], struct symbol_table *table)
{
    for (int i = 0; i <= table->index; i++)
    {
        if (strcmp(name, table->symbols[i].name) == 0)
        {
            return i;
        }
    }
    return -1;
}

int getType(char type[20])
{
    if (strcmp("int", type) == 0)
    {
        return TYPE_INT;
    }
    else if (strcmp("float", type) == 0)
    {
        return TYPE_FLOAT;
    }
    else if (strcmp("char", type) == 0)
    {
        return TYPE_CHAR;
    }
    else if (strcmp("void", type) == 0)
    {
        return TYPE_VOID;
    }
    return -1;
}

//获取二元表达式类型，-1表示表达式类型不匹配
int getExpType(struct ASTNode *T, struct symbol_table *table)
{
    //T为标识符
    if (T->kind == ID || T->kind == ARRAY_CALL)
    {
        int pos = search(T->type_id, table);
        if (pos < 0)
        {
            printf("\033[31m ERROR 01: row %d, 变量 %s 未定义\n\033[0m", T->pos, T->type_id);
            return -1;
        }
        else
        {
            if(T->kind == ARRAY_CALL){
                semantic_analysis0(T->ptr[0], table, 0);
            }
            return getType(table->symbols[pos].type);
        }
    }
    //整型
    if (T->kind == _INT)
    {
        return TYPE_INT;
    }
    if (T->kind == _FLOAT)
    {
        return TYPE_FLOAT;
    }
    if (T->kind == _CHAR)
    {
        return TYPE_CHAR;
    }
    int left = getExpType(T->ptr[0], table);
    int right = getExpType(T->ptr[1], table);
    return left == right ? left : -1;
}

//根据符号表获取变量类型，index为符号下标值
int getVarType(int index, struct symbol_table *table)
{
    return getType(table->symbols[index].type);
}

//获取当前函数返回值类型
int getCurFunType(struct symbol_table *table)
{
    for (int i = table->index; i >= 0; i--)
    {
        if (table->symbols[i].flag == 'F')
        {
            return getType(table->symbols[i].type);
        }
    }
    return -1;
}

void fun_def(struct ASTNode *T, struct symbol_table *table, int level){
        char return_type[32], fun_name[32], param_name[32], param_type[20];
        //返回值类型
        strcpy(return_type, T->ptr[0]->type_id);
        //函数名
        strcpy(fun_name, T->ptr[1]->type_id);
        int fun_pos = search(fun_name, table);
        //函数没有声明，加入符号表
        if (fun_pos < 0)
        {
            struct symbol s = create_symbol(fun_name, 0, return_type, 0, newAlias(), 'F');
            insert_symbol(s, table);
            cur_fun_pos = table->index;
        }else{
            //函数重定义
            if(table->symbols[fun_pos].flag == 'F'){
                printf("\033[31m ERROR 04: row:%d, 函数 %s 重复定义\n\033[0m", T->ptr[0]->pos, fun_name);
            }else{
                table->symbols[fun_pos].flag = 'F';
            }
        }
        memset(return_type, 0, sizeof(char) * 20);
        struct ASTNode *p = T->ptr[1]->ptr[0];
        int count = 0; //形参数量
        //遍历形参列表
        while (p != NULL)
        {
            strcpy(param_name, p->ptr[0]->type_id);
            strcpy(param_type, p->ptr[0]->ptr[0]->type_id);
            //函数没有声明将形参加入符号表
            if (fun_pos < 0)
            {
                if (search(param_name, table) >= 0)
                {
                    printf("\033[31m ERROR 03: row:%d, 变量 %s 重复定义\n\033[0m", p->ptr[0]->pos, param_name);
                }
                struct symbol s = create_symbol(param_name, level + 1, param_type, -1, newAlias(), 'P');
                insert_symbol(s, table);
            }
            else
            { //函数已经声明，判断参数是否匹配
                if (strcmp(table->symbols[fun_pos + count + 1].type, param_type) != 0)
                {
                    printf("\033[31m ERROR 20: row %d, 函数声明与函数定义的参数类型不匹配\n\033[0m", p->ptr[0]->pos);
                }
            }
            p = p->ptr[1];
            memset(param_name, 0, sizeof(char) * 32); //数组清零
            memset(param_type, 0, sizeof(char) * 32);
            count++;
        }
        table->symbols[table->index - count].paramnum = count; //更新函数参数数量
        int pos = table->index;
        //进入stm_list
        semantic_analysis0(T->ptr[2], table, level + 1);
        table->index = pos; //删除函数中定义的局部变量，包括形参
        if(has_return == false){
            printf("\033[31m ERROR 17: 函数%s 没有返回语句\n\033[0m",  T->ptr[1]->type_id);
        }else{
            has_return = false;
        }
}