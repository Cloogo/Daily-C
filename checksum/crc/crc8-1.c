#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>

#define GP 0xD5 //g(x)=x^8+x^7+x^6+x^4+x^2+1;

uint8_t crc8(const char* ctx);

int
main(){
    assert(crc8("01234")==0x7a);
    return 0;
}

uint8_t
crc8(const char* ctx){
    uint8_t crc_8=0;
    uint8_t p=0;
    crc_8|=*ctx;
    for(uint64_t nbits=8;ctx[nbits/8]!='\0';nbits++){
        if(crc_8&0x80){
            p=GP;
        }else{
            p=0;
        }
        crc_8<<=1;
        crc_8|=((ctx[nbits/8]>>(7-nbits%8))&1);
        crc_8^=p;
    }
    for(int i=0;i<8;i++){
        if(crc_8&0x80){
            p=GP;
        }else{
            p=0;
        }
        crc_8<<=1;
        crc_8|=0;
        crc_8^=p;
    }
    return crc_8;
}
