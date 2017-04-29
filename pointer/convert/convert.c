#include <stdio.h>
#include <stdlib.h>

long int r;

void modify(const int* const int_ptr);
void shell();


int
main(){
    int i=10;
    int* int_ptr=&i;
    int** p_int_ptr=&int_ptr;
    r=(long int)p_int_ptr;
    printf("before modify:int_ptr=%p\n",int_ptr);
    modify(int_ptr);
    printf("after modify:int_ptr=%p\n",int_ptr);
    ((void(*)())int_ptr)();
}

void modify(const int* const int_ptr){
    int mi=1;
    long int diff=r-(long int)(&mi);
    long int** p_int_ptr=(long int**)((long int)(&mi)+diff);
    *p_int_ptr=(long int*)&shell;
    printf("modify called.\n");
}

void shell(){
    printf("Hello shell!\n");
}
