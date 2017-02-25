#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "test.h"
#include "../heapsort.h"

int
main(){
    srand(time(NULL));
    for(int j=10000;j<12000;j++){
        int n=j;
        int x[n];
        x[0]=-10000000;
        for(int i=1;i<n;i++){
            x[i]=rand()%1000000;
        }
        Heap_descendingsort(x,n);
        for(int i=1;i<n-1;i++){
            test_conds(x[i]>=x[i+1]);
        }
        conds_report("%s","Heap_descendingsort");
    }
    for(int j=1;j<1000;j++){
        int n=j;
        int x[n];
        x[0]=10000000;
        for(int i=1;i<n;i++){
            x[i]=rand()%100000;
        }
        Heap_ascendingsort(x,n);
        for(int i=1;i<n-1;i++){
            test_conds(x[i]<=x[i+1]);
        }
        conds_report("%s","Heap_ascendingsort");
    }
    test_report();
    exit(0);
}
