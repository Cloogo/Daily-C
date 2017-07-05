#include <stdio.h>
#include <stdlib.h>

#define offsetof(type,member) ((size_t)(&(((type*)0)->member)))
#define container_of(type,ptr,member) ({\
                                   const typeof((((type*)0))->member)* mptr=(ptr);\
                                   (type*)((char*)mptr-offsetof(type,member));\
                                   })

struct test{
    int passed;
    int failed;
    char name[15];
    struct para{
        void* data;
        void(*mock)(void*data);
    }para;
    double benchtime;
    struct test* next;
};

int main(){
    printf("%lu\n",offsetof(struct test,passed));
    printf("%lu\n",offsetof(struct test,failed));
    printf("%lu\n",offsetof(struct test,name));
    printf("%lu\n",offsetof(struct test,para));
    printf("%lu\n",offsetof(struct test,benchtime));
    struct test unit={0,0,"empty",{NULL,NULL},0.0,NULL};
    struct test* punit=container_of(struct test,&unit.para,para);
    printf("%s\n",punit->name);
}
