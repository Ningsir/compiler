%option yylineno
%{
#include "exp.tab.h"
#include "string.h"
#include "Node.h"
int yycolumn = 0;
#define YY_USER_ACTION    	yylloc.first_line=yylloc.last_line=yylineno; \
	yylloc.first_column=yycolumn;	yylloc.last_column=yycolumn+yyleng-1; yycolumn+=yyleng;
typedef union {
	int int_value;
	float float_value;
	char char_value;
	char type_id[32];
	char comment[200];
	struct ASTNode *ptr;
	char compassign_id[32];
} YYLVAL;
#define YYSTYPE YYLVAL

%}
id   [A-Za-z_][A-Za-z0-9_]*  
int    [0-9]+
float    ([0-9]*\.[0-9]+)|([0-9]+\.) 
%%
"//"[^\n]*      {int len = strlen(yytext); strncpy(yylval.comment, yytext + 2, len - 2);return COMMENT;}
{int}       {yylval.int_value=atoi(yytext); return INT;}

{float}		{yylval.float_value=atof(yytext);return FLOAT;}

\'.\' 		{yylval.char_value=yytext[1];return CHAR;}

"int"		{strcpy(yylval.type_id, yytext);
				return TYPE;}
"float"		{strcpy(yylval.type_id, yytext);
				return TYPE;}
"char"		{strcpy(yylval.type_id, yytext);
				return TYPE;}
"return"          {return RETURN;} 
"if"                  {return IF;} 
"else"              {return ELSE;} 
"while"            {return WHILE;}
"continue"			{return CONTINUE;}
"break"				{return BREAK;} 
{id}              {strcpy(yylval.type_id,    yytext); return ID;}
"++"		{return DPLUS;}
"--"		{return DMINUS;}
"+="|"-="|"*="|"/="		{strcpy(yylval.compassign_id, yytext);return COMPASSIGN;}
[=]        {return ASSIGNOP;}
[+]        {return PLUS;}
[-]        {return MINUS;}
[*]        {return STAR;}
[/]        {return DIV;}
"&&"       {return AND;}
"||"       {return OR;}
"!"		   {return NOT;}
">"|"<"|">="|"<="|"=="|"!=" {strcpy(yylval.type_id, yytext);return RELOP;}
[(]        {return LP;}
[)]        {return RP;}
[{]        {return LC;}
[}]        {return RC;}
"["			{return LB;}   
"]"			{return RB;}
[;]        {return SEMI;}   
[,]        {return COMMA;}
"["			{return LB;}   
"]"			{return RB;}
[ \t]      {;}   
[\n]       {yycolumn=0;}   
.          {printf("error in line %d\n",yylineno);}
%%
/*
int main()
{
yylex();
return 0;
}
*/
int yywrap()
{
return 1;
}

