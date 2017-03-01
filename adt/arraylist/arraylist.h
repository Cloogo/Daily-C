#ifndef _ARRAYLIST_H
#define _ARRAYLIST_H
#define T ArrayList
#ifdef __cplusplus
extern "C"{
#endif
typedef struct T{
    size_t cap;
    size_t size;
    void** data;
}T;

extern void ArrayList_new(T* const array,int cap);
extern void ArrayList_free(T* const array);
extern void* ArrayList_get(T* const array,const int idx_);
extern int ArrayList_put(T* const array,void* const e);
extern void* ArrayList_remove(T* const array,const int idx_);
extern void* ArrayList_replace(T* const array,const int idx_,void* const e);
extern int ArrayList_indexof(const T* const array,void* const e,int(*cmp_)(const void*e1,const void*e2));
extern int ArrayList_insert_at(T* const array,int idx_,void* const e);
extern void ArrayList_apply(T* const array,void(*apply_)(void*e));
extern size_t ArrayList_cap(const T* const array);
extern size_t ArrayList_size(const T* const array);
#ifdef __cplusplus
}
#endif
#undef T
#endif
