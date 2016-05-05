//
// Created by joel on 5/3/16.
//

#include "JSAMP701_API/utils.h"

ARM_WORD extract_bits(ARM_WORD word, int top, int bottom){
    ARM_WORD value = word;
    int up = 31 - top;
    int down = up + bottom;
    value = value << up;
    value = value >> down;
    return value;
}

ARM_WORD lsl(ARM_WORD word, int bits){
    ARM_WORD value = word;
    value = value << bits;
    return value;
}

ARM_WORD lsr(ARM_WORD word, int bits){
    ARM_WORD value = word;
    value = value >> bits;
    return value;
}

ARM_WORD asr(ARM_WORD word, int bits){
    int value = word;
    value = value >> bits;
    return (ARM_WORD)value;
}

ARM_WORD ror(ARM_WORD word, int bits){
    ARM_WORD value = word;
    value = (value << (32 - bits)) | (value >> bits);
    return value;
}

//lambda n, z, c, v: z,                       # EQ    (0000)
//lambda n, z, c, v: not z,                   # NE    (0001)
//lambda n, z, c, v: c,                       # CS/HS (0010)
//lambda n, z, c, v: not c,                   # CC/LO (0011)
//lambda n, z, c, v: n,                       # MI    (0100)
//lambda n, z, c, v: not n,                   # PL    (0101)
//lambda n, z, c, v: v,                       # VS    (0110)
//lambda n, z, c, v: not v,                   # VC    (0111)
//lambda n, z, c, v: c and (not z),           # HI    (1000)
//lambda n, z, c, v: (not c) or z,            # LS    (1001)
//lambda n, z, c, v: n == v,                  # GE    (1010)
//lambda n, z, c, v: n != v,                  # LT    (1011)
//lambda n, z, c, v: (not z) and (n == v),    # GT    (1100)
//lambda n, z, c, v: z or (n != v),           # LE    (1101)
//lambda n, z, c, v: True,                    # AL    (1110)
//lambda n, z, c, v: False,                   # XX    (1111)

int cond_test(ARM_CONDITION condition, int flags){
    int n = (flags & 0x80000000) > 0;
    int z = (flags & 0x40000000) > 0;
    int c = (flags & 0x20000000) > 0;
    int v = (flags & 0x10000000) > 0;
    switch(condition){
        case EQ:
            return z;
        case NE:
            return !z;
        case CS:
            return c;
        case CC:
            return !c;
        case MI:
            return n;
        case PL:
            return !n;
        case VS:
            return v;
        case VC:
            return !v;
        case HI:
            return c & (!z);
        case LS:
            return (!c) | z;
        case GE:
            return n==v;
        case LT:
            return n!=v;
        case GT:
            return (!z) & (n==v);
        case LE:
            return z | (n != v);
        case AL:
            return 1;
        case XX:
            return 0;
        default:
            return 0;
    }
}
