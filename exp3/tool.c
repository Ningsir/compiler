#include "tool.h"

extern int cur_fun_pos; //当前函数名在符号表中的位置
extern int jump;    //jump=1表示语义分析进入循环体中，此时可以使用break、continue语句
extern bool has_return;
extern bool in_if;
extern bool in_while;
extern int *offset;
extern int *zero;

struct symbol_table *init_table()
{
    struct symbol_table *table = (struct symbol_table *)malloc(sizeof(struct symbol_table));
    table->index = -1;
    return table;
}
struct symbol create_symbol(char name[32], int level, char type[20], int paramnum, char alias[20], char flag, int *offset)
{
    
    struct symbol s;
    strcpy(s.name, name);
    s.level = level;
    strcpy(s.type, type);
    s.paramnum = paramnum;
    strcpy(s.alias, alias);
    s.flag = flag;
    s.offset = *offset;
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

char *newLabel()
{
    static int no = 0;
    char s[10];
    itoa(no++, s, 10);
    char res[10] = "label";
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

void insert_var_to_table(struct ASTNode *T, struct symbol_table *table, char var_type[20], char var_name[32], int level){
    struct symbol s;
    //普通变量
    if (T->kind == VAR_ID)
    {
        
        s = create_symbol(var_name, level, var_type, -1, newAlias(), 'V', offset);
        *offset = *offset + 4;
    }
    //声明变量并进行初始化
    else if(T->kind == VAR_INIT){
        int t1 = getType(var_type);
        int t2 = getExpType(T->ptr[0], table);
        if (t2 == -1 || t1 != t2){
            printf("\033[31m ERROR 14: row %d, 类型不匹配\n\033[0m", T->pos);
        }
        else{   
            s = create_symbol(var_name, level, var_type, -1, newAlias(), 'V', offset);
            *offset = *offset + 4;
        }
    }
    else
    { //数组
        s = create_symbol(var_name, level, var_type, T->int_value, newAlias(), 'A', offset);
        *offset = *offset + 4 * T->int_value;
    }
    insert_symbol(s, table);
}
void display_table(struct symbol_table *table)
{
    printf("\033[34m%-20s%-20s%-20s%-20s%-20s%-20s%-20s\n\033[0m", "名称", "层号", "类型", "参数数量", "别名", "标记", "offset");
    for (int i = 0; i <= table->index; i++)
    {
        printf("%-20s%-15d%-20s%-15d%-20s%-20c%-20d\n",
               table->symbols[i].name, table->symbols[i].level, table->symbols[i].type, table->symbols[i].paramnum, table->symbols[i].alias, table->symbols[i].flag, table->symbols[i].offset);
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
int reverse_search(char name[32], struct symbol_table *table){
    for (int i = table->index; i >= 0; i--)
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
                semantic_analysis(T->ptr[0], table, 0);
            }
            return getType(table->symbols[pos].type);
        }
    }
    if(T->kind == FUNC_CALL){
        int pos = search(T->type_id, table);
        int kind = getType(table->symbols[pos].type);
        // if (kind == TYPE_VOID){
        //     return -1;
        // }
        return kind;
    }
    //整型
    if (T->kind == CONST_INT)
    {
        return TYPE_INT;
    }
    if (T->kind == CONST_FLOAT)
    {
        return TYPE_FLOAT;
    }
    if (T->kind == CONST_CHAR)
    {
        return TYPE_CHAR;
    }
    int left = getExpType(T->ptr[0], table);
    int right = getExpType(T->ptr[1], table);
    if(left == TYPE_VOID || right == TYPE_VOID){
        printf("\033[31m ERROR 23: row %d, void类型函数不能进行基本运算\n\033[0m", T->pos);
        return -1;
    }
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

int fun_def(struct ASTNode *T, struct symbol_table *table, int level){
        char return_type[32], fun_name[32], param_name[32], param_type[20];
        //返回值类型
        strcpy(return_type, T->ptr[0]->type_id);
        //函数名
        strcpy(fun_name, T->ptr[1]->type_id);
        strcpy(T->ptr[1]->alias, fun_name);
        int fun_pos = search(fun_name, table);
        //函数没有声明，加入符号表
        if (fun_pos < 0)
        { 
            struct symbol s = create_symbol(fun_name, 0, return_type, 0, newAlias(), 'F', offset);
            *offset = *offset + 4;
            insert_symbol(s, table);
            T->ptr[1]->offset = s.offset;
            cur_fun_pos = table->index;
        }else{
            //函数重定义
            if(table->symbols[fun_pos].flag == 'F'){
                printf("\033[31m ERROR 04: row:%d, 函数 %s 重复定义\n\033[0m", T->ptr[0]->pos, fun_name);
                return ERROR;
            }else{
                table->symbols[fun_pos].flag = 'F';
                T->ptr[1]->offset = table->symbols[fun_pos].offset;
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
                int temp = search(param_name, table);
                if (temp >= 0 && table->symbols[temp].flag != 'P')
                {
                    printf("\033[31m ERROR 03: row:%d, 变量 %s 重复定义\n\033[0m", p->ptr[0]->pos, param_name);
                    return ERROR;
                }
                struct symbol s = create_symbol(param_name, level + 1, param_type, -1, newAlias(), 'P', offset);
                *offset = *offset + 4;
                insert_symbol(s, table);
                strcpy(p->ptr[0]->alias, s.alias);//保留别名
                p->ptr[0]->offset = s.offset;
            }
            else
            { //函数已经声明，判断参数是否匹配
                if (strcmp(table->symbols[fun_pos + count + 1].type, param_type) != 0)
                {
                    printf("\033[31m ERROR 20: row %d, 函数声明与函数定义的参数类型不匹配\n\033[0m", p->ptr[0]->pos);
                    return ERROR;
                }
            }
            p = p->ptr[1];
            memset(param_name, 0, sizeof(char) * 32); //数组清零
            memset(param_type, 0, sizeof(char) * 32);
            count++;
        }
        table->symbols[table->index - count].paramnum = count; //更新函数参数数量
        return SUCCESS;
}