#include "mediate_code.h"
#include "stdio.h"
#include "tool.h"

struct codenode *insert_label(struct codenode *tail)
{
    static int count = 0;
    struct codenode *code = (struct codenode *)malloc(sizeof(struct codenode));
    code->op = LABEL;
    char label_name[32] = "label";
    char s[10];
    itoa(count++, s, 10);
    strcat(label_name, s);
    strcpy(code->result.id, label_name);
    tail->next = code;
    //*tail = *code;错误代码，会修改尾节点地址中的内容，从而导致头结点指向错误
    tail = code;
    return tail;
}
//goto arg param function操作
struct codenode *insert(struct codenode *tail, int op, char *name)
{
    struct codenode *code = (struct codenode *)malloc(sizeof(struct codenode));
    code->op = op;
    strcpy(code->result.id, name);
    tail->next = code;
    tail = code;
    return tail;
}

struct codenode *insert_return(struct codenode *tail, int return_type, char *name)
{
    struct codenode *code = (struct codenode *)malloc(sizeof(struct codenode));
    code->op = RETURN;
    code->result.type = return_type;
    if(return_type != TYPE_VOID){
        strcpy(code->result.id, name);
    } 
    tail->next = code;
    tail = code;
    return tail;
} 

//插入常数，格式为：temp := #const
struct codenode *insert_const(struct codenode *tail, struct ASTNode *T)
{
    struct codenode *code = (struct codenode *)malloc(sizeof(struct codenode));
    code->op = ASSIGNOP;
    code->result.kind = ID;
    strcpy(code->result.id, newTemp());
    code->opn1.kind = T->kind;
    if (T->kind == CONST_INT)
    {
        code->opn1.const_int = T->int_value;
    }
    else if (T->kind = CONST_FLOAT)
    {
        code->opn1.const_float = T->float_value;
    }
    else
    {
        code->opn1.const_char = T->char_value;
    }
    tail->next = code;
    tail = code;
    return tail;
}

struct codenode *insert_assignop(struct codenode *tail, struct ASTNode *T, struct symbol_table *table)
{
    struct codenode *code = (struct codenode *)malloc(sizeof(struct codenode));
    code->op = ASSIGNOP;
    code->result.kind = ID;
    int pos = reverse_search(T->type_id, table);
    strcpy(code->result.id, table->symbols[pos].alias);
    code->opn1.kind = ID;
    strcpy(code->opn1.id, T->ptr[0]->alias);
    tail->next = code;
    tail = code;
    return tail;
}

