#include <stdio.h>
#include <stdlib.h>

typedef unsigned long long int uint64;
typedef unsigned char uint8;

int pc[256]={0};

int popcount(uint64 n);

__attribute__((constructor(1001)))
void init(){
    for(int i=0;i<256;i++){
        pc[i]=pc[i/2]+(i&1);
    }
}

int 
main(){
    printf("popcount(1023)=%d\n",popcount(1023));
    printf("popcount(1024)=%d\n",popcount(1024));
    exit(0);
}

//returns numbers of set bits of n
int 
popcount(uint64 n){
    return pc[(uint8)(n>>(0*8))]+pc[(uint8)(n>>(1*8))]+
           pc[(uint8)(n>>(2*8))]+pc[(uint8)(n>>(3*8))]+
           pc[(uint8)(n>>(4*8))]+pc[(uint8)(n>>(5*8))]+
           pc[(uint8)(n>>(6*8))]+pc[(uint8)(n>>(7*8))];
}
