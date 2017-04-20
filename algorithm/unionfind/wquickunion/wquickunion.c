#include <stdio.h>
#include "wquickunion.h"

void
UF_init(UF* uf,int nmembs){
    for(int i=0;i<nmembs;i++){
        uf->grpid[i]=i;
        uf->gsz[i]=1;
    }
    uf->nmembs=nmembs;
    uf->ngrps=nmembs;
}

int
UF_find(UF* uf,int p){
    while(uf->grpid[p]!=p){
        p=uf->grpid[p];
    }
    return p;
}

void
UF_union(UF* uf,int p,int q){
    int pgid=UF_find(uf,p);
    int qgid=UF_find(uf,q);
    if(pgid==qgid){
        return;
    }
    if(uf->gsz[pgid]<uf->gsz[qgid]){
        uf->grpid[pgid]=qgid;
        uf->gsz[qgid]+=uf->gsz[pgid];
    }else{
        uf->grpid[qgid]=pgid;
        uf->gsz[pgid]+=uf->gsz[qgid];
    }
    uf->ngrps--;
}

int
UF_connected(UF* uf,int p,int q){
    return UF_find(uf,p)==UF_find(uf,q);
}

int
UF_grpsn(UF* uf){
    return uf->ngrps;
}

