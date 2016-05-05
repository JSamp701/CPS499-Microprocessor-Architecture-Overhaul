//
// Created by jsamp701 on 3/7/16.
//

#ifndef AS_RI_ARMSIM_CTX_H
#define AS_RI_ARMSIM_CTX_H

#include "../ASAPI/asapi.h"
#include "register.h"

typedef struct ARMSIM_INSTRUCTION ARMSIM_INSTRUCTION;

#define SANITY_NUMBER 123456789

struct ARMSIM_CTX {
    //put necessary instance variables here
    ARM_ADDRESS entry;
    int entry_set;

    int steps;

    int irq;

    ARM_WORD pfr_instruction;
    ARM_ADDRESS pfr_address;
    ARMSIM_INSTRUCTION * pdr;
    ARMSIM_INSTRUCTION * per;

    int running;

    // SANITY TEST CHECK VARIABLE: if this isn't the SANITY_NUMBER defined above, then the ctx isn't actual
    int sanity_test;

    //REGISTER variable
    ARMSIM_REGISTERS *registers;

    //IO / MEMORY VARIABLES

    void *hook_ctx;
    ARMSIM_MMIO_LOAD_HOOK load_hook;
    ARMSIM_MMIO_STORE_HOOK store_hook;
    ARMSIM_LOG_HOOK log_hook;
    ARMSIM_LOG_HOOK trace_hook;
    enum ARMSIM_LOGLEVEL loglevel;
    enum ARMSIM_TRACELEVEL tracelevel;
    int log_hook_defined;
    int trace_hook_defined;
};

// FORWARD DECLARATIONS OF CUSTOM FUNCTIONS
void as_log(ARMSIM_CTX *ctx, const char* message, int level);
void as_trace(ARMSIM_CTX *ctx, const char* message, int level);

//Declaring the methods from the CPU file

int get_steps(ARMSIM_CTX *ctx);

ARMSIM_REGISTERS * get_registers(ARMSIM_CTX *ctx);

ARM_WORD get_byte_from_memory(ARMSIM_CTX *ctx, ARM_ADDRESS address);

void set_byte_in_memory(ARMSIM_CTX *ctx, ARM_ADDRESS address, ARM_WORD value);

ARM_WORD get_word_from_memory(ARMSIM_CTX *ctx, ARM_ADDRESS address);

void set_word_in_memory(ARMSIM_CTX *ctx, ARM_ADDRESS address, ARM_WORD value);

void irq(ARMSIM_CTX *ctx);

void reset_ctx(ARMSIM_CTX *ctx);

void cpu_step(ARMSIM_CTX *ctx);

void run(ARMSIM_CTX *ctx);

void halt(ARMSIM_CTX *ctx);

void branch_cpu(ARMSIM_CTX *ctx, ARM_ADDRESS new_pc);

ARM_ADDRESS get_effective_pc(ARMSIM_CTX *ctx);

void flush(ARMSIM_CTX *ctx);

int cycle(ARMSIM_CTX *ctx);

void cpu_trace(ARMSIM_CTX *ctx, ARMSIM_INSTRUCTION * instruction);

//fetches the raw integer instruction
ARM_WORD fetch_instruction(ARMSIM_CTX *ctx, ARM_ADDRESS * address);// use the address argument to pass back the address

ARMSIM_INSTRUCTION * as_decode_instruction(ARMSIM_CTX *ctx, ARM_ADDRESS address, ARM_WORD instruction);

ARMSIM_INSTRUCTION * as_execute_instruction(ARMSIM_CTX *ctx, ARMSIM_INSTRUCTION * cooked_instruction);

#endif //AS_RI_ARMSIM_CTX_H
