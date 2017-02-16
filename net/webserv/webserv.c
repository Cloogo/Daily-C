#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <ctype.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>
#define DEFAULT_PORT 80
#define DEFAULT_BACKLOG 511
#define DEFAUTL_DIR "/var/www"
#define MAX_URL_LEN 256
#define MAX_HEADER_LEN 4096
#define MAX_BODY_LEN 64*1024
#define MAX_FILETYPE_LEN 36

typedef enum Method{
    GET,
    POST
}Method;

typedef enum Proto{
    HTTP_1_0,
    HTTP_1_1
}Proto;

typedef enum Error{
    NO,
    PEER_CLOSED,
    INTERNAL,
    NOT_FOUND,
    METHOD_NOT_SUPPORTED,
    REQUEST_ERROR,
    MIME_NOT_SUPPORTED,
    PROTO_NOT_SUPPORTED
}Error;

typedef struct Request{
    Method m;
    Proto proto;
    char url[MAX_URL_LEN];
    char header[MAX_HEADER_LEN];
    char body[MAX_BODY_LEN];
}Request;

typedef struct Respond{
    char header[MAX_HEADER_LEN];
    int content_len;
    char filetype[MAX_FILETYPE_LEN];
}Respond;

typedef struct Serv{
    struct sockaddr_in s;
    int backlog;
    int fd;
    char* dir;
}Serv;

typedef struct Client{
    int fd;
    char path[MAX_URL_LEN];
}Client;

struct ext{
    const char* name;
    const char* filetype;
}exts_tab[]={
    {".html","text/html"},
    {".jpg","image/jpeg"},
    {0,0}
};

int getservopt(Serv* s,char* conf);
int getsocket(Serv* s);
void openserver(Serv* s);
void* provserv(void* _sfd);
Error getrequest(Request* r,int sfd);
void elog(Error e,int fd);

int
main(){
    Serv s;
    if(getservopt(&s,"http.conf")==-1){
        fprintf(stderr,"Read configuration file error!\n");
        exit(1);
    }
    if(getsocket(&s)==-1){
        fprintf(stderr,"getsocket error!\n");
        exit(1);
    }
    openserver(&s);
    return 0;
}

void
openserver(Serv* s){
    int clis[FD_SETSIZE];
    for(int i=0;i<FD_SETSIZE;i++){
        clis[i]=-1;
    }
    fd_set rfs,_rfs;
    FD_ZERO(&rfs);
    FD_SET(s->fd,&rfs);
    int maxfd=s->fd;
    int maxi=0;
    while(1){
        _rfs=rfs;
        struct timeval to={.tv_sec=0,.tv_usec=1};
        int nready=select(maxfd+1,&_rfs,NULL,NULL,&to);
        if(nready>0){
            if(FD_ISSET(s->fd,&_rfs)){
                int cli;
                struct sockaddr_in cliaddr;
                int len=sizeof(cliaddr);
                if((cli=accept(s->fd,(struct sockaddr*)&cliaddr,&len))==-1){
                    fprintf(stderr,"accept err!");
                    continue;
                }
#ifdef DEBUG
                printf("accepted client ip: %s\n",inet_ntoa(cliaddr.sin_addr));
#endif
                for(int i=0;i<FD_SETSIZE;i++){
                    if(clis[i]==-1){
                        clis[i]=cli;
                        FD_SET(cli,&rfs);
                        if(cli>maxfd){
                            maxfd=cli;
                        }
                        if(i>maxi){
                            maxi=i;
                        }
                        if(i+1==FD_SETSIZE){
                            fprintf(stderr,"too many connection.\n");
                        }
                        break;
                    }
                }
                nready--;
                if(nready<=0){
                    continue;
                }
            }
            for(int i=0;i<=maxi;i++){
                if(clis[i]==-1){
                    continue;
                }
                if(FD_ISSET(clis[i],&_rfs)){
                    pthread_t id;
                    Client* c=malloc(sizeof(*c));
                    c->fd=clis[i];
                    strcpy(c->path,s->dir);
                    pthread_create(&id,NULL,provserv,(void*)c);
                    FD_CLR(clis[i],&rfs);
                    clis[i]=-1;
                    nready--;
                    if(nready<=0){
                        break;
                    }
               }
           }
        }else if(nready==0){
        }else{
            fprintf(stderr,"select()");
        }
    }
}

