#include "stdio.h"
#include "stdlib.h"

int main(){
    int *a = (int*)malloc(sizeof(int));
    *a = 10;
    int *b = (int *)0;
    printf("int:%d\n", *a);
    return 0;
}