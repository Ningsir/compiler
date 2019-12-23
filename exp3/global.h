#ifndef __GLOBAL
#define __GLOBAL
int cur_fun_pos; //当前函数名在符号表中的位置
int jump = 0;    //jump=1表示语义分析进入循环体中，此时可以使用break、continue语句
bool has_return = false;
bool in_if = false;
bool in_while = false;
int __t1 = 0;
int *offset = &__t1;
int __t2 = 0;
int *zero = &__t2;
FILE *objectFile;
#endif