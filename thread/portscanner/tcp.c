#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <mcheck.h>

typedef struct port_seg{
    struct in_addr destip;
    int min;
    int max;
}port_seg;

typedef struct scan_res{
    int port;
    char* s_name;
    struct scan_res* next;
}scan_res;

void 
usage(char* proname){
    printf("Usage:%s -a destination_addr -m max_port_num -n threads_num\n",proname);
    exit(1);
}

void
para_err(char* err){
    fprintf(stderr,"parameter error:%s\n",err);
    exit(1);
}

void
func_err(char* name){
    fprintf(stderr,"function call error:%s\n",name);
    exit(1);
}

void
sys_err(char* name){
    perror(name);
    exit(1);
}

void
set_nonblock(int sfd){
    int opt=fcntl(sfd,F_GETFD);
    if(opt==-1){
        sys_err("fcntl(F_GETFD)");
    }
    if(fcntl(sfd,F_SETFD,opt|O_NONBLOCK)==-1){
        sys_err("fcntl(F_SETFD)");
    }
}

void*
scan(void* data){
    port_seg* port_info=(port_seg*)data;
#ifdef DEBUG
    printf("%d-%d...\n",port_info->min,port_info->max);
#endif
    scan_res* res_list=(scan_res*)malloc(sizeof(scan_res));
    res_list->next=NULL;
    if(res_list==NULL){
        fprintf(stderr,"malloc");
    }
    scan_res* cur=res_list;
    struct sockaddr_in serv;
    memset(&serv,0,sizeof(serv));
    serv.sin_family=AF_INET;
    serv.sin_addr=port_info->destip;
    for(int i=port_info->min;i<=port_info->max;i++){
        serv.sin_port=htons(i);
        int sfd=socket(AF_INET,SOCK_STREAM,0);
        if(sfd==-1){
            perror("socket");
            continue;
        }
        set_nonblock(sfd);
        if(connect(sfd,(struct sockaddr*)&serv,sizeof(serv))==0){
            struct servent* s=getservbyport(htons(i),"tcp");
            if(s!=NULL){
                cur->port=i;
                cur->s_name=strdup(s->s_name);
                cur->next=(scan_res*)malloc(sizeof(scan_res));
                if(cur->next==NULL){
                    fprintf(stderr,"malloc");
                }
                cur=cur->next;
                cur->next=NULL;
            }
        }else{
            if(errno==EINPROGRESS){
                struct timeval tm={.tv_sec=3,.tv_usec=0};
                fd_set rs,ws;
                FD_ZERO(&rs);
                FD_ZERO(&ws);
                FD_SET(sfd,&rs);
                FD_SET(sfd,&ws);
                int error;
                int len=sizeof(error);
                if(select(sfd+1,&rs,&ws,NULL,&tm)>0){
                    if(getsockopt(sfd,SOL_SOCKET,SO_ERROR,&error,&len)==-1){
                        close(sfd);
                        sys_err("getsockopt");
                    }
                    if(error==0){
                        struct servent* s=getservbyport(htons(i),"tcp");
                        if(s!=NULL){
                            cur->port=i;
                            cur->s_name=strdup(s->s_name);
                            cur->next=(scan_res*)malloc(sizeof(scan_res));
                            if(cur->next==NULL){
                                fprintf(stderr,"malloc");
                            }
                            cur=cur->next;
                            cur->next=NULL;
                        }
                    }
                }
            }
        }
        close(sfd);
    }
    pthread_exit(res_list);
    return NULL;
}

int
main(int argc,char* argv[]){
    if(argc!=9){
        usage(argv[0]);
    }
    pthread_t* threads;
    int nthreads;
    struct in_addr destip;
    int min_port,max_port;
    int nports=0;
    int opt;
    while((opt=getopt(argc,argv,"a:m:n:s:"))!=-1){
        switch(opt){
        case 'a':
            if(inet_aton(optarg,&destip)==0){
                para_err("destination ip address invalid!");
            }
            break;
        case 'm':
            max_port=atoi(optarg);
            if(max_port<0||max_port>65535){
                para_err("max_port_num<0||max_port_num>65535!");
            }
            break;
        case 's':
            min_port=atoi(optarg);
            if(min_port<=0||min_port>65535){
                para_err("min_port<=0||min_port>65535!");
            }
            break;
        case 'n':
            nthreads=atoi(optarg);
            if(nthreads<=0){
                para_err("threads_num<=0!");
            }
            break;
        default:
            usage(argv[0]);
        }
    }
    if(min_port>max_port){
        para_err("min_port>max_port!");
    }
    nports=max_port-min_port+1;
    if(nports<nthreads){
        nthreads=nports;
    }
    int port_num_per_seg=nports/nthreads;
    if(nports%nthreads!=0){
        nthreads++;
    }
    setenv("MALLOC_TRACE","mem.log",1);
    mtrace();
    threads=(pthread_t*)malloc(sizeof(pthread_t)*nthreads);
    if(threads==NULL){
        func_err("malloc");
    }
    printf("scanning ports from %d to %d on %s...\n\n",min_port,max_port,inet_ntoa(destip));
    port_seg ports_info[nthreads];
    for(int i=0;i<nthreads;i++){
        ports_info[i].destip=destip;
        if(i==0){
            ports_info[i].min=min_port;
        }else{
            ports_info[i].min=min_port+i*port_num_per_seg-1;
        }
        if(i==nthreads-1){
            ports_info[i].max=max_port;
        }else{
            ports_info[i].max=(i+1)*port_num_per_seg-1;
        }
        int ret;
        if((ret=pthread_create(&threads[i],NULL,scan,(void*)&ports_info[i]))!=0){
            fprintf(stderr,"pthread_create:%s",strerror(ret));
            continue;
        }
    }
    int valid_ports=0;
    for(int i=0;i<nthreads;i++){
        scan_res* reslist=NULL;
        scan_res* cur=NULL;
        pthread_join(threads[i],(void**)&reslist);
        while(reslist->next!=NULL){
            cur=reslist;
            reslist=reslist->next;
            printf("%d %s\n",cur->port,cur->s_name);
            free(cur->s_name);
            free(cur);
            valid_ports++;
        }
        free(reslist);
    }
    free(threads);
    printf("\n%d ports scanned.\n",nports);
    if(valid_ports==0){
        printf("No server application available.\n");
    }else{
        printf("%d ports available.\n",valid_ports);
    }
    return 0;
}
