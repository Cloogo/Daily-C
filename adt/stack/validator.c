#include <stdio.h>
#include "validator.h"

bool
int_rang(const Validator self,const void* val){
    int* lvi_val=(int*)val;
    Rang* rang=(Rang*)self->data;
    return rang->min<=*lvi_val&&*lvi_val<=rang->max;
}

bool
int_greater(Validator self,const void* val){
    int* lvi_val=(int*)val;
    Previous* prev=(Previous*)self->data;
    if(*lvi_val>=prev->val){
        prev->val=*lvi_val;
        return true;
    }
    return false;
}

bool
int_smaller(Validator self,const void* val){
    int* lvi_val=(int*)val;
    Previous* prev=(Previous*)self->data;
    if(*lvi_val<=prev->val){
        prev->val=*lvi_val;
        return true;
    }
    return false;
}
