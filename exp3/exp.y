%locations
%{
#include "stdio.h"
#include "math.h"
#include "string.h"
#include "Node.h"
extern int yylineno;
extern char *yytext;
extern FILE *yyin;
void display(struct ASTNode *,int);
%}
%union {
	int int_value;
	float float_value;
    char char_value;
	char type_id[32];//标识符
    char compassign_id[32];//复合赋值符号
    char comment[200];
	struct ASTNode *ptr;
} 

%type  <ptr> exp args ext_var_list var specifier external_definition
%type  <ptr> function_declaration compound_statement params_list params_dec statement_list statement
%type  <ptr> ext_def_list var_list value_list constant jump assign
%token <int_value> _INT 
%token <char_value> _CHAR 
%token <type_id> ID	TYPE RELOP 
%token <float_value> _FLOAT
%token <comment> COMMENT
%token <compassign_id> COMPASSIGN
%token _LP RP  LC RC LB RB PLUS MINUS STAR DIV ASSIGNOP AND OR NOT DPLUS_L DPLUS_R DMINUS_L DMINUS_R
%token COMMA SEMI  ARRAY_CALL
%token RETURN IF IF_ELSE WHILE RETURN_NULL
%token FUNC_CALL ARGS EXT_VAR_DEF FUNC_DEC FUNC_DEF VAR_LIST VAR_ID VAR_INIT INIT_VALUE FUN_ID PARAMS_LIST PARAMS
%token EXT_DEF_LIST STM STM_LIST STMT_VAR_DEF ARRAY_ID ARRAY_INIT ARRAY_ELE_ASSIGN ARRAY_ASSIGN VALUE_LIST CONTINUE BREAK COMPASSIGN_ID
//规定运算符是左结合还是右结合，否则会出现冲突
%left PLUS MINUS STAR DIV AND OR ASSIGNOP RELOP COMPASSIGN
%right UMINUS NOT DPLUS DMINUS
%start program
%nonassoc LOWER_THEN_ELSE
%nonassoc ELSE

%%
program
    : ext_def_list    { /*printf("ROOT : \n");display($1,3);*/struct symbol_table *table=  init_table();semantic_analysis0($1, table, 0);}
    ;

ext_def_list: {$$=NULL;}
    |  external_definition ext_def_list {$$=mknode(2,EXT_DEF_LIST,yylineno,$1,$2);}
    ;  

exp	 
    : exp AND exp   {$$=mknode(2,AND,yylineno,$1,$3);strcpy($$->type_id,"AND");}
    | exp OR exp    {$$=mknode(2,OR,yylineno,$1,$3);strcpy($$->type_id,"OR");}
    | exp RELOP exp {$$=mknode(2,RELOP,yylineno,$1,$3);strcpy($$->type_id,$2);}
    | exp PLUS exp  {$$=mknode(2,PLUS,yylineno,$1,$3);strcpy($$->type_id,"PLUS");}
    | exp MINUS exp {$$=mknode(2,MINUS,yylineno,$1,$3);strcpy($$->type_id,"MINUS");}
    | exp STAR exp  {$$=mknode(2,STAR,yylineno,$1,$3);strcpy($$->type_id,"STAR");}
    | exp DIV exp   {$$=mknode(2,DIV,yylineno,$1,$3);strcpy($$->type_id,"DIV");}
    | MINUS exp %prec UMINUS   {$$=mknode(1,UMINUS,yylineno,$2);strcpy($$->type_id,"UMINUS");}
    | _LP exp RP     {$$=$2;}
    | NOT exp       {$$=mknode(1,NOT,yylineno,$2);strcpy($$->type_id,"NOT");}
    | DPLUS  exp      {$$=mknode(1,DPLUS_L,yylineno,$2);strcpy($$->type_id,"DPLUS");}
    | exp DPLUS      {$$=mknode(1,DPLUS_R,yylineno,$1);strcpy($$->type_id,"DPLUS");}
    | DMINUS  exp      {$$=mknode(1,DMINUS_L,yylineno,$2);strcpy($$->type_id,"DMINUS");}
    | exp DMINUS      {$$=mknode(1,DMINUS_R,yylineno,$1);strcpy($$->type_id,"DMINUS");}
    | ID _LP args RP {$$=mknode(1,FUNC_CALL,yylineno,$3);strcpy($$->type_id,$1);}
    | ID _LP RP      {$$=mknode(0,FUNC_CALL,yylineno);strcpy($$->type_id,$1);}
    | ID LB exp RB  {$$=mknode(1,ARRAY_CALL,yylineno, $3);strcpy($$->type_id,$1);}
    | ID            {$$=mknode(0,ID,yylineno);strcpy($$->type_id,$1);}
    | constant      {$$=$1;}
    ;


args
    : exp COMMA args    {$$=mknode(2,ARGS,yylineno,$1,$3);}
    | exp               {$$=mknode(1,ARGS,yylineno,$1);}
    ;
   /*外部变量、函数的声明*/       
external_definition
	: specifier ext_var_list SEMI   {$$=mknode(2,EXT_VAR_DEF,yylineno,$1,$2);}
    | specifier function_declaration SEMI  {$$=mknode(2,FUNC_DEC,yylineno,$1,$2);}
    | specifier function_declaration compound_statement    {$$=mknode(3,FUNC_DEF,yylineno,$1,$2,$3);}
    | COMMENT       {$$=mknode(0,COMMENT,yylineno);strcpy($$->comment, $1);}
    | error   {$$=NULL;}
    ;

