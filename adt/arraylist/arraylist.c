#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "arraylist.h"
#define T ArrayList
#define IDX(idx) ((idx<0)?((idx)+array->size):idx)

void
ArrayList_new(T* const array,int cap){
    assert(array!=NULL&&cap>0);
    array->size=0;
    array->cap=cap;
    array->data=(void**)malloc(sizeof(void*)*cap);
}

void
ArrayList_free(T* const array){
    assert(array!=NULL);
    free(array->data);
}

size_t
ArrayList_size(const T* const array){
    assert(array!=NULL);
    return array->size;
}

size_t
ArrayList_cap(const T* const array){
    assert(array!=NULL);
    return array->cap;
}

void*
ArrayList_get(T* const array,const int idx_){
    assert(array!=NULL);
    int idx=IDX(idx_);
    assert(idx<array->size&&idx>=0);
    return array->data[idx];
}

int
ArrayList_put(T* const array,void* const e){
    assert(array!=NULL&&e!=NULL);
    assert(array->size<array->cap);
    array->data[array->size++]=e;
    return array->size;
}

void*
ArrayList_remove(T* const array,const int idx_){
    assert(array!=NULL);
    int idx=IDX(idx_);
    assert(idx<array->size&&idx>=0);
    void* old=array->data[idx];
    memmove(array->data+idx,array->data+idx+1,(array->size-(idx+1))*sizeof(void*));
    array->size--;
    return old;
}

void*
ArrayList_replace(T* const array,const int idx_,void* const e){
    assert(array!=NULL&&e!=NULL);
    int idx=IDX(idx_);
    assert(idx<array->size&&idx>=0);
    void* old=array->data[idx];
    array->data[idx]=e;
    return old;
}

int
ArrayList_insert_at(T* const array,int idx_,void*const e){
    assert(array!=NULL&&e!=NULL);
    assert(array->size<array->cap);
    int idx=IDX(idx_);
    assert(idx>=0&&idx<=array->size);
    memmove(array->data+idx+1,array->data+idx,(array->size-idx)*sizeof(void*));
    array->data[idx]=e;
    array->size++;
    return array->size;
}


static int 
default_cmp(const void* e1,const void* e2){
    return e1!=e2;
}

int
ArrayList_indexof(const T* const array,void* const e,int(*cmp_)(const void*,const void*)){
    assert(array!=NULL&&e!=NULL);
    if(array->size==0){
        return -1;
    }
    int(*cmp)(const void*,const void*);
    if(cmp_==NULL){
        cmp=default_cmp;
    }else{
        cmp=cmp_;
    }
    for(int i=0;i<array->size;i++){
        if(cmp(array->data[i],e)==0){
            return i;
        }
    }
    return -1;
}

static void
default_apply(void* e){
    free(e);
}

void
ArrayList_apply(T* const array,void(*apply_)(void*e)){
    assert(array!=NULL);
    void(*apply)(void*);
    if(apply_==NULL){
        apply=default_apply;
    }else{
        apply=apply_;
    }
    for(int i=0;i<array->size;i++){
        apply(array->data[i]);
    }
}
