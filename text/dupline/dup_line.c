#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <search.h>
#define MAX_HSIZE 2048//hashtable size
#define MAX_LINE_LEN 1024

char keys[MAX_HSIZE][MAX_LINE_LEN]={{'\0'}};
int keynum=0;

void
count_line(char* filename){
    FILE* istream;
    if(filename==NULL){
        istream=stdin;
    }else{
        istream=fopen(filename,"r");
        if(istream==NULL){
            fprintf(stderr,"can't open %s.\n",filename);
            exit(1);
        }
    }
    ENTRY e,*ep;
    char line[MAX_LINE_LEN]={'\0'};
    while(fgets(line,sizeof(line),istream)!=NULL){
        line[strlen(line)-1]='\0';
        e.key=line;
        ep=hsearch(e,FIND);
        if(ep==NULL){
            int *c=malloc(sizeof(int));
            *c=1;
            strcpy(keys[keynum],line);
            e.key=keys[ki];
            e.data=(void*)c;
            hsearch(e,ENTER);
            keynum++;
        }else{
            int* c=(int*)ep->data;
            *c=*c+1;
            ep->data=(void*)c;
        }
    }
    fclose(istream);
}

//print duplicate line in file(s).
int
main(int argc,char* argv[]){
    if(hcreate(MAX_HSIZE)==0){
        fprintf(stderr,"hcreate:%s\n",strerror(errno));
        exit(1);
    }
    if(argc==1){
        count_line(NULL);
    }else{
        for(int i=1;i<argc;i++){
            count_line(argv[i]);
        }
    }
    ENTRY e,*ep;
    int *c;
    for(int i=0;i<ki;i++){
        e.key=keys[i];
        ep=hsearch(e,FIND);
        if(ep!=NULL){
            c=(int*)(ep->data);
            if(*c>1){
                printf("%s:%d\n",e.key,*c);
            }
            free(c);
        }else{
        }
    }
    hdestroy();
    return 0;
}
