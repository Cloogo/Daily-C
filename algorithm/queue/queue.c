#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../../adt/stack/stack.h"
#define QUEUE_MAX 1024

int s1_arr[QUEUE_MAX];
struct Stack s1=Stack_new(s1_arr);
int s2_arr[QUEUE_MAX];
struct Stack s2=Stack_new(s2_arr);

int
Queue_pop(){
    int* e;
    if(Stack_size(&s2)!=0){
        e=(int*)Stack_pop(&s2);
        return *e;
    }else{
        if(Stack_size(&s1)==0){
            return -1;
        }else{
            while(Stack_size(&s1)!=1){
                e=(int*)Stack_pop(&s1);
                Stack_push(&s2,e);
            }
            e=(int*)Stack_pop(&s1);
            return *e;
        }
    }
}

bool
Queue_push(int e){
    if(Stack_size(&s1)!=0){
        return Stack_push(&s1,&e);
    }else{
        while(Stack_size(&s2)!=0){
            int* t=(int*)Stack_pop(&s2);
            Stack_push(&s1,t);
        }
        return Stack_push(&s1,&e);
    }
}

//use two stack to implement a queue.
int
main(){
    for(int i=0;i<10;i++){
        Queue_push(i);
        if(i==1){
            printf("%d\n",Queue_pop());
            printf("%d\n",Queue_pop());
        }
    }
    for(int i=0;i<10;i++){
        printf("%d\n",Queue_pop());
    }
    return 0;
}
