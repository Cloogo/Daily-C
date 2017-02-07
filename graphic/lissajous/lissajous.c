#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#define PI 3.14159
#define CANVAS_W 128
#define CANVAS_H 40
#define CYCLE 5//number of complete x oscillator revolution
#define ANGULAR_RESOLUTION 0.001
#define NFRAMES 128
#define MS_DELAY_PER_FRAME 80

int
main(){
    char pic[CANVAS_H+1][CANVAS_W+1];
    float phase=0.0;//difference between adjacent frames.
    int x,y;
    srand(time(NULL));
    float freq=drand48()*3.0;//relative frequency of y oscillator
    int color=32;
    for(int k=0;;k++){
        system("clear");
        for(y=0;y<CANVAS_H;y++){
            memset(pic[y],' ',CANVAS_W);
        }
        for(float t=0.0;t<2*PI*CYCLE;t+=ANGULAR_RESOLUTION){
            y=CANVAS_H/2+(int)(CANVAS_H/2*sin(t)+0.5);
            x=CANVAS_W/2+(int)(CANVAS_W/2*sin(t*freq+phase)+0.5);
            pic[y][x]='*';
        }
        for(y=0;y<CANVAS_H;y++){
            for(x=0;x<CANVAS_W;x++){
                if(pic[y][x]=='*'){
                    printf("\e[0;%dm",color);
                    printf("%c",pic[y][x]);
                    printf("\e[0m");
                }else{
                    printf("%c",pic[y][x]);
                }
            }
            printf("\n");
        }
        printf("frame:%d freq:%f phase:%f color:%d\n",k,freq,phase,color);
        usleep(MS_DELAY_PER_FRAME*1000);
        phase+=0.1;
        if(k==NFRAMES){
            k=0;
            srand(time(NULL));
            freq=drand48()*3.0;
            color=32+rand()%15;
            phase=0;
        }
    }
    return 0;
}
