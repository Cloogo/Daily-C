#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arraylist.h"

typedef struct{
    int n;
    int* nums;
}NumChunk;

void
print(void* chunk){
    NumChunk* c=(NumChunk*)chunk;
    for(int i=0;i<c->n;i++){
        printf("%d\t",c->nums[i]);
    }
    printf("\n");
}

void
myfree(void* chunk){
    NumChunk* c=(NumChunk*)chunk;
    free(c->nums);
    free(c);
}

int
main(){
    int nchunk=10;
    struct ArrayList list;
    ArrayList_new(&list,nchunk);
    int nnum=10;
    for(int i=0;i<nchunk;i++){
        NumChunk* c=malloc(sizeof(*c));
        c->n=nnum;
        c->nums=malloc(nnum*sizeof(int));
        memset(c->nums,0,sizeof(int)*nnum);
        for(int j=0;j<nnum;j++){
            c->nums[j]=j;
        }
        ArrayList_put(&list,c);
    }
    ArrayList_apply(&list,print);
    ArrayList_apply(&list,myfree);
    ArrayList_free(&list);
}
