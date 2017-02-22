#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include "stack.h"

static bool
validate(Validator v,void* e){
    return !v||(NULL!=v&&v->validate(v,e));
}

static bool
is_empty(Stack const s){
    return s->top==0;
}

static bool
is_full(Stack const s){
    return s->top==s->size;
}

bool
Stack_push(Stack s,void* e){
    assert(s!=NULL);
    if(is_full(s)||!validate(s->validator,e)){
        return false;
    }
    memcpy(s->data,e,s->bytes);
    s->data+=s->bytes;
    s->top++;
    return true;
}

void*
Stack_pop(Stack s){
    assert(s!=NULL);
    if(is_empty(s)){
        return NULL;
    }
    s->top--;
    s->data-=s->bytes;
    return s->data;
}

size_t
Stack_cap(const Stack s){
    assert(s!=NULL);
    return s->size;
}

size_t
Stack_size(const Stack s){
    assert(s!=NULL);
    return s->top;
}

const void*
Stack_top(const Stack s){
    assert(s!=NULL);
    if(is_empty(s)){
        return NULL;
    }
    const void* e=(const void*)(s->data-s->bytes);
    return e;
}

const void*
Stack_bottom(const Stack s){
    assert(s!=NULL);
    if(is_empty(s)){
        return NULL;
    }
    const void* e=(const void*)(s->data-s->top*s->bytes);
    return e;
}

/*once the stack is full,unable to reset the stack.
 * the function of Stack_reset(s,Stack_size(s),new,old)
 * is similar to Stack_push(s,new).
 */
bool
Stack_reset(Stack s,int idx,void* new,void* old){
    assert(NULL!=s&&NULL!=new&&NULL!=old);
    if(s->top<idx||idx<0){
        return false;
    }
    if(!is_full(s)&&validate(s->validator,new)){
        char* pos=s->data-s->top*s->bytes+idx*s->bytes;
        memcpy(old,pos,s->bytes);
        memcpy(pos,new,s->bytes);
        if(s->top==idx){
            s->data+=s->bytes;
            s->top++;
        }
        return true;
    }else{
        return false;
    }
}

/*once called,unable to reach the top.
 * So Stack_movesp(s,-1)  is similar to Stack_pop(s).
 */
const void*
Stack_movesp(Stack s,const int steps){
    assert(NULL!=s);
    if(s->top+steps>=s->size||s->top+steps<0){
        return NULL;
    }
    s->data+=steps*s->bytes;
    s->top+=steps;
    const void* e=s->data;
    return e;
}
