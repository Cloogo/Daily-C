#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

static void
err_exit(char* dscr){
    fprintf(stderr,"%s",dscr);
    exit(1);
}

//prints links in webpage.
int
main(int argc,char* argv[]){
    if(argc!=2){
        err_exit("Please specify filepath first!\n");
    }
    int fd=open(argv[1],O_RDONLY);
    if(fd==-1){
        err_exit("read file error!\n");
    }
    struct stat fs;
    stat(argv[1],&fs);
    char* text=mmap(NULL,fs.st_size,PROT_READ,MAP_PRIVATE,fd,0);
    if(text==MAP_FAILED){
        err_exit("mmap error!\n");
    }
    char* pattern="\" *(http://[^\"]+)";
    regex_t pre;
    size_t nmatch=2;
    regmatch_t matches[nmatch];
    if(regcomp(&pre,pattern,REG_EXTENDED)){
        fprintf(stderr,"Can't compile regular expression pattern!\n");
        goto clean;
    }
    char* cursor=text;
    while(1){
        if(regexec(&pre,cursor,nmatch,matches,0)==REG_NOMATCH){
            goto clean;
        }
        size_t offset=0;
        for(int i=0;i<nmatch;i++){
            if(matches[i].rm_so==(size_t)-1){
                break;
            }
            if(i==0){
                offset=matches[0].rm_eo;
            }else{
                printf("%.*s\n",matches[i].rm_eo-matches[i].rm_so,cursor+matches[i].rm_so);
            }
        }
        cursor+=offset;
    }
clean:
    munmap(text,fs.st_size);
    regfree(&pre);
    return 0;
}
