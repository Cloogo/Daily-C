#include	<stdio.h>
#include    <limits.h>
#include	<string.h>
#define BITNSLOTS(nb) (((nb)+CHAR_BIT-1)/CHAR_BIT)
#define BITSLOT(b) (b/CHAR_BIT)
#define BITMASK(b) (1<<b%CHAR_BIT)
#define BITSET(a,b) ((a)[BITSLOT(b)]|=BITMASK(b))
#define BITCLEAR(a,b) ((a)[BITSLOT(b)]&=~BITMASK(b))
#define BITTEST(a,b) ((a)[BITSLOT(b)]&BITMASK(b))
#define MAX 46237000
char mark[BITNSLOTS(MAX)];

//print prime numbers between 0 to 46237000
int
main(){
	memset(mark,0,sizeof(mark));
	for(int i=2;i<MAX;i++){
		if(!BITTEST(mark,i)){
			printf("%d\n",i);
			for(int j=i*2;j<MAX;j+=i){
				BITSET(mark,j);
			}
		}
	}
	return 0;
}
