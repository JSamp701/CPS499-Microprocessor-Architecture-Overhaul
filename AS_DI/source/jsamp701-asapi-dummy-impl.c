//
// Created by jsamp701 on 3/2/16.
//

#include <string.h>
#include <stdlib.h>

#include "ASAPI/asapi.h"
#include "armsim-ctx.h"

// Begin defining the various API calls
ARMSIM_STATUS as_create_simulator(
        ARMSIM_CTX **pctx,
        void *hook_ctx,
        ARMSIM_MMIO_LOAD_HOOK load_hook,
        ARMSIM_MMIO_STORE_HOOK store_hook){
    return AS_NOT_IMPLEMENTED;
}

ARMSIM_STATUS as_destroy_simulator(ARMSIM_CTX *ctx){
    return AS_NOT_IMPLEMENTED;
}

ARMSIM_STATUS as_set_loglevel(ARMSIM_CTX *ctx, ARMSIM_LOGLEVEL level){
    return AS_NOT_IMPLEMENTED;
}

ARMSIM_STATUS as_set_debug_log_hook(ARMSIM_CTX *ctx, ARMSIM_LOG_HOOK hook, void *uctx) {
    return AS_NOT_IMPLEMENTED;
}

ARMSIM_STATUS as_set_trace_log_hook(ARMSIM_CTX *ctx, ARMSIM_LOG_HOOK hook, void *uctx) {
    return AS_NOT_IMPLEMENTED;
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
