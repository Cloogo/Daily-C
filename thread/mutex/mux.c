#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void* worker(void* id);
int cmp_and_swap(int* muxval,int testval,int newval);
int mux;
int count=0;

int main(){
    pthread_t jobs[1000];
    mux=0;
    for(int i=0;i<1000;i++){
        pthread_create(&jobs[i],NULL,worker,(void*)i);
    }
    for(int i=0;i<1000;i++){
        pthread_join(jobs[i],NULL);
    }
    return 0;
}

void* worker(void* _id){
    int id=(int)_id;
    int flag=0;
    while(cmp_and_swap(&mux,0,1)==1){
        if(flag==0){
//            printf("worker %d waiting...\n",id);
            flag=1;
        }
    }
//    printf("worker %d working...\n",id);
    count++;
    printf("%d\n",count);
//    printf("worker %d done.\n",id);
    mux=0;
    return NULL;
}

int cmp_and_swap(int* muxval,int testval,int newval){
    int oldval=*muxval;
    if(oldval==testval)*muxval=newval;
    return oldval;
}
