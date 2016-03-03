//
// Created by jsamp701 on 3/2/16.
//

#include "ASAPI/asapi.h"

#ifndef AS_DI_ARMSIM_CTX_DEF_H
#define AS_DI_ARMSIM_CTX_DEF_H

struct ARMSIM_CTX {
    //put necessary instance variables here
    int foo;
    void *hook_ctx;
    ARMSIM_MMIO_LOAD_HOOK load_hook;
    ARMSIM_MMIO_STORE_HOOK store_hook;
    ARMSIM_LOG_HOOK log_hook;
    ARMSIM_LOG_HOOK trace_hook;
    enum ARMSIM_LOGLEVEL loglevel;
    enum ARMSIM_TRACELEVEL tracelevel;
};

//i suggest you define a pseudo-constructor / destructor in this file as well
ARMSIM_STATUS create_context(ARMSIM_CTX* ctx){
    return AS_NOT_IMPLEMENTED;
}

ARMSIM_STATUS delete_context(ARMSIM_CTX* ctx){
    return AS_NOT_IMPLEMENTED;
}
#endif //AS_DI_ARMSIM_CTX_DEF_H