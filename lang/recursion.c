#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX 100

char dict[7][10]={"fat","fate","at","eat","este","rn","estern"};

//seperates the sent with space into word.
//each word can be found in dict.
void find(char* sent,int idx,char ans[MAX][10]){
    if(*sent=='\0'){
        for(int i=0;i<idx-1;i++){
            printf("%s ",ans[i]);
        }
        printf("%s",ans[idx-1]);
        printf("\n");
    }
    for(int i=0;i<7;i++){
        char* next=strstr(sent,dict[i]);
        if(next==sent){
            strcpy(ans[idx],dict[i]);
            find(next+strlen(dict[i]),idx+1,ans);
        }
    }
}

int
main(){
    char ans[MAX][10];
    find("fateatestern",0,ans);
    return 0;
}
