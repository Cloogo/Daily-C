#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PI 3.1415926

int width=600;
int height=320;
int cells=100;
float xyrange=30.0;
int xyscale;
int zscale;
float angle=PI/6;

typedef struct Point{
    float x;
    float y;
}Point;

void corner(int i,int j,struct Point* point);
float f(float x,float y);

int
main(){
    printf("<svg xmlns='http://www.w3.org/2000/svg' \
           style='stroke:blue;fill:green;stroke-width:0.7' \
           width='%d' height='%d'>",width,height);
    Point a,b,c,d;
    xyscale=width/2/xyrange;
    zscale=height*0.4;
    for(int i=0;i<cells;i++){
        for(int j=0;j<cells;j++){
            corner(i+1,j,&a);
            corner(i,j,&b);
            corner(i,j+1,&c);
            corner(i+1,j+1,&d);
            printf("<polygon points='%g,%g %g,%g %g,%g %g,%g'/>\n",
                   a.x,a.y,b.x,b.y,c.x,c.y,d.x,d.y);
        }
    }
    printf("</svg>");
    exit(0);
}

void
corner(int i,int j,struct Point* point){
    //finds point(x,y) at corner of cell(i,j)
    float x=xyrange*((float)i/cells-0.5);
    float y=xyrange*((float)j/cells-0.5);
    //computes surface height z
    float z=f(x,y);
    //projects (x,y,z) isometrically onto 2-D svg canvas(point->x,point->y)
    point->x=width/2+(x-y)*cos(angle)*xyscale;
    point->y=height/2+(x+y)*sin(angle)*xyscale-z*zscale;
}

float
f(float x,float y){
    float r=hypot(x,y);
    return sin(r)/r;
}
