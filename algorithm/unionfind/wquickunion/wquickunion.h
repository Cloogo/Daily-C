#ifndef _QUICKUNION_H
#define _QUICKUNION_H
#define MAX_MEM_N 1024
typedef struct Unionfind{
    int grpid[MAX_MEM_N];
    int gsz[MAX_MEM_N];
    int nmembs;
    int ngrps;
}UF;
extern void UF_init(UF* uf,int nmembs);
extern int UF_find(UF* uf,int p);
extern void UF_union(UF* uf,int p,int q);
extern int UF_connected(UF* uf,int p,int q);
extern int UF_grpsn(UF* uf);
#endif
