//
// Created by jsamp701 on 3/2/16.
//

#include "ASAPI/asapi.h"

#ifndef AS_DI_ARMSIM_CTX_DEF_H
#define AS_DI_ARMSIM_CTX_DEF_H

struct ARMSIM_CTX {
    //put necessary instance variables here
    void *hook_ctx;
    ARMSIM_MMIO_LOAD_HOOK load_hook;
    ARMSIM_MMIO_STORE_HOOK store_hook;
    ARMSIM_LOG_HOOK log_hook;
    ARMSIM_LOG_HOOK trace_hook;
    enum ARMSIM_LOGLEVEL loglevel;
    enum ARMSIM_TRACELEVEL tracelevel;
    int sanity_test;
    int log_hook_defined;
    int trace_hook_defined;
};
#endif //AS_DI_ARMSIM_CTX_DEF_H
