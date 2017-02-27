#include <stdio.h>
#include <string.h>
#include "test.h"
#include "../maxcomsubstr.h"

#define T(arg,res) test_cond(strcmp(maxcomsubstr(arg),res)==0,"")
int
main(){
    T("banana","ana");
    T("Helloworld Hello","Hello");
    T("hello","l");
    T("heo","");
    T("aaaaaaa","aaaaaa");
    T("hello world hello again world hello"," world hello");
    T("Ask not what your country can do for you,but what you can do for your country"," can do for you");
    test_report();
    return 0;
}
