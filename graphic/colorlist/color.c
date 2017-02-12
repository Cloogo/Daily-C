#include <stdio.h>
#include <stdlib.h>

int
main(){
    printf("font color list:\n");
    for(int i=30;i<40;i++){
        printf("\e[0;%dm",i);
        printf("%dm\n",i);
        printf("\e[0m");
    }
    printf("background color list:\n");
    for(int i=40;i<50;i++){
        printf("\e[%dm",i);
        printf("%dm\n",i);
        printf("\e[0m");
    }
}
