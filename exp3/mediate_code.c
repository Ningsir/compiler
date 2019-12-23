#include "mediate_code.h"
#include "stdio.h"
#include "tool.h"

extern int cur_fun_pos; //当前函数名在符号表中的位置
extern int jump;    //jump=1表示语义分析进入循环体中，此时可以使用break、continue语句
extern bool has_return;
extern bool in_if;
extern bool in_while;
extern int *offset;
extern int *zero;
extern FILE *objectFile;
static int count = -1;

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
//goto label操作
struct codenode *insert(struct codenode *tail, int op, char *name)
{
    struct codenode *code = (struct codenode *)malloc(sizeof(struct codenode));
    code->op = op;
    strcpy(code->result.id, name);
    tail->next = code;
    tail = code;
    return tail;
}
//arg param function操作
struct codenode *insert1(struct codenode *tail, int op, struct ASTNode *T)
{
    struct codenode *code = (struct codenode *)malloc(sizeof(struct codenode));
    code->op = op;
    strcpy(code->result.id, T->alias);
    code->result.offset = T->offset;
    tail->next = code;
    tail = code;
    return tail;
}
struct codenode *insert_return(struct codenode *tail, int return_type, struct ASTNode *T)
{
    struct codenode *code = (struct codenode *)malloc(sizeof(struct codenode));
    code->op = RETURN;
    code->result.type = return_type;
    if(return_type != TYPE_VOID){
        strcpy(code->result.id, T->alias);
        code->result.offset = T->offset;
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
    strcpy(code->result.id, T->alias);
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
    code->result.offset = T->offset;
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
    code->result.offset = table->symbols[pos].offset;
    code->opn1.kind = ID;
    strcpy(code->opn1.id, T->ptr[0]->alias);
    code->opn1.offset = T->ptr[0]->offset;
    tail->next = code;
    tail = code;
    return tail;
}

//插入加、减、乘、除操作
struct codenode *insert_basic_operation(struct codenode *tail, struct ASTNode *T)
{
    struct codenode *code = (struct codenode *)malloc(sizeof(struct codenode));
    code->op = T->kind;
    strcpy(code->result.id, T->alias);
    code->result.offset = T->offset;
    strcpy(code->opn1.id, T->ptr[0]->alias);
    code->opn1.offset = T->ptr[0]->offset;

