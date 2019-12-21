
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     CONST_INT = 258,
     CONST_CHAR = 259,
     ID = 260,
     TYPE = 261,
     RELOP = 262,
     CONST_FLOAT = 263,
     COMMENT = 264,
     COMPASSIGN = 265,
     _LP = 266,
     RP = 267,
     LC = 268,
     RC = 269,
     LB = 270,
     RB = 271,
     PLUS = 272,
     MINUS = 273,
     STAR = 274,
     DIV = 275,
     ASSIGNOP = 276,
     AND = 277,
     OR = 278,
     NOT = 279,
     DPLUS_L = 280,
     DPLUS_R = 281,
     DMINUS_L = 282,
     DMINUS_R = 283,
     COMMA = 284,
     SEMI = 285,
     ARRAY_CALL = 286,
     RETURN = 287,
     IF = 288,
     IF_ELSE = 289,
     WHILE = 290,
     RETURN_NULL = 291,
     FUNC_CALL = 292,
     ARGS = 293,
     EXT_VAR_DEF = 294,
     FUNC_DEC = 295,
     FUNC_DEF = 296,
     VAR_LIST = 297,
     VAR_ID = 298,
     VAR_INIT = 299,
     INIT_VALUE = 300,
     FUN_ID = 301,
     PARAMS_LIST = 302,
     PARAMS = 303,
     EXT_DEF_LIST = 304,
     STM = 305,
     STM_LIST = 306,
     STMT_VAR_DEF = 307,
     ARRAY_ID = 308,
     ARRAY_INIT = 309,
     ARRAY_ELE_ASSIGN = 310,
     ARRAY_ASSIGN = 311,
     VALUE_LIST = 312,
     CONTINUE = 313,
     BREAK = 314,
     COMPASSIGN_ID = 315,
     DMINUS = 316,
     DPLUS = 317,
     UMINUS = 318,
     LOWER_THEN_ELSE = 319,
     ELSE = 320
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1676 of yacc.c  */
#line 12 "exp.y"

	int int_value;
	float float_value;
    char char_value;
	char type_id[32];//标识符
    char compassign_id[32];//复合赋值符号
    char comment[200];
	struct ASTNode *ptr;



/* Line 1676 of yacc.c  */
#line 129 "exp.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;

#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif

extern YYLTYPE yylloc;

