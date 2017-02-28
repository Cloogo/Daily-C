#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#define GP 0xD5 //g(x)=x^8+x^7+x^6+x^4+x^2+1

uint8_t crc8_tab[16];
void init_tab();
uint8_t crc8(const unsigned char* ctx);

int
main(int argc,char* argv[]){
    init_tab();
    assert(crc8("01234")==0x7a);
    return 0;
}

void
init_tab(){
    for(int i=0;i<16;i++){
        crc8_tab[i]=0;
        int s=i<<4;
        int poly;
        for(int j=3;j>=0;j--){
            crc8_tab[i]^=((s&0x80)?(GP<<j|0):0);
            if(s&0x80){
                poly=GP;
            }else{
                poly=0;
            }
            s<<=1;
            s^=poly;
        }
//        printf("crc8_tab[%d]=%02x\n",i,crc8_tab[i]);
    }
}

uint8_t
crc8(const unsigned char* ctx){
    uint8_t crc_8=*ctx;
    uint8_t poly=0;
    int i=1;
    do{
        poly=crc8_tab[(crc_8&0xf0)>>4];
        crc_8=(crc_8<<4|0)|((ctx[i]&0xf0)>>4);
        crc_8^=poly;
        poly=crc8_tab[(crc_8&0xf0)>>4];
        crc_8=(crc_8<<4|0)|(ctx[i]&0x0f);
        crc_8^=poly;
    }while(ctx[i]!='\0'&&i++);
    return crc_8;
}