ext_var_list
    : var   {$$=mknode(1,VAR_LIST,yylineno,$1);}
    | var COMMA ext_var_list {$$=mknode(2,VAR_LIST,yylineno,$1, $3);}
    ;

var
    : ID LB _INT RB {$$=mknode(0,ARRAY_ID,yylineno);strcpy($$->type_id, $1); $$->int_value=$3;}
    | ID ASSIGNOP exp  {$$=mknode(1,VAR_INIT,yylineno, $3);strcpy($$->type_id, $1);}
    | ID   {$$=mknode(0,VAR_ID,yylineno);strcpy($$->type_id, $1);}
    | ID LB _INT RB ASSIGNOP LB value_list RB {$$=mknode(1,ARRAY_INIT,yylineno, $7);strcpy($$->type_id, $1); $$->int_value=$3;}
    ;
    
    /*
array_assign
    : ID LB exp RB ASSIGNOP exp {$$=mknode(2,ARRAY_ELE_ASSIGN,yylineno, $3, $6);strcpy($$->type_id, $1);}
    | ID ASSIGNOP LB value_list RB {$$=mknode(1,ARRAY_ASSIGN,yylineno, $4);strcpy($$->type_id, $1);}
    ;*/

    /*包括普通变量赋值、复合赋值、数组赋值*/
assign
    : ID ASSIGNOP exp {$$=mknode(1,ASSIGNOP,yylineno,$3);strcpy($$->type_id, $1);}
    | ID COMPASSIGN exp {$$=mknode(1,COMPASSIGN,yylineno,$3);strcpy($$->type_id, $1);strcpy($$->compassign_id, $2);}
    | ID LB exp RB ASSIGNOP exp {$$=mknode(2,ARRAY_ELE_ASSIGN,yylineno, $3, $6);strcpy($$->type_id, $1);}
    | ID ASSIGNOP LB value_list RB {$$=mknode(1,ARRAY_ASSIGN,yylineno, $4);strcpy($$->type_id, $1);}
    ;

value_list
    : constant {$$=$1;}
    | constant COMMA value_list {$$=mknode(2,VALUE_LIST,yylineno, $1, $3);}
    ;

constant
    : _INT{$$=mknode(0,_INT,yylineno);$$->int_value=$1;$$->type=_INT;}
    | _FLOAT {$$=mknode(0,_FLOAT,yylineno);$$->float_value=$1;$$->type=_FLOAT;}
    | _CHAR  {$$=mknode(0,_CHAR,yylineno);$$->char_value=$1;$$->type=_CHAR;}
    ;

specifier
    : TYPE {$$=mknode(0,TYPE,yylineno);strcpy($$->type_id, $1);}
    ;

function_declaration
	: ID _LP params_list RP {$$=mknode(1,FUN_ID,yylineno, $3);strcpy($$->type_id, $1);}
    | ID _LP RP {$$=mknode(0,FUN_ID,yylineno);strcpy($$->type_id, $1);}
	;

params_list
    : params_dec {$$=mknode(1,PARAMS_LIST,yylineno, $1);}
    | params_dec COMMA params_list  {$$=mknode(2,PARAMS_LIST,yylineno, $1, $3);}
    ;

params_dec
    : specifier ID {$$=mknode(1,PARAMS,yylineno, $1);strcpy($$->type_id, $2);}
    ;

compound_statement
	:  LC statement_list RC {$$=$2;}
	;

statement_list
	: {$$=NULL;}
	|  statement statement_list {$$=mknode(2,STM_LIST,yylineno, $1, $2);}
	;

statement
	: compound_statement   {$$=$1;}
	| exp SEMI  {$$=mknode(1,STM,yylineno, $1);}
    | assign SEMI {$$=$1;}
    | COMMENT       {$$=mknode(0,COMMENT,yylineno);strcpy($$->comment, $1);}
    | RETURN exp SEMI{$$=mknode(1,RETURN,yylineno, $2);}
    | RETURN SEMI{$$=mknode(0,RETURN_NULL,yylineno);}
    | specifier var_list SEMI   {$$=mknode(2,STMT_VAR_DEF,yylineno,$1,$2);}
    | IF _LP exp RP statement %prec LOWER_THEN_ELSE   {$$=mknode(2,IF,yylineno,$3,$5);}
    | IF _LP exp RP statement ELSE statement   {$$=mknode(3,IF_ELSE,yylineno,$3,$5,$7);}
    | WHILE _LP exp RP statement {$$=mknode(2,WHILE,yylineno,$3,$5);}
    | jump SEMI{$$=$1;}
	;

jump
    : CONTINUE {$$=mknode(0,CONTINUE,yylineno);}
    | BREAK {$$=mknode(0,BREAK,yylineno);}
    ;

var_list
    : var   {$$=mknode(1,VAR_LIST,yylineno,$1);}
    | var COMMA var_list {$$=mknode(2,VAR_LIST,yylineno,$1, $3);}
    ;  
%%

int main(int argc, char *argv[]){
	yyin=fopen(argv[1],"r");
	if (!yyin) return 1;
	    yyparse();
	return 0;
	}

#include<stdarg.h>
void yyerror(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    fprintf(stderr, "Grammar Error at Line %d Column %d: ", yylloc.first_line,yylloc.first_column);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, ".\n");
}


