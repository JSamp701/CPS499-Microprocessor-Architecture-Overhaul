//
// Created by jsamp701 on 3/7/16.
//

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "ASAPI/asapi.h"
#include "JSAMP701_API/armsim-ctx.h"
#include "JSAMP701_API/register.h"
#include "JSAMP701_API/instruction.h"
#include "JSAMP701_API/utils.h"


//forward declaration of custom functions
void irq(ARMSIM_CTX *ctx);
void reset_ctx(ARMSIM_CTX *ctx);
int get_steps(ARMSIM_CTX *ctx);
ARMSIM_REGISTERS * get_registers(ARMSIM_CTX *ctx);;
void reset_ctx(ARMSIM_CTX *ctx);
void cpu_step(ARMSIM_CTX *ctx);
void run(ARMSIM_CTX *ctx);
void halt(ARMSIM_CTX *ctx);
void branch_cpu(ARMSIM_CTX *ctx, ARM_ADDRESS new_pc);
ARM_ADDRESS get_effective_pc(ARMSIM_CTX *ctx);
void flush(ARMSIM_CTX *ctx);
int cycle(ARMSIM_CTX *ctx);
void cpu_trace(ARMSIM_CTX *ctx, ARMSIM_INSTRUCTION * instruction);
ARM_WORD fetch_instruction(ARMSIM_CTX *ctx, ARM_ADDRESS * address);
ARMSIM_INSTRUCTION * as_decode_instruction(ARMSIM_CTX *ctx, ARM_ADDRESS address, ARM_WORD instruction);
ARMSIM_INSTRUCTION * as_execute_instruction(ARMSIM_CTX *ctx, ARMSIM_INSTRUCTION * cooked_instruction);

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
    ARMSIM_STATUS status = AS_OK;
    ARMSIM_CTX *ctx = (ARMSIM_CTX*)malloc(sizeof(ARMSIM_CTX));
    (*pctx) = ctx;

    //do the IO stuff
    ctx->hook_ctx = hook_ctx;
    ctx->load_hook = load_hook;
    ctx->store_hook = store_hook;
    ctx->sanity_test = SANITY_NUMBER;
    ctx->loglevel = AL_DEFAULT;
    ctx->tracelevel = AT_DEFAULT;
    ctx->log_hook_defined = 0;
    ctx->trace_hook_defined = 0;

    //now for instance variables
    ctx->registers = create_registers();
    ctx->entry = 0;
    ctx->entry_set = 0;
    ctx->steps = 0;
    ctx->irq = 0;
    ctx->pfr_address = 0;
    ctx->pfr_instruction = 0;
    ctx->pdr = 0;
    ctx->per = 0;
    ctx->running = 0;

    set_static_context(ctx);
    reg_set_static_context(ctx);

    return status;
}

ARMSIM_STATUS as_destroy_simulator(ARMSIM_CTX *ctx){
    delete_registers(ctx->registers);
    free(ctx);
    return AS_OK;
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
    return AS_OK;
}

ARMSIM_STATUS as_set_trace_log_hook(ARMSIM_CTX *ctx, ARMSIM_LOG_HOOK hook, void *uctx) {
    if(ctx == 0 || ctx->sanity_test != SANITY_NUMBER){
        return AS_NULL_PTR;
    }
    ctx->trace_hook = hook;
    ctx->trace_hook_defined = 1;
    return AS_OK;
}

ARMSIM_STATUS as_get_ram_word(ARMSIM_CTX *ctx, ARM_ADDRESS address, ARM_WORD *pval) {
    (*pval) = get_word_from_memory(ctx, address);
    return AS_OK;
}

ARMSIM_STATUS as_set_ram_word(ARMSIM_CTX *ctx, ARM_ADDRESS address, ARM_WORD value) {
    set_word_in_memory(ctx, address, value);
    return AS_OK;
}

ARMSIM_STATUS as_get_reg(ARMSIM_CTX *ctx, ARM_REG reg, ARM_WORD *pval) {
    (*pval) = get_register(ctx->registers, reg);
    return AS_OK;
}

ARMSIM_STATUS as_set_reg(ARMSIM_CTX *ctx, ARM_REG reg, ARM_WORD value) {
    set_register(ctx->registers, reg, value);
    return AS_OK;
}

ARMSIM_STATUS as_get_mode(ARMSIM_CTX *ctx, ARM_MODE *pmd) {
    (*pmd) = get_mode(ctx->registers);
    return AS_OK;
}

ARMSIM_STATUS as_set_mode(ARMSIM_CTX *ctx, ARM_MODE mode) {
    set_mode(ctx->registers, mode);
    return AS_OK;
}