Error
getrequest(Request* r,int sfd){
    memset(r,0,sizeof(*r));
    char buf[1024];
    int nread=0;
    int header_tag=0;
    char* sep;
    char* phdr=r->header;
    char* phdr_end=r->header+MAX_HEADER_LEN;
    char* pbody=r->body;
    char* pbody_end=r->body+MAX_BODY_LEN;
    char* purl=r->url;
    int error;
    while(1){
        struct timeval to={0,1};
        //pay attention to the feather of function read.(set to block defaultly).
        if(setsockopt(sfd,SOL_SOCKET,SO_RCVTIMEO,(char*)&to,sizeof(struct timeval))==-1){
            return INTERNAL;
        }
        nread=read(sfd,buf,sizeof(buf));
        if(nread>0){
            if(!header_tag){
                if((sep=strstr(buf,"\r\n\r\n"))!=NULL){
                    if(phdr+(sep-buf)<phdr_end){
                        strncpy(phdr,buf,sep-buf);
                        phdr+=sep-buf;
                        sep+=4;
                        if(sep<buf+nread){
                            strncpy(pbody,sep,buf+nread-sep);
                            pbody+=buf+nread-sep;
                        }
                    }else{
                        strncpy(phdr,buf,phdr_end-phdr);
                        phdr+=phdr_end-phdr;
                    }
                    header_tag=1;
                }else{
                    if(phdr+nread<phdr_end){
                        strncpy(phdr,buf,nread);
                        phdr+=nread;
                    }else{
                        strncpy(phdr,buf,phdr_end-phdr);
                        phdr+=phdr_end-phdr;
                    }
                }
            }else{
                if(pbody+nread<pbody_end){
                    strncpy(pbody,buf,nread);
                    pbody+=nread;
                }else{
                    strncpy(pbody,buf,pbody_end-pbody);
                    pbody+=pbody_end-pbody;
                }
            }
        }else if(nread==0){
            return PEER_CLOSED;
        }else if(nread==-1){
            if(errno==EINTR){
                continue;
            }else if(errno==EWOULDBLOCK||errno==EAGAIN){
                break;
            }else if(errno==ENOENT){
                break;
            }else{
                return INTERNAL;
            }
        }
    }
    *pbody='\0';
    *phdr='\0';
#ifdef DEBUG
    printf("<Header>\n%s\n</Header>\n",r->header);
    printf("<Body>\n%s\n</Body>\n",r->body);
#endif
    phdr=r->header;
    if(strncasecmp(phdr,"GET",3)==0){
        r->m=GET;
        phdr+=3;
    }else if(strncasecmp(phdr,"POST",4)==0){
        r->m=POST;
        phdr+=4;
    }else{
        fprintf(stderr,"Unsupport method!\n");
        return METHOD_NOT_SUPPORTED;
    }
    while(isspace(*phdr)){
        phdr++;
    }
    while(!isspace(*phdr)){
        *purl++=*phdr++;
    }
    *purl='\0';
    if(r->url[2]=='\0'){
        strcpy(r->url,"/index.html");
    }
    while(isspace(*phdr)){
        phdr++;
    }
    if(strncasecmp(phdr,"HTTP/1.0",8)==0){
        r->proto=HTTP_1_0;
    }else if(strncasecmp(phdr,"HTTP/1.1",8)==0){
        r->proto=HTTP_1_1;
    }else{
        return PROTO_NOT_SUPPORTED;
    }
    return NO;
}

void*
provserv(void* _c){
    Client* c=_c;
    int sfd=c->fd;
#ifdef DEBUG
    printf("socket fd=%d\n",sfd);
#endif
    Request r;
    Error e=getrequest(&r,sfd);
    if(e==NO){
        Respond res;
        memset(&res,0,sizeof(res));
        for(int i=0;exts_tab[i].name!=0;i++){
            if(strstr(r.url,exts_tab[i].name)!=NULL){
                strcpy(res.filetype,exts_tab[i].filetype);
            }
        }
        if(res.filetype[0]=='\0'){
            elog(MIME_NOT_SUPPORTED,sfd);
        }else{
            //buffer overflow? 
            strcat(c->path,r.url);
#ifdef DEBUG
            printf("path=%s\n",c->path);
            printf("filetype=%s\n",res.filetype);
#endif
            int f=open(c->path,O_RDONLY);
            if(f==-1){
                elog(NOT_FOUND,sfd);
            }else{
                struct stat filestat;
                fstat(f,&filestat);
                res.content_len=filestat.st_size;
                sprintf(res.header,"HTTP/1.1 200 OK\r\nContent-Type:%s\r\n\r\n",res.filetype);
                write(sfd,res.header,strlen(res.header));
                char buf[1024];
                int nread;
                while((nread=read(f,buf,sizeof(buf)))>0){
                    write(sfd,buf,nread);
                }
                close(f);
            }
        }
        close(sfd);
    }else if(e==PEER_CLOSED){
        elog(e,sfd);
    }else{
        elog(e,sfd);
        close(sfd);
    }
    free(c);
    return NULL;
}

