#include "minheap.h"
#include "maxheap.h"
#include "heapsort.h"

void
Heap_ascendingsort(int* x,int n){
    for(int i=1;i<n;i++){
        MaxHeap_siftup(x,i);
    }
    for(int i=n-1;i>=1;i--){
        MaxHeap_extractmax(x,i);
    }
}

void
Heap_descendingsort(int* x,int n){
    for(int i=1;i<n;i++){
        MinHeap_siftup(x,i);
    }
    for(int i=n-1;i>=1;i--){
        MinHeap_extractmin(x,i);
    }
}
