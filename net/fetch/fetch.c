#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

void fetch(char* url);

int
main(int argc,char* argv[]){
    if(argc!=2){
        fprintf(stderr,"usage:%s url\n",argv[0]);
        exit(1);
    }
    fetch(argv[1]);
    return 0;
}

/*
 * resolve hostname and port from url,return query string.
 * If can't resolve hostname from url,hostname[0]='\0',query=NULL;
 * If can't resolve port from url,port=default_port;
 * If can't resolve query from url,hostname="0",return NULL.
 * If url doesn't end with '/',return NULL.
 * for example:
 * url:http://www.baidu.com:80/ hostname:www.baidu.com port:80 query:/
 * url:http://www.baidu.com/ hostname:www.baidu.com port:default_port query:/
 * url:www.baidu.com/query?h hostname:www.baidu.com port:default_port query:/query?h
 */
char*
parse_url(char* url,char* hostname,char* port,char* default_port){
    assert(url!=NULL&&hostname!=NULL&&port!=NULL);
    char* query=NULL;
    int plen=0;
    if(strstr(url,"://")!=NULL){
        if(strncmp(url,"http://",7)==0){
            url=url+strlen("http://");
        }else{
            hostname[0]='\0';
            return NULL;
        }
    }
    if((query=strchr(url,':'))!=NULL){
        strncpy(hostname,url,query-url);
        if((plen=strcspn(query+1,"/"))==0){
            strcpy(port,default_port);
        }else{
            strncpy(port,query+1,plen);
            query+=1+plen;
        }
    }else{
        query=strchr(url,'/');
        if(query==NULL){
            hostname[0]='0';
            hostname[1]='\0';
            return NULL;
        }else{
            strncpy(hostname,url,query-url);
            strcpy(port,default_port);
        }
    }
    if(*query=='\0'){
        return NULL;
    }else{
        return query;
    }
}

//print content found at url.
void
fetch(char* url){
    char port[6]={'\0'};
    char hostname[128]={'\0'};

    char* query=parse_url(url,hostname,port,"80");
    if(hostname[0]=='\0'){
        printf("Invalid URL!\n"
               "Can't resolve hostname!\n"
               "Support http:// only.\n");
        exit(1);
    }
    if(query==NULL){
        printf("Invalid URL!\n"
               "URL should end with '/' !\n");
        exit(1);
    }
    int sfd=socket(AF_INET,SOCK_STREAM,0);
    if(sfd==-1){
        perror("socket");
        exit(1);
    }
    struct hostent *hostinfo;
    struct sockaddr_in s_addr;
    hostinfo=gethostbyname(hostname);
    if(hostinfo==NULL){
        char errbuf[1024];
        sprintf(errbuf,"gethostbyname(%s):",hostname);
        herror(errbuf);
        exit(1);
    }else{
#ifdef DEBUG
        printf("Got destination host info.\n");
#endif
    }
    memset(&s_addr,0,sizeof(s_addr));
    //gdb.
    memcpy((char*)&s_addr.sin_addr.s_addr,(char*)hostinfo->h_addr,hostinfo->h_length);
#ifdef DEBUG
    printf("dest ip:%s\n",inet_ntoa(s_addr.sin_addr));
#endif
    s_addr.sin_family=AF_INET;
    s_addr.sin_port=htons(atoi(port));
    if(connect(sfd,(struct sockaddr*)&s_addr,sizeof(s_addr))==-1){
        perror("connect");
        exit(1);
    }else{
#ifdef DEBUG
        printf("connected.\n");
#endif
    }
    char req[1024]={'\0'};
    //request header should end with "\r\n"
    sprintf(req,\
            "GET %s HTTP/1.1\r\n"
            "Host:%s\r\n"
            "Accept:*\r\n"
            "Accept-Encoding:UTF-8\r\n"
            "Connection:close\r\n"
            "User-Agent:Linux\r\n"
            "\r\n",query,hostname);
    if(write(sfd,req,strlen(req))==-1){
        perror("send");
        exit(1);
    }else{
#ifdef DEBUG
        printf("request sent.\n");
#endif
    }
    struct timeval timeout={60,0};
    setsockopt(sfd,SOL_SOCKET,SO_RCVTIMEO,(char*)&timeout,sizeof(struct timeval));
    char res[1024];
    int size_recv=0,total_size=0;
    while(1){
        memset(res,0,sizeof(res));
        if((size_recv=read(sfd,res,sizeof(res)))==-1){
            if(errno==EWOULDBLOCK||errno==EAGAIN){
                printf("Recv timeout.\n");
                break;
            }else if(errno==EINTR){
                printf("Interrupt by signal.\n");
                continue;
            }else if(errno==ENOENT){
                printf("Recv RST segement.\n");
                break;
            }else{
                printf("Unknow error!\n");
                break;
            }
        }else if(size_recv==0){
            printf("\nPeer close.\n");
            break;
        }else{
            total_size+=size_recv;
            printf("%s",res);
        }
    }
    printf("Reply received,total_size=%d bytes.\n",total_size);
    close(sfd);
}
