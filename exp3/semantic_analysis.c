#include "tool.h"
#include "mediate_code.h"

//存储中间代码链表的头节点、尾节点
struct codenode *head;
struct codenode *tail;

bool has_error = false;
void code_init()
{
    head = (struct codenode *)malloc(sizeof(struct codenode));
    tail = head;
}

//进行语义分析之前的一些初始化操作
void semantic_analysis0(struct ASTNode *T)
{
    struct symbol_table *table = init_table(); //初始化符号表
    code_init();                               //TAC初始化
    semantic_analysis(T, table, 0);
    if (has_error == false)
    {
        display_code(head->next);
    }
}

//遍历AST, 创建符号表并进行静态语义分析
void semantic_analysis(struct ASTNode *T, struct symbol_table *table, int level)
{
    char var_type[20];
    char var_name[32];
    struct ASTNode *p;
    char fun_name[32];
    char return_type[20];
    int count;
    char param_name[32];
    char param_type[20];
    int pos;
    switch (T->kind)
    {
    case EXT_DEF_LIST:
        semantic_analysis(T->ptr[0], table, level);
        if (T->ptr[1] != NULL)
            semantic_analysis(T->ptr[1], table, level);
        break;
    case EXT_VAR_DEF:
        //变量类型
        strcpy(var_type, T->ptr[0]->type_id);
        //遍历变量列表
        p = T->ptr[1];
        while (p != NULL)
        {
            strcpy(var_name, p->ptr[0]->type_id);
            if (search(var_name, table) >= 0)
            {
                printf("\033[31m ERROR 03: row %d, 变量 %s 重复定义\n\033[0m", p->pos, var_name);
                has_error = true;
                exit(0);
            }
            else
            {
                insert_var_to_table(p->ptr[0], table, var_type, var_name, level);
            }
            memset(var_name, 0, sizeof(char) * 32);
            p = p->ptr[1];
        }
        memset(var_type, 0, sizeof(char) * 20);
        //遍历变量列表，生成中间代码
        semantic_analysis(T->ptr[1], table, level);
        break;

    case STMT_VAR_DEF:
        //变量类型
        strcpy(var_type, T->ptr[0]->type_id);
        //遍历变量列表
        p = T->ptr[1];
        while (p != NULL)
        {
            strcpy(var_name, p->ptr[0]->type_id);
            int var_pos = search(var_name, table);
            //函数体中的变量声明可以与其他函数中的局部变量、形式参数命名重合，不能与本函数中的命名重合
            if ((var_pos >= 0 && var_pos < cur_fun_pos && table->symbols[var_pos].flag != 'P') || var_pos > cur_fun_pos)
            {
                printf("\033[31m ERROR 03: row %d, 变量 %s 重复定义\n\033[0m", p->ptr[0]->pos, var_name);
                has_error = true;
                exit(0);
            }
            else
            {
                insert_var_to_table(p->ptr[0], table, var_type, var_name, level);
            }
            memset(var_name, 0, sizeof(char) * 32);
            p = p->ptr[1];
        }
        memset(var_type, 0, sizeof(char) * 20);
        // printf("\nVAR: \n");
        // display_table(table);
        semantic_analysis(T->ptr[1], table, level);
        break;

    case FUNC_CALL:
    {
        strcpy(fun_name, T->type_id);
        pos = search(fun_name, table);
        if (pos < 0)
        {
            printf("\033[31m ERROR 02: row %d, 调用未定义函数 %s \n\033[0m", T->ptr[0]->pos, fun_name);
            has_error = true;
            exit(0);
        }
        else
        {
            //符号表中对应函数参数数量
            int params_num = table->symbols[pos].paramnum;
            //调用无参函数
            if (params_num == 0)
            {
                if (T->num != 0)
                {
                    printf("\033[31m ERROR 07: row %d, 调用函数 %s 参数太多\n\033[0m", T->ptr[0]->pos, fun_name);
                    has_error = true;
                    exit(0);
                }
            }
            else if (params_num > 0)
            {
                //以无参形式调用有参函数
                if (T->num == 0)
                {
                    printf("\033[31m ERROR 08: row %d, 调用函数 %s 参数太少\n\033[0m", T->ptr[0]->pos, fun_name);
                    has_error = true;
                    exit(0);
                }
                else
                {
                    count = 0; //调用函数参数数量
                    p = T->ptr[0];
                    while (p != NULL)
                    {
                        p = p->ptr[1];
                        count++;
                    }
                    if (count > params_num)
                    {
                        printf("\033[31m ERROR 07: row %d, 调用函数 %s 参数太多\n\033[0m", T->ptr[0]->pos, fun_name);
                        has_error = true;
                        exit(0);
                    }
                    else if (count < params_num)
                    {
                        printf("\033[31m ERROR 08: row %d, 调用函数 %s 参数太少\n\033[0m", T->ptr[0]->pos, fun_name);
                        has_error = true;
                        exit(0);
                    }
                }
            }
        }
        //有参函数
        if(T->num == 1){ 
            //生成arg语句
            semantic_analysis(T->ptr[0], table, level);  
        } 
        int fun_type = getType(table->symbols[pos].type);//函数类型
        //call语句
        tail = insert_call(tail, fun_type, T->type_id);
        if(tail->opn1.type != TYPE_VOID){
            strcpy(T->alias, tail->result.id);
        } 
        break;
    }
    case ARRAY_CALL:
        pos = search(T->type_id, table);
        if (pos < 0)
        {
            printf("\033[31m ERROR 01: row %d, 变量%s未定义\n\033[0m", T->pos, T->type_id);
            has_error = true;
            exit(0);
        }
        else
        {
            //对非数组元素赋值
            if (table->symbols[pos].flag != 'A')
            {
                printf("\033[31m ERROR 10: row %d, 数组访问错误：对非数组变量 %s 采用下标变量的形式访问\n\033[0m", T->pos, T->type_id);
                has_error = true;
                exit(0);
            }
        }
        break;
    case ARGS:
        semantic_analysis(T->ptr[0], table, level);
        tail = insert(tail, ARG, T->ptr[0]->alias);
        if(T->num == 2){
            semantic_analysis(T->ptr[1], table, level);
        }
        break;
    //函数声明：不能重复声明，不支持重载
    case FUNC_DEC:
        //返回值类型
        strcpy(return_type, T->ptr[0]->type_id);
        //函数名
        strcpy(fun_name, T->ptr[1]->type_id);
        if (search(fun_name, table) >= 0)
        {
            printf("\033[31m ERROR 04: row %d, 函数 %s 重复声明\n\033[0m", T->ptr[0]->pos, fun_name);
            has_error = true;
            exit(0);
        }
        else
        {
            struct symbol s = create_symbol(fun_name, level, return_type, 0, newAlias(), 'f');
            insert_symbol(s, table);
            cur_fun_pos = table->index;
        }
        memset(return_type, 0, sizeof(char) * 20);
        p = T->ptr[1]->ptr[0];
        count = 0; //形参数量
        //遍历形参列表
        while (p != NULL)
        {
            strcpy(param_name, p->ptr[0]->type_id);
            strcpy(param_type, p->ptr[0]->ptr[0]->type_id);
            struct symbol s = create_symbol(param_name, level + 1, param_type, -1, newAlias(), 'P');
            insert_symbol(s, table);
            p = p->ptr[1];
            memset(param_name, 0, sizeof(char) * 32); //数组清零
            memset(param_type, 0, sizeof(char) * 32);
            count++;
        }
        table->symbols[table->index - count].paramnum = count; //更新函数参数数量
        break;

    //函数定义：参数要与声明的函数参数匹配
    case FUNC_DEF:
    {
        int res = fun_def(T, table, level);
        if(res == ERROR){
            has_error = true;
            exit(0);
        }
        cur_fun_pos = reverse_search(T->ptr[1]->type_id, table);
        pos = table->index;
        tail = insert(tail, FUNCTION, T->ptr[1]->type_id);
        //形参列表
        semantic_analysis(T->ptr[1], table, level + 1);
        //进入函数体
        semantic_analysis(T->ptr[2], table, level + 1); 
        table->index = pos; //删除函数中定义的局部变量，包括形参
        if (has_return == false && getType(T->ptr[0]->type_id) != TYPE_VOID)
        {
            printf("\033[31m ERROR 17: 函数%s 没有返回语句\n\033[0m", T->ptr[1]->type_id);
            has_error = true;
            exit(0);
        }
        else
        {
            has_return = false;
        }
        break;
    }
    case FUN_ID:
        //进入参数列表
        if (T->num == 1)
        {
            semantic_analysis(T->ptr[0], table, level);
        }
        break;
    case PARAMS_LIST:
        semantic_analysis(T->ptr[0], table, level);
        if (T->num == 2)
        {
            semantic_analysis(T->ptr[1], table, level);
        }
        break;
    case PARAMS:
        // pos = search(T->type_id, table);
        tail = insert(tail, PARAM, T->alias);
        // printf("PARAM %s\n", tail->result.id);
        break;
    case STM_LIST:
        semantic_analysis(T->ptr[0], table, level); //statement
        if (T->ptr[1] != NULL)
            semantic_analysis(T->ptr[1], table, level); //statement_list
        break;
    case STM:
        semantic_analysis(T->ptr[0], table, level);
        break;

    case RETURN:
    {
        if (in_if == false && in_while == false)
        {
            has_return = true;
        }
        int fun_return_type = getCurFunType(table); //函数返回值类型
        int return_type = -1;
        //函数返回值类型为void
        if (fun_return_type == TYPE_VOID)
        {
            printf("\033[31m ERROR 16: row %d, void类型函数不应该有返回值\n\033[0m", T->pos);
            has_error = true;
            exit(0);
        }
        else
        {
            //返回值中调用函数
            if (T->ptr[0]->kind == FUNC_CALL)
            {
                strcpy(fun_name, T->ptr[0]->type_id);
                pos = search(fun_name, table); //函数下标值
                if (pos < 0)
                {
                    printf("\033[31m ERROR 02: row %d, 函数 %s 不存在\n\033[0m", T->ptr[0]->pos, fun_name);
                    has_error = true;
                    exit(0);
                }
                else
                {
                    return_type = getType(table->symbols[pos].type); //获取调用函数的类型
                }
                memset(fun_name, 0, sizeof(char) * 32);
            }
            else
            {                                               //返回值是表达式
                return_type = getExpType(T->ptr[0], table); //获取表达式类型
            }

            if (return_type != fun_return_type)
            {
                printf("\033[31m ERROR 15: row %d, 函数返回值类型与函数定义的返回值类型不匹配\n\033[0m", T->ptr[0]->pos);
                has_error = true;
                exit(0);
            }
            //return结果
            semantic_analysis(T->ptr[0], table, level);
            tail = insert_return(tail, fun_return_type, T->ptr[0]->alias);
        }
        break;
    }
    case RETURN_NULL:
        if (getCurFunType(table) != TYPE_VOID)
        {
            printf("\033[31m ERROR 15: row %d, 函数返回值类型与函数定义的返回值类型不匹配\n\033[0m", T->pos);
            has_error = true;
            exit(0);
        }
        tail = insert_return(tail, TYPE_VOID, " ");
        if (in_if == false && in_while == false)
        {
            has_return = true;
        }
        break;
    case VAR_LIST:
        semantic_analysis(T->ptr[0], table, level);
        if(T->num == 2){
            semantic_analysis(T->ptr[1], table, level);
        }
        break;
    case VAR_ID:

        break;
    case VAR_INIT: 
        semantic_analysis(T->ptr[0], table, level);
        tail = insert_assignop(tail, T, table);
        break;
    case INIT_VALUE:
        break;
    case IF:
        pos = table->index;
        semantic_analysis(T->ptr[0], table, level + 1);
        in_if = true;
        tail = insert(tail, LABEL, T->ptr[0]->alias);
        semantic_analysis(T->ptr[1], table, level + 1);
        tail = insert(tail, LABEL, newLabel());
        in_if = false;
        // printf("\nIF:\n");
        // display_table(table);
        table->index = pos;
        break;
    case IF_ELSE:
    {
        pos = table->index;
        //条件
        semantic_analysis(T->ptr[0], table, level + 1);
        in_if = true;
        char elseLabel[32];//else语句标签
        strcpy(elseLabel, newLabel());
        tail = insert(tail, GOTO, elseLabel);//条件不成立跳转到else部分
        tail = insert(tail, LABEL, T->ptr[0]->alias);
        //if
        semantic_analysis(T->ptr[1], table, level + 1);
        table->index = pos;
        pos = table->index;
        tail = insert(tail, LABEL, elseLabel);
        //else
        semantic_analysis(T->ptr[2], table, level + 1);
        in_if = false;
        table->index = pos;
        break;
    }
    case WHILE:
        pos = table->index;
        semantic_analysis(T->ptr[0], table, level + 1); //条件
        char outWhileLabel[32];
        strcpy(outWhileLabel, newLabel());
        tail = insert(tail, GOTO, outWhileLabel);//条件不成立跳出while循环
        tail = insert(tail, LABEL, T->ptr[0]->alias);//while循环体的label
        jump = 1;
        in_while = true;
        semantic_analysis(T->ptr[1], table, level + 1); //while复合体
        tail = insert(tail, GOTO, T->ptr[0]->alias);//跳转到循环开始处
        tail = insert(tail, LABEL, outWhileLabel);
        in_while = false;
        jump = 0;
        // printf("\nWHILE:\n");
        // display_table(table);
        table->index = pos;
        break;
    case CONTINUE:
        if (jump == 0)
        {
            printf("\033[31m ERROR 19: row %d, continue语句必须在循环中使用\n\033[0m", T->pos);
            has_error = true;
            exit(0);
        }
        break;
    case BREAK:
        if (jump == 0)
        {
            printf("\033[31m ERROR 18: row %d, break语句必须在循环中使用\n\033[0m", T->pos);
            has_error = true;
            exit(0);
        }
        break;
    case ARRAY_ID:

        break;
    case ARRAY_INIT:

        break;

    case VALUE_LIST:
        break;
    case ID:
        pos = reverse_search(T->type_id, table);
        if (pos < 0 || (pos >= 0 && pos < cur_fun_pos && table->symbols[pos].flag == 'P'))
        {
            printf("\033[31m ERROR 01: row %d, 使用未定义变量%s\n\033[0m", T->pos, T->type_id);
            has_error = true;
            exit(0);
        }
        else if (table->symbols[pos].flag == 'A')
        {
            printf("\033[31m ERROR 21: row %d, 不能以普通变量形式访问数组\n\033[0m", T->pos);
            has_error = true;
            exit(0);
        }
        // T->table_index = pos;
        strcpy(T->alias, table->symbols[pos].alias);
        break;
    //类型，比如int，float
    case TYPE:
        break;
    case CONST_INT:
    case CONST_FLOAT:
    case CONST_CHAR:
        tail = insert_const(tail, T);
        strcpy(T->alias, tail->result.id);
        // printf("%s := #%d\n", tail->result.id, tail->opn1.const_int);
        break;
    //赋值语句
    case COMPASSIGN:
    case ASSIGNOP:
    {
        int kind = T->ptr[0]->kind;
        if (kind == PLUS || kind == MINUS || kind == STAR || kind == DIV ||
            kind == CONST_INT || kind == CONST_FLOAT || kind == CONST_CHAR || ID)
        {
            pos = search(T->type_id, table);
            if (pos < 0)
            {
                printf("\033[31m ERROR 13: row %d, 赋值语句错误：变量%s不存在\n\033[0m", T->pos, T->type_id);
                has_error = true;
                exit(0);
            }
            else
            {
                if (assignType(T, table) == -1)
                {
                    printf("\033[31m ERROR 14: row %d, 类型不匹配\n\033[0m", T->pos);
                    has_error = true;
                    exit(0);
                }
            }
        }
        //表达式
        semantic_analysis(T->ptr[0], table, level);
        tail = insert_assignop(tail, T, table);
        break;
    }
    //数组元素赋值
    case ARRAY_ELE_ASSIGN:
        pos = search(T->type_id, table);
        if (pos < 0)
        {
            printf("\033[31m ERROR 13: row %d, 赋值语句错误：变量%s不存在\n\033[0m", T->pos, T->type_id);
            has_error = true;
            exit(0);
        }
        else
        {
            //对非数组元素赋值
            if (table->symbols[pos].flag != 'A')
            {
                printf("\033[31m ERROR 12: row %d, 赋值语句错误：不能对非数组变量 %s 采用下标变量的方式赋值\n\033[0m", T->pos, T->type_id);
                has_error = true;
                exit(0);
            }
            //判断下标值是否为整型
            if (getExpType(T->ptr[0], table) != TYPE_INT)
            {
                printf("\033[31m ERROR 11: row %d, 数组 %s 下标值不是整型表达式\n\033[0m", T->pos, T->type_id);
                has_error = true;
                exit(0);
            }
            int array_type = getVarType(pos, table);
            int exp_type = getExpType(T->ptr[1], table);
            //判断赋值类型是否匹配
            if (exp_type == -1 || (array_type != exp_type))
            {
                printf("\033[31m ERROR 14: row %d, 类型不匹配\n\033[0m", T->pos);
                has_error = true;
                exit(0);
            }
        }
        semantic_analysis(T->ptr[0], table, level);
        semantic_analysis(T->ptr[1], table, level);
        break;
    //数组赋值
    case ARRAY_ASSIGN:
        pos = search(T->type_id, table);
        if (pos < 0)
        {
            printf("\033[31m ERROR 13: row %d, 赋值语句错误：变量%s不存在\n\033[0m", T->pos, T->type_id);
            has_error = true;
            exit(0);
        }
        break;
    //注释
    case COMMENT:
        break;
    case AND:
        break;
    case OR:
        semantic_analysis(T->ptr[0], table, level);
        semantic_analysis(T->ptr[1], table, level);
        break;
    case RELOP:
        semantic_analysis(T->ptr[0], table, level);
        semantic_analysis(T->ptr[1], table, level);
        //插入条件跳转语句
        tail = insert_relop(tail, T);
        //跳转目标label名用alias存储
        strcpy(T->alias, tail->result.id);
        break;
    case PLUS:
    case MINUS:
    case STAR:
    case DIV:
        if (getExpType(T, table) == -1)
        {
            printf("\033[31m ERROR 14: row %d, 类型不匹配\n\033[0m", T->pos);
            has_error = true;
            exit(0);
        }
        semantic_analysis(T->ptr[0], table, level);
        semantic_analysis(T->ptr[1], table, level);
        tail = insert_basic_operation(tail, T);
        // printf("%s := %s %s\n", tail->result.id, tail->opn1.id, tail->opn2.id);
        break;
    //一元运算符
    case UMINUS:
    case NOT:
        break;
    case DPLUS_R:
    case DMINUS_R:
        if (T->ptr[0]->kind != ID)
        {
            printf("\033[31m ERROR 22: row %d, 对非左值表达式进行自增、自减运算\n\033[0m", T->pos);
            has_error = true;
            exit(0);
        }
        else
        {
            semantic_analysis(T->ptr[0], table, level);
        }
        break;
    }
}
