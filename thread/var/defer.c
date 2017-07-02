#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

void* worker(void* data){
    printf("%d defer %lld ticks\n",pthread_self(),clock()-(clock_t)data);
    clock_t start=clock();
    int sum=0;
    for(int i=0;i<10000000;i++){
        sum+=i;
        if(i==5000){
//            printf("%d sum defer %lld ticks\n",pthread_self(),clock()-start);
        }
    }
//    printf("%d %lld ticks\n",pthread_self(),clock()-start);
}

int main(){
    clock_t start=clock();
    pthread_t pid[1000];
    for(int i=0;i<1000;i++){
        pthread_create(&pid[i],NULL,worker,(void*)clock());
    }
    for(int i=0;i<1000;i++){
        pthread_join(pid[i],NULL);
    }
//    printf("total %lld ticks\n",clock()-start);
}
