//
// Created by jsamp701 on 2/29/16.
//

#include "ASAPI/asapi.h"
#include <dlfcn.h>

static ARMSIM_CTX *ctx;
static void *as_lib_handle;
static ARMSIM_LOG_HOOK wrapper_logger;
static void *wrapper_logger_ctx;

// API Pointers
static ARMSIM_STATUS (*create_simulator)(ARMSIM_CTX**, void*,ARMSIM_MMIO_STORE_HOOK, ARMSIM_MMIO_LOAD_HOOK);
static ARMSIM_STATUS (*destroy_simulator)(ARMSIM_CTX*);
static ARMSIM_STATUS (*set_log_level)(ARMSIM_CTX*, ARMSIM_LOGLEVEL);
static ARMSIM_STATUS (*set_trace_level)(ARMSIM_CTX*, ARMSIM_TRACELEVEL);
static ARMSIM_STATUS (*set_log_hook)(ARMSIM_CTX*, ARMSIM_LOG_HOOK, void*);
static ARMSIM_STATUS (*set_trace_hook)(ARMSIM_CTX*, ARMSIM_LOG_HOOK, void*);
static ARMSIM_STATUS (*get_ram_word)(ARMSIM_CTX*, ARM_ADDRESS, ARM_WORD*);
static ARMSIM_STATUS (*set_ram_word)(ARMSIM_CTX*, ARM_ADDRESS, ARM_WORD);
static ARMSIM_STATUS (*get_reg)(ARMSIM_CTX*, ARM_REG, ARM_WORD*);
static ARMSIM_STATUS (*set_reg)(ARMSIM_CTX*, ARM_REG, ARM_WORD);
static ARMSIM_STATUS (*get_mode)(ARMSIM_CTX*, ARM_MODE*);
static ARMSIM_STATUS (*set_mode)(ARMSIM_CTX*, ARM_MODE);
static ARMSIM_STATUS (*get_banked_reg)(ARMSIM_CTX*, ARM_BANKED_REG, ARM_WORD*);
static ARMSIM_STATUS (*set_banked_reg)(ARMSIM_CTX*, ARM_BANKED_REG, ARM_WORD);
static ARMSIM_STATUS (*reset_cpu)(ARMSIM_CTX*);
static ARMSIM_STATUS (*enable_cache)(ARMSIM_CTX*, ARM_ADDRESS);
static ARMSIM_STATUS (*invalidate_cache)(ARMSIM_CTX*, ARM_ADDRESS);
static ARMSIM_STATUS (*notify_event)(ARMSIM_CTX*, ARMSIM_EVENT);
static ARMSIM_STATUS (*execute)(ARMSIM_CTX*, size_t, size_t*);

static enum ASAPI_MAJOR LIB_MAJOR = ASAPI_MAJOR_0;
static enum ASAPI_MINOR LIB_MINOR = ASAPI_MINOR_2;

char* initialize_library(char *lib_path, ARMSIM_LOG_HOOK log_hook, void *actx){
    char *error;
    wrapper_logger=log_hook;
    wrapper_logger_ctx = actx;
}

const char* test_function_for_loading(){
    return "HI!";
}
