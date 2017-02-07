#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>

unsigned long count[UCHAR_MAX+1];

//read chars from standard input and calculate char frequency.
int
main(){
    int c=0;
    while(scanf("%c",&c)!=EOF){
        count[c]++;
    }
    for(int i=0;i<UCHAR_MAX+1;i++){
        if(count[i]){
            printf("%d %c:%lu\n",i,isprint(i)?i:'-',count[i]);
        }
    }
    return 0;
}
