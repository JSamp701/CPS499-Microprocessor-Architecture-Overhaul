#ifndef ARMSIM_API_H
#define ARMSIM_API_H

/* ArmSim API Definition Header
 * 
 * The ARM Simulator ("ArmSim") API is a software contract between
 * student simulator projects ("simulators") and various testing,
 * debugging, and interaction tools ("tools" or "hosts").  Simulators are
 * built as libraries implementing a specific version of the API
 * and are "hosted" by various tools to visualize CPU state,
 * verify proper CPU behavior on official test programs, etc.
 *
 * (c) 2016, Bob Jones University
 */

#include <stddef.h>     // NULL, etc.
#include <stdint.h>     // uint32_t, etc.

// API Versioning
//---------------------------
// Follows a variant of semantic versioning:
//  * Major number: changes only when the API undergoes a _breaking_ change
//      (i.e., v2.0 tools won't work with v1.9 simulators, and v1.9 tools won't work with v2.0 simulators)
//      (examples: an existing API function's signature changes, a function is removed from the API)
//  * Minor number: changes whenever the API changes at all
//      (i.e., a v2.0 tool could use a v2.1 simulator, but a v2.1 tool couldn't use a v.20 simulator)
//      (example: an extra/optional function is added to the API)
enum ASAPI_MAJOR {
    ASAPI_MAJOR_0   = 0,    // Pre-production development version
    ASAPI_MAJOR_CURRENT = ASAPI_MAJOR_0
} ASAPI_MAJOR;

enum ASAPI_MINOR {
    ASAPI_MINOR_1   = 1,
    ASAPI_MINOR_2   = 2,
    ASAPI_MINOR_CURRENT = ASAPI_MINOR_2
} ASAPI_MINOR;

// Public functions that return the major and minor numbers
// identifying the version of the ArmSim API implemented by
// *this* particular ARM simulator library.
enum ASAPI_MAJOR asapi_major_version();
enum ASAPI_MINOR asapi_minor_version();

// Macro that expands to the "default" implementation of the version functions
#define DEFINE_DEFAULT_ASAPI_VERSION_FUNCTIONS \
    ASAPI_MAJOR asapi_major_version() { \
        return ASAPI_MAJOR_CURRENT; \
    } \
    ASAPI_MINOR asapi_minor_version() { \
        return ASAPI_MINOR_CURRENT; \
    }

//------------------------------------------------------------------
// NO API DETAILS *ABOVE* THIS COMMENT CAN CHANGE!!!
// (Except to add new version numbers to the appropriate enums
// and update the _CURRENT enum members appropriately...)
//------------------------------------------------------------------
// Anything *below* this comment *can* change in future versions
// of the ArmSim API (indicated by increasing version numbers).
//------------------------------------------------------------------

// Primitive typedefs
typedef uint8_t     ARM_BYTE;
typedef int8_t      ARM_SBYTE;
typedef uint16_t    ARM_HWORD;
typedef int16_t     ARM_SHWORD;
typedef uint32_t    ARM_WORD;
typedef int32_t     ARM_SWORD;

// ArmSim simulates a 32-bit address space
typedef uint32_t    ARM_ADDRESS;

// Standard status/error codes to be used by all conforming implementations.
typedef enum ARMSIM_STATUS {
    AS_OK               = 0,    // No error
    AS_MIN              = AS_OK,    // (also the minimum status code)
    
    // System errors
    AS_BAD_MALLOC       = 10,   // Failed to allocate memory
    AS_BAD_IO           = 11,   // System I/O failed (e.g., debug logging to STDERR)
    
    // API errors
    AS_NOT_IMPLEMENTED  = 20,   // This particular feature is NOT IMPLEMENTED
    AS_NULL_PTR         = 21,   // A required pointer parameter was NULL
    AS_BAD_CONST        = 22,   // An invalid constant was passed (e.g., -1 for an ARMSIM_LOGLEVEL)
    
    // Simulation errors
    AS_BAD_INSTRUCTION  = 30,   // Unknown/invalid instruction encountered
    AS_BAD_ALIGNMENT    = 31,   // WORD access at non-WORD-aligned address, etc.
    AS_SWI_ZERO         = 32,   // Not an error per-se, but an indicator that execution should not be resumed

    // Cache related errors
    AS_CACHE_DISABLED   = 50,   // Operation works only if caching is enabled, but it is not
    
    AS_MAX                      // Unused; all real status codes are < this one
} ARMSIM_STATUS;

