//
// Created by joel on 4/30/16.
//

#include <stdlib.h>
#include "ASAPI/asapi.h"
#include "JSAMP701_API/register.h"
#include "JSAMP701_API/armsim-ctx.h"

void reg_set_static_context(ARMSIM_CTX *ctx){
    reg_static_context = ctx;
    context_defined = 1;
}

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

ARMSIM_REGISTERS * create_registers() {
    ARMSIM_REGISTERS *registers = (ARMSIM_REGISTERS*)malloc(sizeof(ARMSIM_REGISTERS));// so, this is a pointer, to a pointer to a list of ARM_WORD
    ARM_WORD * actual_registers = (ARM_WORD*)malloc(sizeof(ARM_WORD)*37);
    (*registers) = actual_registers;
    return registers;
}

void delete_registers(ARMSIM_REGISTERS *registers){
    free((*registers));
    free(registers);
}

unsigned int get_cpsr(ARMSIM_REGISTERS *registers) {
    return (*registers)[ABR_cpsr];
}

void set_cpsr(ARMSIM_REGISTERS *registers, unsigned int cpsr){
    (*registers)[ABR_cpsr] = cpsr;
}

int can_fiq(ARMSIM_REGISTERS *registers) {
    return (get_cpsr(registers) & 0x40);
}

int can_irq(ARMSIM_REGISTERS *registers) {
    return ((get_cpsr(registers)) & 0x80);
}

ARM_WORD get_spsr(ARMSIM_REGISTERS *registers) {
    ARM_MODE mode = get_mode(registers);
    switch(mode){
        case AM_FIQ:
            return (*registers)[ABR_spsr_fiq];
        case AM_IRQ:
            return (*registers)[ABR_spsr_irq];
        case AM_SVC:
            return (*registers)[ABR_spsr_svc];
        default:
            return 0;
    }
}

ARM_MODE get_mode(ARMSIM_REGISTERS *registers) {
    return (ARM_MODE)(get_cpsr(registers) & 0x1f);
}

void set_mode(ARMSIM_REGISTERS *registers, ARM_MODE mode){
    //self._cpsr.value = (self._cpsr.value & 0xffffffe0) | (mode & 0x1f)
    ARM_WORD new_cpsr = 0;
    new_cpsr = get_cpsr(registers);
    new_cpsr = new_cpsr & 0xffffffe0;
    new_cpsr = new_cpsr | ( mode & 0x1f);
    set_cpsr(registers, new_cpsr);
}

void reset_registers(ARMSIM_REGISTERS *registers, ARM_ADDRESS entry) {
    as_log(reg_static_context, "ENTERED RESET REGISTERS\n",0);
    int i = 0;
    for(i = 0; i < 36; ++i){
        (*registers)[i] = 0;
    }
    as_log(reg_static_context, "50% RESET REGISTERS\n",0);
    set_cpsr(registers, 0b00011111);

//        self._r[15] = entry
//        self._r[13] = 0x7000
    as_log(reg_static_context, "~60% RESET REGISTERS\n",0);
    (*registers)[15] = entry;
    (*registers)[13] = 0x7000;
    as_log(reg_static_context, "100% RESET REGISTERS\n",0);
}

void set_mode_complex(ARMSIM_REGISTERS *registers, ARM_MODE mode, int save_cpsr){
    //because my register accessing logic is modified to directly access the correct cells, I do not need to
    //back up / store any registers here, except the cpsr
    if(save_cpsr != 0 && (mode == AM_SVC || mode == AM_FIQ || mode == AM_SYS || mode == AM_IRQ) ){
        if(mode == AM_SVC){
            (*registers)[ABR_spsr_svc] = get_cpsr(registers);
        }
        else if(mode == AM_IRQ){
            (*registers)[ABR_spsr_irq] = get_cpsr(registers);
        }
        else if(mode == AM_FIQ){
            (*registers)[ABR_spsr_fiq] = get_cpsr(registers);
        }
    }
    set_mode(registers, mode);
}

void revert_mode(ARMSIM_REGISTERS *registers){
    ARM_MODE mode = get_mode(registers);
    if(mode == AM_FIQ || mode == AM_IRQ || mode == AM_SVC) {
        ARM_WORD spsr = get_spsr(registers);
        set_mode_complex(registers, spsr & 0x1F, 0);
        set_cpsr(registers,spsr);
    }
}

void set_flags(ARMSIM_REGISTERS *registers, int nFlag, int cFlag, int zFlag, int vFlag){
//# Compose 4-bit condition field at the top of the CPSR
//    field = (0, 0x80000000)[n] | (0, 0x40000000)[z] | (0, 0x20000000)[c] | (0, 0x10000000)[v]
//
//# Actually update the register
//    self._cpsr.value = field | (self._cpsr.value & 0x0fffffff)
    unsigned int field = 0;
    ARM_WORD cpsr = get_cpsr(registers);
    field = ((unsigned int)nFlag << 31) | ((unsigned int)zFlag << 30) | ((unsigned int)cFlag << 29) | ((unsigned int)vFlag << 28);
    set_cpsr(registers, field | cpsr & 0x0fffffff);
}


//self._bank_map = {
//        MODE_SYS: {
//                13: MB[13],
//                14: MB[14],
//        },
//        MODE_SVC: {
//                13: MB[17],
//                14: MB[18],
//        },
//        MODE_IRQ: {
//                13: MB[26],
//                14: MB[27],
//        },
//        MODE_FIQ: {
//                8:  MB[29],
//                9:  MB[30],
//                10: MB[31],
//                11: MB[32],
//                12: MB[33],
//                13: MB[34],
//                14: MB[35],
//        }
//}
ARM_WORD get_register(ARMSIM_REGISTERS *registers, ARM_REG reg) {
    ARM_WORD value = 0;
    ARM_MODE mode = get_mode(registers);
    if(reg < AR_COUNT) {
        if (mode == AM_SVC && (reg == 13 || reg == 14)){
            return (*registers)[reg + 4];
        }
        else if(mode == AM_IRQ && (reg == 13 || reg == 14)){
            return (*registers)[reg + 13];
        }
        else if(mode == AM_FIQ && (reg < 15 && reg > 7)){
            return (*registers)[reg + 21];
        }
        return (*registers)[reg];
    }
    return value;
}

void set_register(ARMSIM_REGISTERS *registers, ARM_REG reg, ARM_WORD value){
    ARM_MODE mode = get_mode(registers);
    if(reg < AR_COUNT){
        if (mode == AM_SVC && (reg == 13 || reg == 14)){
            (*registers)[reg + 4] = value;
        }
        else if(mode == AM_IRQ && (reg == 13 || reg == 14)){
            (*registers)[reg + 13] = value;
        }
        else if(mode == AM_FIQ && (reg < 15 && reg > 7)){
            (*registers)[reg + 21] = value;
        }
        (*registers)[reg] = value;
    }
}

ARM_WORD increment(ARMSIM_REGISTERS *registers, ARM_REG reg, ARM_WORD amount, int post) {
    ARM_WORD value = 0;
    ARM_WORD old = 0;
    if(reg < AR_COUNT){
        old = get_register(registers, reg);
        set_register(registers, reg, old + amount);
        value = (post != 0)? old : get_register(registers, reg);
    }
    return value;
}