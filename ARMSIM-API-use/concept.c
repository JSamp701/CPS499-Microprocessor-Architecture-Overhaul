/* Bare-bones implementation of the ArmSim API defined in asapi.h */

// Import the API definitions
#include <asapi.h>

// Report the "current" version defined in asapi.h
DEFINE_DEFAULT_ASAPI_VERSION_FUNCTIONS

// Internal/private data stored in an ArmSim context structure
struct ARMSIM_CTX {
    // Active register bank (for r0-r15 of current mode)
    ARM_WORD    regs[AR_COUNT];
    
    // CPSR register (we are a downlevel simulator
    // that doesn't implement all the banked registers,
    // so we need to handle this one separately)...
    ARM_WORD    cpsr;
    
    // Current operating mode
    ARM_MODE    mode;
    
    // Simulated RAM (size and pointer to storage)
    size_t      ram_size;
    ARM_BYTE    *ram_data;
    
    // Logging levels, hook function pointers,
    // and associated "user context" pointers
    ARMSIM_LOGLEVEL     debug_log_level;
    ARMSIM_LOG_HOOK     debug_log_hook;
    void                *debug_log_uctx;
    
    ARMSIM_TRACELEVEL   trace_log_level;
    ARMSIM_LOG_HOOK     trace_log_hook;
    void                *trace_log_uctx;
    
    // Memory-mapped I/O hook function pointers with "user context"
    ARMSIM_MMIO_LOAD_HOOK   mmio_load_hook;
    ARMSIM_MMIO_STORE_HOOK  mmio_store_hook;
    void                    *mmio_uctx;
};


// All error-checking macros assume a local ARMSIM_STATUS variable named "status"
// and a local label named "cleanup".
#define ENSURE_EQ(Expr, Val, Status) if ((Expr) != (Val)) { status = (Status); goto cleanup; }
#define ENSURE_NE(Expr, Val, Status) ENSURE_EQ((Expr), !(Val), Status)
#define ENSURE_NOTNULL(Ptr, Status) ENSURE_NE(Ptr, NULL, Status)
#define CHAIN_ENSURE(Expr) if((status = (Expr)) != AS_OK) { goto cleanup; }


ARMSIM_STATUS as_create_simulator(
    ARMSIM_CTX **pctx,
    size_t ram_size)
{
    ARMSIM_STATUS status = AS_MAX;
    
    // Check parameters
    ENSURE_NOTNULL(pctx, AS_NULL_PTR);
    *pctx = NULL;
    ENSURE_NE(ram_size, 0, AS_BAD_MALLOC);
    
    // Allocate room for structure AND associated RAM (one block)
    ARMSIM_CTX *obj = malloc(sizeof(ARMSIM_CTX) + ram_size);
    ENSURE_NOTNULL(obj, AS_BAD_MALLOC);
    
    // Link the RAM and the structure
    obj->ram_size = ram_size;
    obj->ram_data = (ARM_BYTE *)&obj[1];    // I.e., immediately following the structure in the allocated block
    
    // Initialize CPU state and clear out RAM
    CHAIN_ENSURE(as_reset_cpu(obj));
    CHAIN_ENSURE(as_clear_ram(obj));
    
    // TODO: establish default handlers for debug logging,
    // trace logging, and MMIO loads/stores
    
    // Everything is complete--transfer ownership to user's pointer
    *pctx = obj;
    obj = NULL;
    
    status = AS_OK;
cleanup:
    if (obj) { free(obj); } // If ownership was not handed off, free the partially-constructed object
    return status;
}

ARMSIM_STATUS as_destroy_simulator(
    ARMSIM_CTX *ctx)
{
    ARMSIM_STATUS status = AS_MAX;
    
    // Validate parameter
    ENSURE_NOTNULL(ctx, AS_NULL_PTR);
    
    // Free memory (both the structure and the RAM array were allocated in one block)
    free(ctx);
    
    status = AS_OK;
cleanup:
    return status;
}

