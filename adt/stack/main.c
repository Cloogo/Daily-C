#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "stack.h"
#include "validator.h"

int
main(){
    srand(time(NULL));
    int buf[101];
    struct Rang r={.min=0,.max=100};
    struct Validator v=Validator_new_rang(&r);
    struct Stack s=Stack_new_with_validator(buf,&v);
    int e;
    for(int i=0;i<100;i++){
        e=rand()&0xfff;
        Stack_push(&s,&e);
    }
    printf("Range from 0 to 100:\n");
    while(s.top!=0){
        int* e=Stack_pop(&s);
        printf("%d\n",*e);
    }
    printf("Num larger than 100:\n");
    struct Previous p={.val=100};
    struct Validator v2=Validator_new_greater(&p);
    struct Stack s2=Stack_new_with_validator(buf,&v2);
    for(int i=0;i<100;i++){
        e=rand()%0xfff;
        Stack_push(&s2,&e);
    }
    while(s2.top!=0){
        int* e=Stack_pop(&s2);
        printf("%d\n",*e);
    }
    return 0;
}