// Standard debug-logging levels
typedef enum ARMSIM_LOGLEVEL {
    AL_OFF          = 0,    // No log messages at all
    AL_REQUIRED     = 1,    // Only offically required log messages
                            // You are free to use values in the range 2..10
    AL_MAX          = 10    // to provide fine-grained control over logging
                            // verbosity
} ARMSIM_LOGLEVEL;

// Standard trace-logging levels
typedef enum ARMSIM_TRACELEVEL {
    AT_OFF  =   0,  // No trace log output AT ALL
    AT_FAST =   1,  // Trace output with only instruction count/PC/flags/registers
    AT_FULL =   2   // Full trace output, including simulated RAM MD5
} ARMSIM_TRACELEVEL;

// Asynchronous/external events that the simulated CPU must be informed of
typedef enum ARMSIM_EVENT {
    AE_IRQ  =   1,  // Normal interrupt event
    AE_FIQ  =   2   // Fast interrupt event
} ARMSIM_EVENT;

// Basic (non-banked, non-status) ARM register identifiers.
// Used to access the currently-active set of 16 program-
// accessible general-purpose registers.
typedef enum ARM_REG {
    AR_r0   =   0,
    AR_r1   =   1,
    AR_r2   =   2,
    AR_r3   =   3,
    AR_r4   =   4,
    AR_r5   =   5,
    AR_r6   =   6,
    AR_r7   =   7,
    AR_r8   =   8,
    AR_r9   =   9,
    AR_r10  =   10,
    AR_r11  =   11,
    AR_r12  =   12,
    AR_r13  =   13,
    AR_sp   =   AR_r13,     // Alias for r13
    AR_r14  =   14,
    AR_lr   =   AR_r14,     // Alias for r14
    AR_r15  =   15,
    AR_pc   =   AR_r15,     // Alias for r15
    
    AR_MAX      =   AR_r15,
    AR_COUNT    =   AR_MAX + 1
} ARM_REG;

// ARM operating mode identifiers
typedef enum ARM_MODE {
    // Baseline mode (only mode used in down-level simulators)
    AM_SYS  =   0x1F,   // SYSTEM: privileged variant of USER
    
    // Up-level modes
    AM_FIQ  =   0x11,   // Fast hardward interrupt response mode
    AM_IRQ  =   0x12,   // Normal hardware interrupt response mode
    AM_SVC  =   0x13,   // Supervisor mode, for handling SVC/SWI instructions
    
    // Non-simulated modes (included for completeness)
    AM_USR  =   0x10,   // USER: non-privileged/user mode
    AM_ABT  =   0x17,   // ABORT: prefetch/MMU fault recovery mode
    AM_UND  =   0x1B,   // UNDEFINED: unknown instruction recovery/emulation mode    
} ARM_MODE;

// Full (banked and status) ARM register identifiers.
// Used to access the full set of [up to] 37 bank-switched registers.
typedef enum ARM_BANKED_REG {
    // Never-banked registers (the same everywhere)
    ABR_r0  =   0,
    ABR_MIN =   ABR_r0,
    ABR_r1  =   1,
    ABR_r2  =   2,
    ABR_r3  =   3,
    ABR_r4  =   4,
    ABR_r5  =   5,
    ABR_r6  =   6,
    ABR_r7  =   7,
    
    // Rarely-banked registers (the same everywhere but FIQ)
    ABR_r8_nf   =   8,      // "_nf" == "not FIQ"
    ABR_r9_nf   =   9,
    ABR_r10_nf  =   10,
    ABR_r11_nf  =   11,
    ABR_r12_nf  =   12,
    
    // USER/SYSTEM mode SP/LR
    ABR_r13_usr =   13,
    ABR_sp_usr  =   ABR_r13_usr,
    ABR_r14_usr =   14,
    ABR_lr_usr  =   ABR_r14_usr,
    
    // Global (never-banked) PC register and CPSR
    ABR_r15     =   15,
    ABR_pc      =   ABR_r15,
    ABR_cpsr    =   16,
    
    // SVC mode SP/LR/SPSR
    ABR_r13_svc     =   17,
    ABR_sp_svc      =   ABR_r13_svc,
    ABR_r14_svc     =   18,
    ABR_lr_svc      =   ABR_r14_svc,
    ABR_spsr_svc    =   19,
    
    // ABORT mode SP/LR/SPSR (not implemented by a standard ArmSim project)
    ABR_r13_abt     =   20,
    ABR_sp_abt      =   ABR_r13_abt,
    ABR_r14_abt     =   21,
    ABR_lr_abt      =   ABR_r14_abt,
    ABR_spsr_abt    =   22,
    
    // UNDEFINED mode SP/LR/SPSR (not implemented by a standard ArmSim project)
    ABR_r13_und     =   23,
    ABR_sp_und      =   ABR_r13_und,
    ABR_r14_und     =   24,
    ABR_lr_und      =   ABR_r14_und,
    ABR_spsr_und    =   25,
    
    // IRQ mode SP/LR/SPSR
    ABR_r13_irq     =   26,
    ABR_sp_irq      =   ABR_r13_irq,
    ABR_r14_irq     =   27,
    ABR_lr_irq      =   ABR_r14_irq,
    ABR_spsr_irq    =   28,
    
    // FIQ mode r8-r12/SP/LR/SPSR (not usually implemented)
    ABR_r8_fiq      =   29,
    ABR_r9_fiq      =   30,
    ABR_r10_fiq     =   31,
    ABR_r11_fiq     =   32,
    ABR_r12_fiq     =   33,
    ABR_r13_fiq     =   34,
    ABR_sp_fiq      =   ABR_r13_fiq,
    ABR_r14_fiq     =   35,
    ABR_lr_fiq      =   ABR_r14_fiq,
    ABR_spsr_fiq    =   36,
    
    ABR_MAX         = ABR_spsr_fiq,
    ABR_COUNT       = ABR_MAX + 1
    
} ARM_BANKED_REG;

