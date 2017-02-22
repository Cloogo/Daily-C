#ifndef _STACK_H
#define _STACK_H

#ifdef __cplusplus
extern "C"{
#endif

#include "validator.h"
#define T Stack 
struct T{
    int top;
    const size_t size;
    char* data;
    const size_t bytes;
    Validator validator;
};

typedef struct T* T;

extern bool Stack_push(T s,void* e);
extern void* Stack_pop(T s);
extern const void* Stack_top(const T s);
extern const void* Stack_bottom(const T s);
extern size_t Stack_size(const T s);
extern size_t Stack_cap(const T s);
extern bool Stack_reset(T s,int idx,void* new,void* old);
extern const void* Stack_movesp(T s,const int steps);

#define Stack_new(data) {\
    0,sizeof(data)/sizeof(data[0]),(char*)data,sizeof(data[0]),NULL\
}

#define Stack_new_with_validator(data,validator) {\
    0,sizeof(data)/sizeof(data[0]),(char*)data,sizeof(data[0]),validator\
}
#undef T
#ifdef __cplusplus
}
#endif

#endif
