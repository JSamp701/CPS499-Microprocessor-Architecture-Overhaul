//
// Created by joel on 4/30/16.
//

#ifndef AS_RI_REGISTER_H
#define AS_RI_REGISTER_H

#include "../ASAPI/asapi.h"

#define REGISTER_SANITY_NUMBER 987654321

//# Master register bank
//#---------------------------------------------
//# 0-7: unbanked registers (used in all modes)
//# 8-12: banked GPRs (used everywhere but FIQ)
//# 13-14: SP_usr, LR_usr
//# 15: PC (global, UNUSED)
//# 16: CPSR (global, UNUSED)
//# 17-19: SP_svc, LR_svc, SPSR_svc
//# 20-22: SP_abt, LR_abt, SPSR_abt (unused)
//# 23-25: SP_und, LR_und, SPSR_und (unused)
//# 26-28: SP_irq, LR_irq, SPSR_irq
//# 29-33: R8_fiq-R12_fiq
//# 34-36: SP_fiq, LR_fiq, SPSR_fiq

typedef ARM_WORD * ARMSIM_REGISTERS;

typedef struct ARMSIM_CTX ARMSIM_CTX;

static ARMSIM_CTX *reg_static_context;
static int context_defined;

void reg_set_static_context(ARMSIM_CTX *ctx);

ARMSIM_REGISTERS * create_registers();

void delete_registers(ARMSIM_REGISTERS *registers);

ARM_WORD get_cpsr(ARMSIM_REGISTERS *registers);

void set_cpsr(ARMSIM_REGISTERS *registers, unsigned int cpsr);

int can_fiq(ARMSIM_REGISTERS *registers);

int can_irq(ARMSIM_REGISTERS *registers);

ARM_WORD get_spsr(ARMSIM_REGISTERS *registers);

ARM_MODE get_mode(ARMSIM_REGISTERS *registers);

void set_mode(ARMSIM_REGISTERS *registers, ARM_MODE mode);

void reset_registers(ARMSIM_REGISTERS *registers, ARM_ADDRESS entry);

void set_mode_complex(ARMSIM_REGISTERS *registers, ARM_MODE mode, int save_cpsr);

void revert_mode(ARMSIM_REGISTERS *registers);

void set_flags(ARMSIM_REGISTERS *registers, int nFlag, int cFlag, int zFlag, int vFlag);

ARM_WORD get_register(ARMSIM_REGISTERS *registers, ARM_REG reg);

void set_register(ARMSIM_REGISTERS *registers, ARM_REG reg, ARM_WORD value);

ARM_WORD increment(ARMSIM_REGISTERS *registers, ARM_REG reg, ARM_WORD amount, int post);

#endif //AS_RI_REGISTER_H