// Forward declaration of [opaque] simulator context structure to be
// defined [internally] by simulator code.
typedef struct ARMSIM_CTX ARMSIM_CTX;

// Declared type of a "logging hook," used by ArmSim hosts to capture
// a simulator's trace and debug logging output programmatically.
// Takes a user-provided (at hook-registration) context pointer
// and a pointer to a NUL-terminated C-string and returns nothing.
typedef void (*ARMSIM_LOG_HOOK)(void *uctx, const char *msg);

// Declared type of "memory-mapped-I/O hooks," used by ArmSim hosts
// to intercept memory reads/writes.  This allows simulator hosts
// to maintain the simulator's RAM store *and* provide memory-mapped
// I/O access to simulated devices (e.g., a serial console).
// <address> shall always be WORD-aligned.
typedef ARM_WORD (*ARMSIM_MMIO_LOAD_HOOK)(void *uctx, ARM_ADDRESS address);
typedef void (*ARMSIM_MMIO_STORE_HOOK)(void *uctx,
                                       ARM_ADDRESS address,
                                       ARM_WORD value);


// API for creating a new ArmSim instance.
//----------------------------------------
// Allocates space for an ARMSIM_CTX, including register storage and status
// variables, callback function pointers, user-provided context pointer
// for callbacks, etc.
//
// If successful, sets *<pctx> = &new_ARMSIM_CTX_object and returns AS_OK.
// On any error, sets *<pctx> = NULL and returns an approprate ARMSIM_STATUS.
//
// CPU status should be the same as after a call to as_reset_cpu().
//
// The debug logging level should be AL_REQUIRED.  The trace logging
// level should be AT_FULL.
ARMSIM_STATUS as_create_simulator(
    ARMSIM_CTX **pctx,
    void *hook_ctx,
    ARMSIM_MMIO_LOAD_HOOK load_hook,
    ARMSIM_MMIO_STORE_HOOK store_hook);


// API for destroying an existing ArmSim instance.
//------------------------------------------------
// Releases all resources (memory, file handles, etc.) associated with
// an ArmSim instance.
// <pctx> must be NULL or the address of an ARMSIM_CTX object created by
// as_create_simulator().
ARMSIM_STATUS as_destroy_simulator(ARMSIM_CTX *ctx);


// API to set the debug-logging level for this ArmSim instance.
ARMSIM_STATUS as_set_loglevel(ARMSIM_CTX *ctx, ARMSIM_LOGLEVEL level);


// API to set the trace-logging level for this ArmSim instance
ARMSIM_STATUS as_set_tracelevel(ARMSIM_CTX *ctx, ARMSIM_TRACELEVEL level);


