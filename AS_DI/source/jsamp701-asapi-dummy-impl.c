//
// Created by jsamp701 on 3/2/16.
//

#include <string.h>
#include <stdlib.h>

#include "ASAPI/asapi.h"
#include "armsim-ctx.h"

#define SANITY_NUMBER 123456789

// FORWARD DECLARATIONS OF CUSTOM FUNCTIONS
void as_log(ARMSIM_CTX *ctx, const char* message, int level);
void as_trace(ARMSIM_CTX *ctx, const char* message, int level);

// DEFINITION OF LIRARY FUNCTIONS

enum ASAPI_MAJOR asapi_major_version(){
    return ASAPI_MAJOR_CURRENT;
}
enum ASAPI_MINOR asapi_minor_version(){
    return ASAPI_MINOR_CURRENT;
}

// Begin defining the various API calls
ARMSIM_STATUS as_create_simulator(
        ARMSIM_CTX **pctx,
        void *hook_ctx,
        ARMSIM_MMIO_LOAD_HOOK load_hook,
        ARMSIM_MMIO_STORE_HOOK store_hook){
    ARMSIM_CTX *actx = (ARMSIM_CTX*)malloc(sizeof(ARMSIM_CTX));
    (*pctx) = actx;
    actx->hook_ctx = hook_ctx;
    actx->load_hook = load_hook;
    actx->store_hook = store_hook;
    actx->sanity_test = SANITY_NUMBER;
    actx->loglevel = AL_DEFAULT;
    actx->tracelevel = AT_DEFAULT;
    actx->log_hook_defined = 0;
    actx->trace_hook_defined = 0;
    return AS_OK;
}

ARMSIM_STATUS as_destroy_simulator(ARMSIM_CTX *ctx){
    return AS_NOT_IMPLEMENTED;
}

ARMSIM_STATUS as_set_loglevel(ARMSIM_CTX *ctx, ARMSIM_LOGLEVEL level){
    if(ctx == 0 || ctx->sanity_test != SANITY_NUMBER){
        return AS_NULL_PTR;
    }
    ctx->loglevel = level;
    return AS_OK;
}

ARMSIM_STATUS as_set_tracelevel(ARMSIM_CTX *ctx, ARMSIM_TRACELEVEL level){
    if(ctx == 0 || ctx->sanity_test != SANITY_NUMBER){
        return AS_NULL_PTR;
    }
    ctx->tracelevel = level;
    return AS_OK;
}

ARMSIM_STATUS as_set_debug_log_hook(ARMSIM_CTX *ctx, ARMSIM_LOG_HOOK hook, void *uctx) {
    if(ctx == 0 || ctx->sanity_test != SANITY_NUMBER){
        return AS_NULL_PTR;
    }
    ctx->log_hook = hook;
    ctx->log_hook_defined = 1;
    //test it
    as_log(ctx, "INSIDE AS_SET_DEBUG_LOG_HOOK", AL_DEFAULT);
    return AS_OK;
}

ARMSIM_STATUS as_set_trace_log_hook(ARMSIM_CTX *ctx, ARMSIM_LOG_HOOK hook, void *uctx) {
    if(ctx == 0 || ctx->sanity_test != SANITY_NUMBER){
        return AS_NULL_PTR;
    }
    ctx->trace_hook = hook;
    ctx->trace_hook_defined = 1;
    as_trace(ctx, "INSIDE AS_TRACE_LOG_HOOK", AT_DEFAULT);
    return AS_OK;
}

ARMSIM_STATUS as_get_ram_word(ARMSIM_CTX *ctx, ARM_ADDRESS address, ARM_WORD *pval) {
    return AS_NOT_IMPLEMENTED;
}

ARMSIM_STATUS as_set_ram_word(ARMSIM_CTX *ctx, ARM_ADDRESS address, ARM_WORD value) {
    return AS_NOT_IMPLEMENTED;
}

ARMSIM_STATUS as_get_reg(ARMSIM_CTX *ctx, ARM_REG reg, ARM_WORD *pval) {
    return AS_NOT_IMPLEMENTED;
}

ARMSIM_STATUS as_set_reg(ARMSIM_CTX *ctx, ARM_REG reg, ARM_WORD value) {
    return AS_NOT_IMPLEMENTED;
}

ARMSIM_STATUS as_get_mode(ARMSIM_CTX *ctx, ARM_MODE *pmd) {
    return AS_NOT_IMPLEMENTED;
}

ARMSIM_STATUS as_set_mode(ARMSIM_CTX *ctx, ARM_MODE mode) {
    return AS_NOT_IMPLEMENTED;
}

ARMSIM_STATUS as_get_banked_reg(ARMSIM_CTX *ctx, ARM_BANKED_REG reg, ARM_WORD *pval) {
    return AS_NOT_IMPLEMENTED;
}

ARMSIM_STATUS as_set_banked_reg(ARMSIM_CTX *ctx, ARM_BANKED_REG reg, ARM_WORD value) {
    return AS_NOT_IMPLEMENTED;
}

ARMSIM_STATUS as_reset_cpu(ARMSIM_CTX *ctx) {
    return AS_NOT_IMPLEMENTED;
}

ARMSIM_STATUS as_cache_enable(ARMSIM_CTX *ctx, ARM_ADDRESS cache_limit) {
    return AS_NOT_IMPLEMENTED;
}

ARMSIM_STATUS as_cache_invalidate(ARMSIM_CTX *ctx, ARM_ADDRESS address) {
    return AS_NOT_IMPLEMENTED;
}

ARMSIM_STATUS as_notify_event(ARMSIM_CTX *ctx, ARMSIM_EVENT event) {
    return AS_NOT_IMPLEMENTED;
}

ARMSIM_STATUS as_execute(ARMSIM_CTX *ctx, size_t cycles, size_t *finished) {
    return AS_NOT_IMPLEMENTED;
}

// CUSTOM FUNCTIONS

void as_trace(ARMSIM_CTX* ctx, const char* message, int level ){
    if(ctx->trace_hook_defined != 0)
    {
        ctx->trace_hook(ctx->hook_ctx, message);
    }
}

void as_log(ARMSIM_CTX* ctx, const char* message, int level ){
    if(ctx->log_hook_defined != 0)
    {
        ctx->log_hook(ctx->hook_ctx, message);
    }
}
