#include <stdio.h>
#include <stdlib.h>

//Raise Bus error.
int
main(){
    int arr[4]={1,2,3,4};
    for(unsigned int i=3;i>=0;i--){
        printf("arr[%u]=%d\n",i,arr[i]);
    }
    exit(0);
}