// API to set a debug logging callback hook (called whenever the simulator
// wishes to record a debug log message).  If <hook> is NULL, restores
// the default debug logging behavior (i.e., fputs() to STDERR).
// <uctx> is simply remembered and passed on as-is when <hook> is called.
ARMSIM_STATUS as_set_debug_log_hook(ARMSIM_CTX *ctx,
                                    ARMSIM_LOG_HOOK hook,
                                    void *uctx);


// API to set a trace logging callback hook (called each time the simulator
// needs to log an instruction execution trace record).  If <hook> is NULL,
// restores the default trace logging behavior (i.e., fputs() to STDOUT).
// <uctx> is simply remembered and passed on as-is when <hook> is called.
ARMSIM_STATUS as_set_trace_log_hook(ARMSIM_CTX *ctx,
                                    ARMSIM_LOG_HOOK hook,
                                    void *uctx);

// APIs to query/modify simulated RAM on a WORD basis *as if* the
// simulated ARM CPU were performing the loads/stores.
// Will trigger load/store hooks *unless* caching has been implemented
// and enabled.  <address> must be WORD-aligned!
ARMSIM_STATUS as_get_ram_word(ARMSIM_CTX *ctx, ARM_ADDRESS address, ARM_WORD *pval);
ARMSIM_STATUS as_set_ram_word(ARMSIM_CTX *ctx, ARM_ADDRESS address, ARM_WORD value);


// APIs to query/modify active (non-banked, non-status) registers.
ARMSIM_STATUS as_get_reg(ARMSIM_CTX *ctx, ARM_REG reg, ARM_WORD *pval);
ARMSIM_STATUS as_set_reg(ARMSIM_CTX *ctx, ARM_REG reg, ARM_WORD value);


// APIs to query/modify the current processor mode
// (May always return AS_NOT_IMPLEMENTED in down-level simulators.)
ARMSIM_STATUS as_get_mode(ARMSIM_CTX *ctx, ARM_MODE *pmd);
ARMSIM_STATUS as_set_mode(ARMSIM_CTX *ctx, ARM_MODE mode);


// APIs to query/modify *any* (including banked/status) register.
// (May always return AS_NOT_IMPLEMENTED in down-level simulators.)
ARMSIM_STATUS as_get_banked_reg(ARMSIM_CTX *ctx, ARM_BANKED_REG reg, ARM_WORD *pval);
ARMSIM_STATUS as_set_banked_reg(ARMSIM_CTX *ctx, ARM_BANKED_REG reg, ARM_WORD value);


// API to reset simulated ARM processor to its initial/boot-strap state
// (this does *not* affect simulated RAM in any way, but if the simulated CPU
// includes a *cache*, the cache is *disabled*)
ARMSIM_STATUS as_reset_cpu(ARMSIM_CTX *ctx);


// **OPTIONAL API** (if unimplemented, must return AS_NOT_IMPLEMENTED)
// Enables simulated caching of all reads/writes below <cache_limit>.
// (Passing 0 as <cache_limit> effectively turns off caching.)
// (Every call to as_cache_enable effectively flushes the cache.)
ARMSIM_STATUS as_cache_enable(ARMSIM_CTX *ctx, ARM_ADDRESS cache_limit);


// **OPTIONAL API** (if unimplemented, must return AS_NOT_IMPLEMENTED)
// If cache is both implemented and enabled, invalidates the cache line
// containing the word associated with <address>.
// Returns AS_CACHE_DISABLED if the cache is implemented but currently disabled.
ARMSIM_STATUS as_cache_invalidate(ARMSIM_CTX *ctx, ARM_ADDRESS address);


// API to inform simulated processor that an external/asynchronous event
// has happened (e.g., to trigger interrupt handling on the next
// execution cycle)
ARMSIM_STATUS as_notify_event(ARMSIM_CTX *ctx, ARMSIM_EVENT event);


// API to trigger the execution of <cycles> instructions.
// If <cycles> == 0, then execute instructions until either an
// error is detected or until an "SWI #0" instructions is emulated.
//
// If <finished> is not NULL, it will receive the number of
// instructions *actually* fully emulated (e.g., if a memory-alignement error
// is encountered after 2 instructions when 5 were requested, the
// function would return AS_ALIGNMENT_ERROR and set *<finished> to 2).
//
// If <cycles> instructions are executed without error/SWI-0, returns
// AS_OK.  If an SWI-0 is encountered (even on the final instruction),
// AS_SWI_ZERO is returned.  Otherwise an approprate error status is returned.
ARMSIM_STATUS as_execute(ARMSIM_CTX *ctx, size_t cycles, size_t *finished);

#endif
