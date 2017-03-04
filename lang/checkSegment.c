#include	<stdio.h>
#include	<stdlib.h>

int glvi_x=1,glvi_y=1;//(数据段)data segment:初始化的全局变量和静态变量
int glvi_a,glvi_b;//bbs segment:未初始化数据
int main(){
	static int lvi_z;//bbs segment
	int lvi_m;//局部变量不储存于可执行文件
	//(文本段)text segment:可执行指令等
	int *lvpi_i=(int *)malloc(sizeof(int));
	/*
	 * 初始化的局部变量储存在文本段
	 */
	int lvi_o=1;
	int lvi_ie=100;
    free(lvpi_i);
	exit(0);
}
