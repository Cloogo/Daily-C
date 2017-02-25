#include "num.h"
#include "minheap.h"

//x[1..last-1]:heap
void
MinHeap_siftup(int* x,int last){
    int t=x[last];
    int i;
    for(i=last;x[i/2]>t;i/=2){
        x[i]=x[i/2];
    }
    x[i]=t;
}

//x[2..last]:heap
void
MinHeap_siftdown(int* x,int last){
    int t=x[1];
    int c=1;
    for(int p=1;p*2<=last;p=c){
        c=p*2;
        if(c+1<=last&&x[c+1]<x[c]){
            c++;
        }
        if(x[c]>=t){
            break;
        }
        x[p]=x[c];
    }
    x[c]=t;
}

int
MinHeap_extractmin(int* x,int n){
    if(n<=1){
        return x[1];
    }
    int min=x[1];
    swap(x[1],x[n]);
    MinHeap_siftdown(x,n-1);
    return min;
}