ARMSIM_STATUS as_reset_cpu(
    ARMSIM_CTX *ctx)
{
    ARMSIM_STATUS status = AS_MAX;
    
    // Validate parameter
    ENSURE_NOTNULL(ctx, AS_NULL_PTR);
    
    // Clear all registers
    for (int i = 0; i < AR_COUNT; ++i) {
        ctx->regs[i] = 0;
    }
    ctx->cpsr = 0;
    ctx->mode = AM_SYS;
    
    status = AS_OK;
cleanup:
    return status;
}

ARMSIM_STATUS as_clear_ram(
    ARMSIM_CTX *ctx)
{
    ARMSIM_STATUS status = AS_MAX;
    
    ENSURE_NOTNULL(ctx, AS_NULL_PTR);
    
    memset(ctx->ram_data, 0, ctx->ramsize);
    
    status = AS_OK;
cleanup:
    return status;
}

ARMSIM_STATUS as_load_ram(
    ARMSIM_CTX *ctx,
    ARM_ADDRESS start,
    void *data,
    size_t size)
{
    return AS_NOT_IMPLEMENTED;
}

ARMSIM_STATUS as_dump_ram(
    ARMSIM_CTX *ctx,
    ARM_ADDRESS start,
    void *buff,
    size_t space)
{
    return AS_NOT_IMPLEMENTED;
}

ARMSIM_STATUS as_set_loglevel(
    ARMSIM_CTX *ctx,
    ARMSIM_LOGLEVEL level)
{
    return AS_NOT_IMPLEMENTED;
}

ARMSIM_STATUS as_set_debug_log_hook(
    ARMSIM_CTX *ctx,
    ARMSIM_LOG_HOOK hook,
    void *uctx)
{
    return AS_NOT_IMPLEMENTED;
}

ARMSIM_STATUS as_set_tracelevel(
    ARMSIM_CTX *ctx,
    ARMSIM_TRACELEVEL level)
{
    return AS_NOT_IMPLEMENTED;
}

ARMSIM_STATUS as_set_trace_log_hook(
    ARMSIM_CTX *ctx,
    ARMSIM_LOG_HOOK hook,
    void *uctx)
{
    return AS_NOT_IMPLEMENTED;
}

ARMSIM_STATUS as_set_mmio_hooks(
    ARMSIM_CTX *ctx,
    ARMSIM_MMIO_LOAD_HOOK load,
    ARMSIM_MMIO_STORE_HOOK store,
    void *uctx)
{
    return AS_NOT_IMPLEMENTED;
}

ARMSIM_STATUS as_get_ram_word(
    ARMSIM_CTX *ctx,
    ARM_ADDRESS address,
    ARM_WORD *pval)
{
    return AS_NOT_IMPLEMENTED;
}

ARMSIM_STATUS as_set_ram_word(
    ARMSIM_CTX *ctx,
    ARM_ADDRESS address,
    ARM_WORD value)
{
    return AS_NOT_IMPLEMENTED;
}

ARMSIM_STATUS as_get_reg(
    ARMSIM_CTX *ctx,
    ARM_REG reg,
    ARM_WORD *pval)
{
    return AS_NOT_IMPLEMENTED;
}

ARMSIM_STATUS as_set_reg(
    ARMSIM_CTX *ctx,
    ARM_REG reg,
    ARM_WORD value)
{
    return AS_NOT_IMPLEMENTED;
}

ARMSIM_STATUS as_get_mode(
    ARMSIM_CTX *ctx,
    ARM_MODE *pmd)
{
    return AS_NOT_IMPLEMENTED;
}

ARMSIM_STATUS as_set_mode(
    ARMSIM_CTX *ctx,
    ARM_MODE mode)
{
    return AS_NOT_IMPLEMENTED;
}

ARMSIM_STATUS as_get_banked_reg(
    ARMSIM_CTX *ctx,
    ARM_BANKED_REG reg,
    ARM_WORD *pval)
{
    return AS_NOT_IMPLEMENTED;
}
ARMSIM_STATUS as_set_banked_reg(
    ARMSIM_CTX *ctx,
    ARM_BANKED_REG reg,
    ARM_WORD value)
{
    return AS_NOT_IMPLEMENTED;
}

ARMSIM_STATUS as_execute(
    ARMSIM_CTX *ctx,
    size_t cycles,
    size_t *finished)
{
    return AS_NOT_IMPLEMENTED;
}
