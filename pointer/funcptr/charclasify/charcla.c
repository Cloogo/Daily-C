#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>

void print(char* title,int (*sortouter)(int));

int
main(){
    print("punctuation",ispunct);
    print("space",isspace);
    print("blank",isblank);
    print("ctl",iscntrl);
    print("printable(include space)",isprint);
    print("grapical(exclude space)",isgraph);
    print("ascii",isascii);
    print("xdigit",isxdigit);
    print("alpha",isalpha);
    print("num",isdigit);
    print("lower",islower);
    print("upper",isupper);
}

void
print(char* title,int (*sortouter)(int)){
    printf("%s:\n",title);
    for(int i=0;i<UCHAR_MAX+1;i++){
        if(sortouter(i)){
            printf("%d:%c\n",i,i);
        }
    }
    printf("\n");
}
