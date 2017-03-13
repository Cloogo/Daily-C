#include <stdio.h>
#include <stdlib.h>

typedef double Celsius;
typedef double Fahrenheit;
typedef char* String;

const Celsius ABSOLUTE_ZERO_C=-273.15;
const Celsius FREESING_C=0;
const Celsius BOILING_C=100;

Fahrenheit ctof(const Celsius c);
Celsius ftoc(const Fahrenheit f);
String stringof_c(const Celsius c);
String stringof_f(const Fahrenheit f);

int main(){
    Celsius c=ABSOLUTE_ZERO_C;
    Fahrenheit f=ctof(c);
    printf("ctof(%f)=%f\n",ABSOLUTE_ZERO_C,f);
    printf("%s\n",stringof_c(ABSOLUTE_ZERO_C));
    printf("%s\n",stringof_f(ctof(ABSOLUTE_ZERO_C)));
    exit(0);
}

Fahrenheit 
ctof(const Celsius c){
    return c*9/5+32;
}

Celsius
ftoc(const Fahrenheit f){
    return (f-32)*5/9;
}

String
stringof_c(const Celsius c){
    static char buf[1024];
    snprintf(buf,1024,"Celsius:%f ℃ ",c);
    return buf;
}

String
stringof_f(const Fahrenheit f){
    static char buf[1024];
    snprintf(buf,1024,"Fahrenheit:%f ℉ ",f);
    return buf;
}
