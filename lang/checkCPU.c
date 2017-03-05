#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>

struct checkData{
	short a;
	int b;
	double c;
};

static int checkCPU(){//static限制函数的作用域，缺省情况(extern)下外部可见(external object)
	union checkData{
		int a;
		char b;
	}test;
	test.a=1;
	return (test.b==1);
}

int main(){
	struct checkData test={
	};
	int a=0x00010001;
	memcpy(&test,&a,sizeof(&a));
	if(test.a>1){
		printf("big endian.\n");//操作数存放方式：从高字节到低字节　low->high:0x00,0x01,0x00,0x01
	}else printf("little endian.\n");//操作数存放方式：从低字节到高字节　low->high:0x01,0x00,0x01,0x00
	//printf("%d\t%d\t%f\n",test.a,test.b,test.c);
	if(checkCPU){
		printf("little endian.\n");
	}else printf("big endian.\n");
	exit(0);
}
