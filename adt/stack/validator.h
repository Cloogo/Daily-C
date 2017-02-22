#ifndef _VALIDATOR_H
#define _VALIDATOR_H
#include <stdbool.h>
#ifdef __cplusplus
extern "C"{
#endif

typedef struct Validator* Validator;

struct Validator{
    bool (* const validate)(Validator self,void* val);
    void* const data;
};

typedef struct Rang{
    const int min;
    const int max;
}Rang;

typedef struct Previous{
    int val;
}Previous;

extern bool int_rang(const Validator self,const void* val);
extern bool int_smaller(Validator self,const void* val);
extern bool int_greater(Validator self,const void* val);

#define Validator_new(validate,data){\
    validate,\
    data\
}

#define Validator_new_rang(rang) { \
    int_rang,\
    rang \
}

#define Validator_new_smaller(prev) { \
    int_smaller,\
    prev\
}

#define Validator_new_greater(prev) {\
    int_greater,\
    prev\
}

#ifdef __cplusplus
}
#endif

#endif
