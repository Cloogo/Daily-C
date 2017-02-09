#include <stdio.h>
#include <stdlib.h>

typedef struct IP_HDR{
    int version:4;
    int hdr_len:4;
    int tos:8;
    int total_len:16;
    int identification:16;
    int fragmentation:16;
    int ttl:8;
    int port:8;
    int chk_sum:16;
    int src_ip;
    int dst_ip;
}IP_HDR;

unsigned short checksum(IP_HDR hdr){
    unsigned long sum;
    unsigned short *p=(unsigned short*)&hdr;
    unsigned short *h=p;
    int n=sizeof(hdr)/sizeof(unsigned short);
    for(sum=0;p<h+n;p++){
        sum+=*p;
    }
    sum=(sum>>16)+(sum&0xffff);
    sum+=(sum>>16);
    return ~sum;
}

int main(){
    IP_HDR hdr;
    hdr.chk_sum=0;
    unsigned short cks=checksum(hdr);
    hdr.chk_sum=cks;
    cks=checksum(hdr);
    if(cks!=0){
        printf("Ip datagram broken!\n");
        printf("Throw away ip datagram!\n");
    }else{
        printf("Ip datagram is correct.\n");
    }
    hdr.port=80;
    cks=checksum(hdr);
    if(cks!=0){
        printf("Ip datagram broken!\n");
        printf("Throw away ip datagram!\n");
    }else{
        printf("Ip datagram is correct.\n");
    }
    return 0;
}
