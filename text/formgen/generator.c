#include <stdio.h>
#include <stdlib.h>

char infos[7][32]={
    "Cloogo",
    "20",
    "Software Engineer"
    ,"Guangzhou",
    "uuu",
    "12312341234",
    "test@gmail.com"
};

//replace variants in form of $? from form.txt with chars in infos.

int
main(){
    FILE* istream=fopen("form.txt","r");
    if(istream==NULL){
        perror("fopen()");
        exit(1);
    }
    char next;
    while((next=fgetc(istream))!=EOF){
        if(next=='$'){
            next=fgetc(istream);
            if(next!=EOF){
                switch(next){
                case '$':
                    fputc(next,stdout);
                    break;
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                    fputs(infos[next-'0'],stdout);
                    break;
                default:
                    fputc(next,stdout);
                    break;
                }
            }
        }else{
            fputc(next,stdout);
        }
    }
    fclose(istream);
    return 0;
}