void
elog(Error e,int fd){
    char ebuf[MAX_BODY_LEN];
    switch(e){
    case PEER_CLOSED:
        break;
    case INTERNAL:
        sprintf(ebuf,"HTTP/1.1 500 INTERNAL\r\n"
                "Content-Type:text/html\r\n"
                "\r\n"
                "<html><head><title>500</title></head><body><p>Server Internal Error!</p></body></html>\r\n");
        write(fd,ebuf,strlen(ebuf));
        break;
    case PROTO_NOT_SUPPORTED:
        sprintf(ebuf,"HTTP/1.1 400 Protocal\r\n"
                "Content-Type:text/html\r\n"
                "\r\n"
                "<html><head><title>400</title></head><body><p>Protocal Not Supported!</p></body></html>\r\n");
        write(fd,ebuf,strlen(ebuf));
       break;
    case METHOD_NOT_SUPPORTED:
        sprintf(ebuf,"HTTP/1.1 400 Method\r\n"
                "Content-Type:text/html\r\n"
                "\r\n"
                "<html><head><title>400</title></head><body><p>Method Not Supported!</p></body></html>\r\n");
        write(fd,ebuf,strlen(ebuf));
        break;
    case NOT_FOUND:
        sprintf(ebuf,"HTTP/1.1 404 Not Found\r\n"
                "Content-Type:text/html\r\n"
                "\r\n"
                "<html><head><title>404</title></head><body><p>Resource Not Found!</p></body></html>\r\n");
        write(fd,ebuf,strlen(ebuf));
        break;
    case MIME_NOT_SUPPORTED:
        sprintf(ebuf,"HTTP/1.1 400 MIME Not Support\r\n"
                "Content-Type:text/html\r\n"
                "\r\n"
                "<html><head><title>400</title></head><body><p>MIME Not Supported!</p></body></html>\r\n");
        write(fd,ebuf,strlen(ebuf));
        break;
       
    }
}

int
getsocket(Serv* s){
    int fd=socket(AF_INET,SOCK_STREAM,0);
    if(fd==-1){
        return -1;
    }
    int opt=1;
    if(setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt))==-1){
        goto err;
    }
    if(bind(fd,(struct sockaddr*)&s->s,sizeof(s->s))==-1){
        goto err;
    }
    if(listen(fd,s->backlog)==-1){
        goto err;
    }
    s->fd=fd;
    return 0;
err:{
        close(fd);
        return -1;
    }
}

int
getservopt(Serv* s,char* conf){
    memset(s,0,sizeof(*s));
    s->s.sin_family=AF_INET;
    s->s.sin_addr.s_addr=htonl(INADDR_ANY);
    FILE* f=fopen(conf,"r");
    if(f==NULL){
        return -1;
    }
    int backlog=0;
    int port=0;
    char* dir=NULL;
    char* p=NULL;
    char buf[1024];
    while(fgets(buf,sizeof(buf),f)!=NULL){
        buf[strlen(buf)-1]='\0';
        if((p=strstr(buf,"port"))!=NULL){
            port=atoi(p+5);
        }else if((p=strstr(buf,"directory"))!=NULL){
            dir=strdup(p+10);
        }else if((p=strstr(buf,"backlog"))!=NULL){
            backlog=atoi(p+8);
        }
        memset(buf,0,sizeof(buf));
    }
    if(port==0){
        port=DEFAULT_PORT;
    }
    if(backlog==0){
        backlog=DEFAULT_BACKLOG;
    }
    s->s.sin_port=htons(port);
    s->backlog=backlog;
    if(dir==NULL){
        dir=strdup(DEFAUTL_DIR);
    }
    s->dir=dir;
#ifdef DEBUG
    printf("dir=%s port=%d backlog=%d\n",dir,port,backlog);
#endif
    fclose(f);
    return 0;
}