ARMSIM_STATUS as_get_banked_reg(ARMSIM_CTX *ctx, ARM_BANKED_REG reg, ARM_WORD *pval) {
    (*pval) = (*(ctx->registers))[reg];
    return AS_OK;
}

ARMSIM_STATUS as_set_banked_reg(ARMSIM_CTX *ctx, ARM_BANKED_REG reg, ARM_WORD value) {
    ARMSIM_REGISTERS * registers = ctx->registers;
    ARM_WORD * actual_registers = *registers;
    actual_registers[reg] = value;
    return AS_OK;
}

ARMSIM_STATUS as_reset_cpu(ARMSIM_CTX *ctx) {
    reset_ctx(ctx);
    return AS_OK;
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

ARMSIM_STATUS as_set_entry(ARMSIM_CTX *ctx, ARM_ADDRESS entry){
    ctx->entry = entry;
    ctx->entry_set = 1;
//    as_log(ctx, "LEAVING AS_SET_ENTRY\n",0);
    return AS_OK;
}

ARMSIM_STATUS as_execute(ARMSIM_CTX *ctx, size_t cycles, size_t *finished) {
    if(!ctx->entry_set){
        return AS_BAD_ENTRY;
    }
    as_log(ctx, "Entering as_execute\n",0);

    int start_steps = get_steps(ctx);
    int current_steps = start_steps;
    if(cycles == 0){
        run(ctx);
    }
    else {
        for (; current_steps - start_steps < cycles && current_steps == get_steps(ctx); ++current_steps) {
            cpu_step(ctx);
        }
    }
    (*finished) = (unsigned int)(get_steps(ctx) - start_steps);
    as_log(ctx, "Leaving as_execute\n",0);
    return AS_OK;
}

ARMSIM_STATUS as_level(const char **level){
    (*level) = "3:100:Reference implementation";
    return AS_OK;
}

// CUSTOM FUNCTIONS
void as_trace(ARMSIM_CTX* ctx, const char* message, int level ){
    if(ctx->trace_hook_defined != 0)
    {
        ctx->trace_hook(ctx->hook_ctx, message);
    }
}

static int logNum = 0;

void as_log(ARMSIM_CTX* ctx, const char* message, int level ){
    if(ctx->log_hook_defined != 0)
    {
        char * string = (char*)malloc(sizeof(char)*20);
        sprintf(string, "MESSAGENUM: %d", ++logNum);
        ctx->log_hook(ctx->hook_ctx, string);
        ctx->log_hook(ctx->hook_ctx, message);
        free(string);
    }
}

// FUNCTIONS FROM JPJUECK'S ARMSIM

int get_steps(ARMSIM_CTX *ctx){
    return ctx->steps;
}

ARMSIM_REGISTERS * get_registers(ARMSIM_CTX *ctx){
    return ctx->registers;
}

//based on the way the memory is set up, WORD-alignment isnt necessary.
//(the memory is an array of bytes, and is indexed as such)
ARM_WORD get_byte_from_memory(ARMSIM_CTX *ctx, ARM_ADDRESS address){
/*    int diffFromMod8 = address % ;
    ARM_ADDRESS address_to_use = address;
    if(diffFromMod8 != 0){
        //going to need to be a bit trickys
        address_to_use = address - diffFromMod8;
    }
    ARM_WORD result = ctx->load_hook(ctx->hook_ctx, address_to_use);
    ARM_WORD mask = 0xFF;
    for(int i = 0; i < diffFromMod8; ++i){
        mask = mask << 8;
    }*/
    return (ctx->load_hook(ctx->hook_ctx, address) & 0xFF);
}

void set_byte_in_memory(ARMSIM_CTX *ctx, ARM_ADDRESS address, ARM_WORD value){
    ARM_WORD original = ctx->load_hook(ctx->hook_ctx, address);
    original = (original & 0xFFFFFF00) | value;
    ctx->store_hook(ctx->hook_ctx, address, original);
}

ARM_WORD get_word_from_memory(ARMSIM_CTX *ctx, ARM_ADDRESS address){
    return ctx->load_hook(ctx->hook_ctx, address);
}

void set_word_in_memory(ARMSIM_CTX *ctx, ARM_ADDRESS address, ARM_WORD value){
    return ctx->store_hook(ctx->hook_ctx, address, value);
}

void irq(ARMSIM_CTX *ctx){
    ctx->irq = 1;
}

void reset_ctx(ARMSIM_CTX *ctx){
    as_log(ctx, "ENTERING RESET_CTX\n",0);
    ctx->pfr_address = 0;
    ctx->pfr_instruction = 0;
    if(ctx->pdr !=0) {
        free_instruction(ctx->pdr);
    }
    ctx->pdr = 0;
    if(ctx->per != 0){
        free_instruction(ctx->per);
    }
    ctx->per = 0;
    reset_registers(ctx->registers, ctx->entry);
    ctx->steps = 0;
    ctx->running = 0;
    as_log(ctx, "Leaving as_execute\n",0);
}

void cpu_step(ARMSIM_CTX *ctx){
    as_log(ctx, "ENTERING STEP\n",0);
    while(cycle(ctx) == 0){
        continue;
    }
    flush(ctx);
    as_log(ctx, "Leaving step\n",0);
}

void run(ARMSIM_CTX *ctx){
    as_log(ctx, "Entering run\n",0);
    ctx->running = 1;
    while(ctx->running == 1){
        cycle(ctx);
    }

    flush(ctx);
    as_log(ctx, "Leaving run\n",0);
}

void halt(ARMSIM_CTX *ctx){
    ctx->running = 0;
}

void branch_cpu(ARMSIM_CTX *ctx, ARM_ADDRESS new_pc){
    as_log(ctx, "Entering branch_cpu\n",0);
    char * log_message = (char*)malloc(sizeof(char) * LOG_STRING_LENGTH);
    char * instruction_string = instruction_to_string(ctx->pdr);
    sprintf(log_message, "**** BRANCH TO %#010x -- FLUSHING PIPELINE [pfr=%#010x, pdr=[%s] ****",
    new_pc, ctx->pfr_instruction, instruction_string);
    as_log(ctx, (const char *)log_message, 0);
    free(log_message);
    free(instruction_string);
    ctx->pfr_address = 0;
    ctx->pfr_instruction = 0;
    if(ctx->pdr != 0){
        free_instruction(ctx->pdr);
    }
    ctx->pdr = 0;
    set_register(ctx->registers, AR_r15, new_pc);
    as_log(ctx, "Leaving branch_cpu\n",0);
}

ARM_ADDRESS get_effective_pc(ARMSIM_CTX *ctx){
    as_log(ctx, "Entering get_effective_pc\n",0);
    int delta = 0;
    if(ctx->pfr_address != 0 && ctx->pfr_instruction != 0){
        if(ctx->pdr != 0)
            delta = 8;
        else
            delta = 4;
    }
    as_log(ctx, "Leaving get_efffective_pc\n",0);
    return get_register(ctx->registers, AR_r15) - delta;
}

void flush(ARMSIM_CTX *ctx){
    as_log(ctx, "Entering flush\n",0);
    set_register(ctx->registers, AR_r15, get_effective_pc(ctx));
    ctx->pfr_address = 0;
    ctx->pfr_instruction = 0;
    if(ctx->pdr != 0){
        free_instruction(ctx->pdr);
    }
    ctx->pdr = 0;
    as_log(ctx, "Leaving flush\n",0);
}

int cycle(ARMSIM_CTX *ctx){
    as_log(ctx, "Entering cycle\n",0);
    if(ctx->irq != 0 && can_irq(ctx->registers)){
        unsigned int irq_ret = get_effective_pc(ctx) + 4;
        if(ctx->log_hook_defined != 0){
            as_log(ctx, "----------------------------------------------------------------------------------------", 0);
            char * string1 = (char*)malloc(sizeof(char)*LOG_STRING_LENGTH);
            sprintf(string1, "!!!! IRQ: ePC=%#010x, tPC=%#010x !!!!", get_effective_pc(ctx), get_register(ctx->registers, AR_r15));
            as_log(ctx, string1, 0);
            free(string1);
            char * string2 = (char*)malloc(sizeof(char)*LOG_STRING_LENGTH);
            char * instruction_string = instruction_to_string(master_decode(irq_ret - 4, get_word_from_memory(ctx, irq_ret - 4)));
            sprintf(string2, "???? Effective IRQ return: %s ????", instruction_string);
            as_log(ctx, string2, 0);
            free(string2);
        }
        set_mode(ctx->registers, AM_IRQ);
        set_cpsr(ctx->registers, get_cpsr(ctx->registers) | 0xC0);
        set_register(ctx->registers, AR_r14, irq_ret);
        branch_cpu(ctx, 0x00000018);
        ctx->irq = 0;

    }

    ctx-> per = as_execute_instruction(ctx, ctx->pdr);

    ctx->pdr = as_decode_instruction(ctx, ctx->pfr_address, ctx->pfr_instruction);

    ARM_ADDRESS * address = (ARM_ADDRESS*)malloc(sizeof(ARM_ADDRESS));
    ctx->pfr_instruction = fetch_instruction(ctx, address);
    ctx->pfr_address = *address;
    free(address);
    if(ctx->per != 0) {
        ctx->steps += 1;
        if (ctx->trace_hook_defined != 0) {
            cpu_trace(ctx, ctx->per);
        }
    }
    as_log(ctx, "Leaving cycle\n",0);
    return (ctx->per != 0);
}

void cpu_trace(ARMSIM_CTX *ctx, ARMSIM_INSTRUCTION * instruction){
    as_log(ctx, "Entering cpu_trace\n",0);
    ARM_WORD *r = (*ctx->registers);
    if(instruction != 0){
        char *traceString1 = (char*)malloc(sizeof(char)*LOG_STRING_LENGTH);
        sprintf(traceString1, "%06d %08x %08x", ctx->steps, instruction->address, get_cpsr(ctx->registers));
        as_trace(ctx, traceString1, 0);
        free(traceString1);
        char *traceString2 = (char*)malloc(sizeof(char)*LOG_STRING_LENGTH);
        sprintf(traceString2, "\\t 0=%08x  1=%08x  2=%08x  3=%08x  4=%08x  5=%08x  6=%08x  7=%08x",
        r[0], r[1], r[2], r[3], r[4], r[5], r[6], r[7]);
        as_trace(ctx, traceString2, 0);
        free(traceString2);
        char *traceString3 = (char*)malloc(sizeof(char)*LOG_STRING_LENGTH);
        sprintf(traceString3, "\\t 8=%08x  9=%08x 10=%08x 11=%08x 12=%08x 13=%08x 14=%08x",
        r[8], r[9], r[10], r[11], r[12], r[13], r[14]);
        as_trace(ctx, traceString3,0);
        free(traceString3);
    }
    as_log(ctx, "Leaving cpu_trace\n",0);
}

//fetches the raw integer instruction
ARM_WORD fetch_instruction(ARMSIM_CTX *ctx, ARM_ADDRESS * address) {// use the address argument to pass back the address
    as_log(ctx, "Entering fetch_instruction\n",0);
    ARM_WORD instruction = 0;
    (*address) = increment(ctx->registers, AR_r15, 4, 1);
    instruction = get_word_from_memory(ctx, *address);
    char * logstring = (char*)malloc(sizeof(char)*LOG_STRING_LENGTH);
    sprintf(logstring, "\t\tFETCH(%#010x) -> %#010x", *address, instruction);
    as_log(ctx, logstring, 0);
    free(logstring);
    as_log(ctx, "Leaving fetch_instruction\n",0);
    return instruction;
}

ARMSIM_INSTRUCTION * as_decode_instruction(ARMSIM_CTX *ctx, ARM_ADDRESS address, ARM_WORD instruction){
    as_log(ctx, "Entering as_decode_instruction\n",0);
    if(address == 0 && instruction == 0){
        as_log(ctx, "Leaving as_decode_instruction\n",0);
        return 0;
    }

    ARMSIM_INSTRUCTION * cooked = master_decode(address, instruction);
    char * string = (char*)malloc(sizeof(char)*LOG_STRING_LENGTH);
    char * instruction_string = instruction_to_string(cooked);
    sprintf(string, "\tDECODE(%#010x @ %#010x) -> %s", address, instruction, instruction_string);
    as_log(ctx, string, 0);
    free(instruction_string);
    free(string);
    as_log(ctx, "Leaving as_decode_instruction\n",0);
    return cooked;
}

ARMSIM_INSTRUCTION * as_execute_instruction(ARMSIM_CTX *ctx, ARMSIM_INSTRUCTION * cooked_instruction){
    as_log(ctx, "Entering as_execute_instruction\n",0);
    if(cooked_instruction == 0){
        as_log(ctx, "Leaving as_execute_instruction\n",0);
        return 0;
    }
    char * string = (char*)malloc(sizeof(char) * LOG_STRING_LENGTH);
    char * instruction_string = instruction_to_string(cooked_instruction);
    if(cond_test(cooked_instruction->condition, get_cpsr(ctx->registers)) != 0){
        sprintf(string, "EXECUTING([%s] %x)", instruction_string, get_mode(ctx->registers));
        actually_execute_instruction(cooked_instruction);
    }
    else{
        sprintf(string, "SKIPPING([%s] %x)", instruction_string, get_mode(ctx->registers));
    }
    as_log(ctx, string, 0);
    free(string);
    free(instruction_string);
    as_log(ctx, "Leaving as_execute_instruction\n",0);
    return cooked_instruction;
}