    strcpy(code->opn2.id, T->ptr[1]->alias);
    code->opn2.offset = T->ptr[1]->offset;
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
        code->result.offset = *offset;
        *offset = *offset + 4;
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
    code->opn1.offset = T->ptr[0]->offset;
    strcpy(code->opn2.id, T->ptr[1]->alias);
    code->opn2.offset = T->ptr[1]->offset;
    strcpy(code->result.id, newLabel());
    tail->next = code;
    tail = code;
    return tail;
}
char *getArgReg(){
    char s[2];
    itoa(++count, s, 10);
    char *res = (char*)malloc(sizeof(char) * 5);
    strcpy(res, "a");
    strcat(res, s);
    return res;
}
void display_code(struct codenode *head, FILE *file)
{
    if (head == NULL)
    {
        return;
    }
    struct codenode *p = head;
    switch (p->op)
    {
    case PLUS:
        printf("%s(%d) := %s(%d) + %s(%d)\n", p->result.id, p->result.offset, p->opn1.id, p->opn1.offset, p->opn2.id, p->opn2.offset);
        fprintf(file, "%s := %s + %s\n", p->result.id, p->opn1.id, p->opn2.id);
        fprintf(objectFile, "lw $t1, %d($sp)\nlw $t2, %d($sp)\nadd $t3, $t1, $t2\nsw $t3, %d($sp)\n", p->opn1.offset, p->opn2.offset, p->result.offset);
        display_code(p->next, file);
        break;
    case MINUS:
        printf("%s(%d) := %s(%d) - %s(%d)\n", p->result.id, p->result.offset, p->opn1.id, p->opn1.offset, p->opn2.id, p->opn2.offset);
        fprintf(file, "%s := %s - %s\n", p->result.id, p->opn1.id, p->opn2.id);
        fprintf(objectFile, "lw $t1, %d($sp)\nlw $t2, %d($sp)\nsub $t3, $t1, $t2\nsw $t3, %d($sp)\n", p->opn1.offset, p->opn2.offset, p->result.offset);
        display_code(p->next, file);
        break;
    case STAR:
        printf("%s(%d) := %s(%d) * %s(%d)\n", p->result.id, p->result.offset, p->opn1.id, p->opn1.offset, p->opn2.id, p->opn2.offset);
        fprintf(file, "%s := %s * %s\n", p->result.id, p->opn1.id, p->opn2.id);
        fprintf(objectFile, "lw $t1, %d($sp)\nlw $t2, %d($sp)\nmul $t3, $t1, $t2\nsw $t3, %d($sp)\n", p->opn1.offset, p->opn2.offset, p->result.offset);
        display_code(p->next, file);
        break;
    case DIV:
        printf("%s(%d) := %s(%d) / %s(%d)\n", p->result.id, p->result.offset, p->opn1.id, p->opn1.offset, p->opn2.id, p->opn2.offset);
        fprintf(file, "%s := %s / %s\n", p->result.id, p->opn1.id, p->opn2.id);
        fprintf(objectFile, "lw $t1, %d($sp)\nlw $t2, %d($sp)\ndiv $t3, $t1, $t2\nsw $t3, %d($sp)\n", p->opn1.offset, p->opn2.offset, p->result.offset);
        display_code(p->next, file);
        break;
    case COMPASSIGN:
        break;
    case ASSIGNOP:
        if (p->opn1.kind == ID)
        {
            printf("%s(%d) := %s(%d)\n", p->result.id, p->result.offset, p->opn1.id, p->opn1.offset);
            fprintf(file, "%s := %s\n", p->result.id, p->opn1.id);
            fprintf(objectFile, "lw $t1, %d($sp)\nmove $t3, $t1\nsw $t3, %d($sp)\n", p->opn1.offset, p->result.offset);
        }
        if (p->opn1.kind == CONST_INT)
        {
            printf("%s(%d) := #%d\n", p->result.id, p->result.offset, p->opn1.const_int);
            fprintf(file, "%s := #%d\n", p->result.id, p->opn1.const_int);
            fprintf(objectFile, "li $t3, %d\nsw $t3, %d($sp)\n", p->opn1.const_int, p->result.offset);
        }
        if (p->opn1.kind == CONST_FLOAT)
        {
            printf("%s(%d) := #%f\n", p->result.id, p->result.offset, p->opn1.const_float);
            fprintf(file, "%s := #%f\n", p->result.id, p->opn1.const_float);
        }
        if (p->opn1.kind == CONST_CHAR)
        {
            printf("%s(%d) := #%c\n", p->result.id, p->result.offset, p->opn1.const_char);
            fprintf(file, "%s := #%c\n", p->result.id, p->opn1.const_char);
        }
        display_code(p->next, file);
        break;
    case FUNCTION:
        printf("FUNCTION %s(%d)\n", p->result.id, p->result.offset);
        fprintf(file, "FUNCTION %s\n", p->result.id);
        fprintf(objectFile, "%s:\n", p->result.id);
        count = -1;
        display_code(p->next, file);
        break;
    case LABEL:
        printf("LABEL %s\n", p->result.id);
        fprintf(file, "LABEL %s\n", p->result.id);
        fprintf(objectFile, "%s:\n", p->result.id);
        display_code(p->next, file);
        break;
    case GOTO:
        printf("GOTO %s\n", p->result.id);
        fprintf(file, "GOTO %s\n", p->result.id);
        fprintf(objectFile, "j %s\n", p->result.id);
        display_code(p->next, file);
        break;
    case ARG:
        printf("ARG %s(%d)\n", p->result.id, p->result.offset);
        fprintf(file, "ARG %s\n", p->result.id);
        fprintf(objectFile, "lw $%s, %d($sp)\n", getArgReg(), p->result.offset);
        display_code(p->next, file);
        break;
    case CALL:
        //需要保存返回地址
        if(p->opn1.type == TYPE_VOID){
            printf("CALL %s\n", p->opn1.id);
            fprintf(file, "CALL %s\n", p->opn1.id);
            fprintf(objectFile, "sw $ra, %d($sp)\njal %s\nlw $ra, %d($sp)\n", p->opn1.offset, p->opn1.id, p->opn1.offset);
        }else{
            printf("%s(%d) := CALL %s\n",p->result.id,p->result.offset, p->opn1.id);
            fprintf(file, "%s := CALL %s\n",p->result.id, p->opn1.id);
            fprintf(objectFile, "sw $ra, %d($sp)\njal %s\nsw $v0, %d($sp)\nlw $ra, %d($sp)\n", p->opn1.offset, p->opn1.id, p->result.offset, p->opn1.offset);
        }
        count = -1;
        display_code(p->next, file);
        break;
    case PARAM:
        printf("PARAM %s(%d)\n", p->result.id, p->result.offset);
        fprintf(file, "PARAM %s\n", p->result.id);
        fprintf(objectFile, "sw $%s, %d($sp)\n", getArgReg(),p->result.offset);
        if(p->next->op != PARAM){
            count = -1;
        }
        display_code(p->next, file);
        break;
    case EQU:
        printf("IF %s(%d) == %s(%d) GOTO %s\n", p->opn1.id, p->opn1.offset, p->opn2.id,p->opn2.offset, p->result.id);
        fprintf(file, "IF %s == %s GOTO %s\n", p->opn1.id, p->opn2.id, p->result.id);
        fprintf(objectFile, "lw $t1, %d($sp)\nlw $t2, %d($sp)\nbeq $t1,$t2, %s\n", p->opn1.offset, p->opn2.offset, p->result.id);
        display_code(p->next, file);
        break;
    case NE:
        printf("IF %s(%d) != %s(%d) GOTO %s\n", p->opn1.id, p->opn1.offset, p->opn2.id,p->opn2.offset, p->result.id);
        fprintf(file, "IF %s != %s GOTO %s\n", p->opn1.id, p->opn2.id, p->result.id);
        fprintf(objectFile, "lw $t1, %d($sp)\nlw $t2, %d($sp)\nbne $t1,$t2, %s\n", p->opn1.offset, p->opn2.offset, p->result.id);
        display_code(p->next, file);
        break;
    case GT:
        printf("IF %s(%d) > %s(%d) GOTO %s\n", p->opn1.id, p->opn1.offset, p->opn2.id,p->opn2.offset, p->result.id);
        fprintf(file, "IF %s > %s GOTO %s\n", p->opn1.id, p->opn2.id, p->result.id);
        fprintf(objectFile, "lw $t1, %d($sp)\nlw $t2, %d($sp)\nbgt $t1,$t2, %s\n", p->opn1.offset, p->opn2.offset, p->result.id);
        display_code(p->next, file);
        break;
    case NG:
        printf("IF %s(%d) <= %s(%d) GOTO %s\n", p->opn1.id, p->opn1.offset, p->opn2.id,p->opn2.offset, p->result.id);
        fprintf(file, "IF %s <= %s GOTO %s\n", p->opn1.id, p->opn2.id, p->result.id);
        fprintf(objectFile, "lw $t1, %d($sp)\nlw $t2, %d($sp)\nblt $t1,$t2, %s\n", p->opn1.offset, p->opn2.offset, p->result.id);
        display_code(p->next, file);
        break;
    case GTE:
        printf("IF %s(%d) >= %s(%d) GOTO %s\n", p->opn1.id, p->opn1.offset, p->opn2.id,p->opn2.offset, p->result.id);
        fprintf(file, "IF %s >= %s GOTO %s\n", p->opn1.id, p->opn2.id, p->result.id);
        fprintf(objectFile, "lw $t1, %d($sp)\nlw $t2, %d($sp)\nbge $t1,$t2, %s\n", p->opn1.offset, p->opn2.offset, p->result.id);
        display_code(p->next, file);
        break;
    case NGE:
        printf("IF %s(%d) < %s(%d) GOTO %s\n", p->opn1.id, p->opn1.offset, p->opn2.id,p->opn2.offset, p->result.id);
        fprintf(file, "IF %s < %s GOTO %s\n", p->opn1.id, p->opn2.id, p->result.id);
        fprintf(objectFile, "lw $t1, %d($sp)\nlw $t2, %d($sp)\nble $t1,$t2, %s\n", p->opn1.offset, p->opn2.offset, p->result.id);
        display_code(p->next, file);
        break;
    case RETURN:
        if(p->result.type != TYPE_VOID){
            printf("RETURN %s(%d)\n", p->result.id, p->result.offset);
            fprintf(file, "RETURN %s\n", p->result.id);
            fprintf(objectFile, "lw $v0, %d($sp)\njr $ra\n", p->result.offset);
        }else
        {
           printf("RETURN\n");
           fprintf(file, "RETURN\n");
        }
        display_code(p->next, file);
        break;
    default:
        break;
    }
}
