#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

void print3(int n);
void print_binary(int n);

int main(){
    print3(INT_MIN);
    print3(0);
    print3(INT_MAX);
    return 0;
}

void print3(int n){
    print_binary(n);
    print_binary(~n);
    print_binary(~n+1);
}

void print_binary(int n){
    char pre[][5]={"0000\0","0001\0","0010\0","0011\0",
               "0100\0","0101\0","0110\0","0111\0",
               "1000\0","1001\0","1010\0","1011\0",
               "1100\0","1101\0","1110\0","1111\0"};
    printf("(%+11d)10=(",n);
    for(int i=0;i<8;i++){
        printf("%s",pre[(n&0xf0000000)>>28]);
        n<<=4;
    }
    printf(")2\n");
}
