//
// Created by joel on 5/3/16.
//



#ifndef AS_RI_UTILS_H
#define AS_RI_UTILS_H

#include "../ASAPI/asapi.h"

typedef enum ARM_CONDITION {
    EQ,
    NE,
    CS,
    HS=CS,
    CC,
    LO=CC,
    MI,
    PL,
    VS,
    VC,
    HI,
    LS,
    GE,
    LT,
    GT,
    LE,
    AL,
    XX
} ARM_CONDITION;

ARM_WORD extract_bits(ARM_WORD word, int top, int bottom);

ARM_WORD lsl(ARM_WORD word, int bits);

ARM_WORD lsr(ARM_WORD word, int bits);

ARM_WORD asr(ARM_WORD word, int bits);

ARM_WORD ror(ARM_WORD word, int bits);

int cond_test(ARM_CONDITION condition, int flags);

#endif //AS_RI_UTILS_H
