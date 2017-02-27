#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int 
comlen(const char* p,const char* q){
    int i=0;
    for(;*p!='\0'&&*p==*q;p++,q++){
        i++;
    }
    return i;
}

static int
pstrcmp(const void* p1,const void* p2){
    const char* pstr1=*(const char* const*)p1;
    const char* pstr2=*(const char* const*)p2;
    return strcmp(pstr1,pstr2);
}

const char*
maxcomsubstr(const char* str){
    static char buf[1024];
    int slen=strlen(str);
    const char* pstr=str;
    const char* psuffixes[slen];
    int i;
    for(i=0;*pstr!='\0';){
        psuffixes[i++]=pstr++;
    }
    qsort(psuffixes,slen,sizeof(char*),(int(*)(const void*,const void*))pstrcmp);
    int maxlen=0;
    int len=0;
    int maxi=0;
    for(i=0;i<slen-1;i++){
        len=comlen(psuffixes[i],psuffixes[i+1]);
        if(len>maxlen){
            maxlen=len;
            maxi=i;
        }
    }
    sprintf(buf,"%.*s",maxlen,psuffixes[maxi]);
    return buf;
}
