#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

volatile int count=0;

void* worker(void* data){
    for(int i=0;i<1000;i++)
        count++;
}

int main(){
    pthread_t p[1000];
    for(int i=0;i<1000;i++){
        pthread_create(&p[i],NULL,worker,NULL);
    }
    for(int i=0;i<1000;i++){
        pthread_join(p[i],NULL);
    }
    printf("%d\n",count);
}