//插入加、减、乘、除操作
struct codenode *insert_basic_operation(struct codenode *tail, struct ASTNode *T)
{
    struct codenode *code = (struct codenode *)malloc(sizeof(struct codenode));
    code->op = T->kind;
    strcpy(code->result.id, newTemp());
    strcpy(T->alias, code->result.id);
    strcpy(code->opn1.id, T->ptr[0]->alias);
    strcpy(code->opn2.id, T->ptr[1]->alias);
    tail->next = code;
    tail = code;
    return tail;
}
struct codenode *insert_call(struct codenode *tail, int fun_type, char *fun_name){
    struct codenode *code = (struct codenode *)malloc(sizeof(struct codenode));
    code->op = CALL;
    code->opn1.type = fun_type;
    strcpy(code->opn1.id, fun_name);
    if(fun_type != TYPE_VOID){
        strcpy(code->result.id, newTemp());
    }
    tail->next = code;
    tail = code;
    return tail;
}
// 条件跳转
struct codenode *insert_relop(struct codenode *tail, struct ASTNode *T)
{
    struct codenode *code = (struct codenode *)malloc(sizeof(struct codenode));
    char op[32];
    strcpy(op, T->type_id);
    if (strcmp(op, "==") == 0)
    {
        code->op = EQU;
    }
    if (strcmp(op, "!=") == 0)
    {
        code->op = NE;
    }
    if (strcmp(op, ">=") == 0)
    {
        code->op = GTE;
    }
    if (strcmp(op, "<=") == 0)
    {
        code->op = NG;
    }
    if (strcmp(op, ">") == 0)
    {
        code->op = GT;
    }
    if (strcmp(op, "<") == 0)
    {
        code->op = NGE;
    }
    strcpy(code->opn1.id, T->ptr[0]->alias);
    strcpy(code->opn2.id, T->ptr[1]->alias);
    strcpy(code->result.id, newLabel());
    tail->next = code;
    tail = code;
    return tail;
}
void display_code(struct codenode *head)
{
    if (head == NULL)
    {
        return;
    }
    struct codenode *p = head;
    switch (p->op)
    {
    case PLUS:
        printf("%s := %s + %s\n", p->result.id, p->opn1.id, p->opn2.id);
        display_code(p->next);
        break;
    case MINUS:
        printf("%s := %s - %s\n", p->result.id, p->opn1.id, p->opn2.id);
        display_code(p->next);
        break;
    case STAR:
        printf("%s := %s * %s\n", p->result.id, p->opn1.id, p->opn2.id);
        display_code(p->next);
        break;
    case DIV:
        printf("%s := %s / %s\n", p->result.id, p->opn1.id, p->opn2.id);
        display_code(p->next);
        break;
    case COMPASSIGN:
        break;
    case ASSIGNOP:
        if (p->opn1.kind == ID)
        {
            printf("%s := %s\n", p->result.id, p->opn1.id);
        }
        if (p->opn1.kind == CONST_INT)
        {
            printf("%s := #%d\n", p->result.id, p->opn1.const_int);
        }
        if (p->opn1.kind == CONST_FLOAT)
        {
            printf("%s := #%f\n", p->result.id, p->opn1.const_float);
        }
        if (p->opn1.kind == CONST_CHAR)
        {
            printf("%s := #%c\n", p->result.id, p->opn1.const_char);
        }
        display_code(p->next);
        break;
    case FUNCTION:
        printf("FUNCTION %s\n", p->result.id);
        display_code(p->next);
        break;
    case LABEL:
        printf("LABEL %s\n", p->result.id);
        display_code(p->next);
        break;
    case GOTO:
        printf("GOTO %s\n", p->result.id);
        display_code(p->next);
        break;
    case ARG:
        printf("ARG %s\n", p->result.id);
        display_code(p->next);
        break;
    case CALL:
        if(p->opn1.type == TYPE_VOID){
            printf("CALL %s\n", p->opn1.id);
        }else{
            printf("%s := CALL %s\n",p->result.id, p->opn1.id);
        }
        display_code(p->next);
        break;
    case PARAM:
        printf("PARAM %s\n", p->result.id);
        display_code(p->next);
        break;
    case EQU:
        printf("IF %s == %s GOTO %s\n", p->opn1.id, p->opn2.id, p->result.id);
        display_code(p->next);
        break;
    case NE:
        printf("IF %s != %s GOTO %s\n", p->opn1.id, p->opn2.id, p->result.id);
        display_code(p->next);
        break;
    case GT:
        printf("IF %s > %s GOTO %s\n", p->opn1.id, p->opn2.id, p->result.id);
        display_code(p->next);
        break;
    case NG:
        printf("IF %s <= %s GOTO %s\n", p->opn1.id, p->opn2.id, p->result.id);
        display_code(p->next);
        break;
    case GTE:
        printf("IF %s >= %s GOTO %s\n", p->opn1.id, p->opn2.id, p->result.id);
        display_code(p->next);
        break;
    case NGE:
        printf("IF %s < %s GOTO %s\n", p->opn1.id, p->opn2.id, p->result.id);
        display_code(p->next);
        break;
    case RETURN:
        if(p->result.type != TYPE_VOID){
            printf("RETURN %s\n", p->result.id);
        }else
        {
           printf("RETURN\n");
        }
        display_code(p->next);
        break;
    default:
        break;
    }
}
