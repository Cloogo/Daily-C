#include <stdio.h>
#include <assert.h>
#include "stack.h"
#include "validator.h"

bool
stack_with_no_validator_test(){
    int arr[10];
    struct Stack s=Stack_new(arr);
    for(int i=0;i<100;i++){
        Stack_push(&s,&i);
    }
    assert(Stack_size(&s)==10);
    const int* e=Stack_top(&s);
    assert(*e==9);
    e=Stack_bottom(&s);
    assert(*e==0);
    assert(NULL==Stack_movesp(&s,0));
    assert(NULL==Stack_movesp(&s,1));
    assert(NULL==Stack_movesp(&s,-Stack_size(&s)-1));
    e=Stack_movesp(&s,-1);
    assert(*e==9);
    assert(NULL==Stack_movesp(&s,1));//something wonderfull!
    for(int i=8;i>=0;i--){
        e=Stack_pop(&s);
        assert(*e==i);
    }
    assert(Stack_size(&s)==0);
    assert(NULL==Stack_pop(&s));

    int n[1];
    n[0]=1;
    struct Stack s2=Stack_new(n);
    int new=10;
    int old;
    assert(Stack_reset(&s2,Stack_size(&s2),&new,&old)==true);
    assert(Stack_size(&s2)==1);
    assert(old==1);
    e=Stack_top(&s2);
    assert(*e==10);
    e=Stack_bottom(&s2);
    assert(*e==10);
    assert(Stack_reset(&s2,1,&new,&old)==false);
    assert(Stack_reset(&s2,0,&new,&old)==false);
    return true;
}

int
main(){
    stack_with_no_validator_test();
    return 0;
}
