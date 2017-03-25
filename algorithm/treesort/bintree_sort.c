#include "bintree_sort.h"
#include <assert.h>
#include <stdlib.h>

struct Tree{
    int val;
    Tree l;
    Tree r;
};

static void add(Tree* t,const int val);
static void release(Tree* t);
static void toarr(Tree t,int* arr);

int idx;

#ifdef UNIT_TEST
unsigned int mem_used=0;
#endif

void 
Bintree_sort(int* arr,int n){
    assert(arr!=NULL);
    Tree t=NULL;
    idx=0;
    for(int i=0;i<n;i++){
        add(&t,arr[i]);
    }
    toarr(t,arr);
    release(&t);
}

static void
add(Tree* t,const int val){
    if(*t==NULL){
        *t=malloc(sizeof(**t));
#ifdef UNIT_TEST
        mem_used+=sizeof(**t);
#endif
        assert(*t!=NULL);
        (*t)->val=val;
        (*t)->l=NULL;
        (*t)->r=NULL;
        return;
    }
    if(val<(*t)->val){
        add(&((*t)->l),val);
    }else{
        add(&((*t)->r),val);
    }
}

static void
toarr(Tree t,int* arr){
    if(t==NULL){
        return;
    }
    toarr(t->l,arr);
    arr[idx++]=t->val;
    toarr(t->r,arr);
}

static void
release(Tree* t){
    if(*t==NULL){
        return;
    }
    release(&((*t)->l));
    release(&((*t)->r));
    free(*t);
}

#ifdef UNIT_TEST
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#define N 1000000

int arr[N];
int
main(){
    srand(time(NULL));
    printf("N\ttime_cost/ms\tmem_used/kb\n");
    for(int m=10000;m<100000;m+=10000){
        for(int n=0;n<m;n++){
            arr[n]=rand()%N;
        }
        clock_t start=clock();
        Bintree_sort(arr,m);
        for(int l=0;l<m-1;l++){
            assert(arr[l]<=arr[l+1]);
        }
        clock_t end=clock();
        printf("%d\t",m);
        printf("%.2f\t",(float)(end-start)/CLOCKS_PER_SEC*1000);
        printf("%.2f\t",(float)mem_used/1024);
        printf("\n");
        mem_used=0;
    }
    return 0;
}
#endif
