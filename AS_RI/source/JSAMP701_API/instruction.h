//
// Created by joel on 5/3/16.
//

#ifndef AS_RI_INSTRUCTION_H
#define AS_RI_INSTRUCTION_H

#include "../ASAPI/asapi.h"
#include "utils.h"

typedef struct ARMSIM_CTX ARMSIM_CTX;

#define INSTRUCTION_SANITY_NUMBER 123454321

static ARMSIM_CTX *static_context;
static int context_defined;

void set_static_context(ARMSIM_CTX *ctx);

typedef enum INSTRUCTION_TYPE{
    base,
    data_processing,
    mul,
    ld_str,
    ld_str_multi,
    branch,
    branch_xchg,
    swi,
    mrs,
    msr
} INSTRUCTION_TYPE ;

typedef struct ARMSIM_INSTRUCTION {
    // Variables drawn from the base instruction type
    ARM_ADDRESS address;
    ARM_WORD raw_instruction;
    ARM_CONDITION condition;
    int affects_status;
    int error;

    int sanity_test;

    // Custom variables for extension
    INSTRUCTION_TYPE instruction_type; //this is used to determine what to cast the associated data as...
    void *data;     // used to store additional data for each instruction
                    // this data will be stored in structs (to be defined below) depending on the type of command
} ARMSIM_INSTRUCTION;

char * instruction_to_string(ARMSIM_INSTRUCTION * instruction);

ARMSIM_INSTRUCTION * master_decode(ARM_ADDRESS address, ARM_WORD raw_instruction);

void actually_execute_instruction(ARMSIM_INSTRUCTION * instruction);

void free_instruction(ARMSIM_INSTRUCTION * instruction);

#endif //AS_RI_INSTRUCTION_H
