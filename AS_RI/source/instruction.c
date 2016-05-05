//
// Created by joel on 5/3/16.
//

#include "JSAMP701_API/instruction.h"
#include "JSAMP701_API/register.h"
#include "JSAMP701_API/armsim-ctx.h"

#include <stdlib.h>
#include <stdio.h>

void set_static_context(ARMSIM_CTX *ctx){
    static_context = ctx;
    context_defined = 1;
}

//BASE
ARMSIM_INSTRUCTION * create_base(ARM_ADDRESS address, ARM_WORD raw){
    as_log(static_context, "Entering Base Create\n",0);
    ARMSIM_INSTRUCTION *instruction = (ARMSIM_INSTRUCTION*)malloc(sizeof(ARMSIM_INSTRUCTION));
    instruction->address = address;
    instruction->raw_instruction = raw;
    instruction->condition = (ARM_CONDITION)((raw & 0xf0000000) >> 28);
    instruction->affects_status = 0;
    instruction->instruction_type = base;
    instruction->error = 0;
    instruction->sanity_test = INSTRUCTION_SANITY_NUMBER;
    as_log(static_context, "Leaving Base Create\n",0);
    return instruction;
}

void base_execute(ARMSIM_INSTRUCTION * instruction){
    as_log(static_context, "Base Execute\n",0);
}

char * base_to_string(ARMSIM_INSTRUCTION * instruction){
    as_log(static_context, "Entering Base String\n",0);
    char * logstring = (char*)malloc(sizeof(char) * LOG_STRING_LENGTH);
    sprintf(logstring, "%#010x\t%#010x\t??%x",instruction->address, instruction->raw_instruction, instruction->condition);
    as_log(static_context, "Leaving Base String\n",0);
    return logstring;
}

//BRANCHXCHG
ARMSIM_INSTRUCTION * decode_branchxchg(ARM_ADDRESS address, ARM_WORD raw){
    as_log(static_context, "Entering BRANCHXCHG Decode\n",0);
    ARMSIM_INSTRUCTION * instruction = create_base(address, raw);
    instruction->instruction_type = branch_xchg;
    //just store the integer, nothing else is really needed...
    instruction->data = (void*)(raw & 0x0000000f);
    as_log(static_context, "Leaving BRANCHXCHG Decode\n",0);
    return instruction;
}

char * branchxchg_to_string(ARMSIM_INSTRUCTION * instruction){
    as_log(static_context, "Entering BRANCHXCHG String\n",0);
    char * logstring = (char*)malloc(sizeof(char) * LOG_STRING_LENGTH);
    sprintf(logstring, "%010x\t%#010x\tbx%x\t%x",instruction->address, instruction->raw_instruction, (int)(instruction->data), instruction->condition);
    as_log(static_context, "Leaving BRANCHXCHG String\n",0);
    return logstring;
}

void branchxchg_execute(ARMSIM_INSTRUCTION * instruction){
    as_log(static_context, "Entering BRANCHXCHG Execute\n",0);
    if(context_defined != 0){
        branch_cpu(static_context, (ARM_ADDRESS)instruction->data);
    }
    as_log(static_context, "Leaving BRANCHXCHG Execute\n",0);
}


//MUL
typedef struct MUL_DATA{
    int rd, rs, rm;
} MUL_DATA;

ARMSIM_INSTRUCTION * decode_mul(ARM_ADDRESS address, ARM_WORD raw){
    as_log(static_context, "Entering MUL Decode\n",0);
    ARMSIM_INSTRUCTION * instruction = create_base(address, raw);
    if((raw & 0x0fc00000) != 0){
        instruction->error = 1;
    }
    MUL_DATA * data = (MUL_DATA*)malloc(sizeof(MUL_DATA));
    instruction->data = (void*)data;
    instruction->affects_status = (raw & 0x00100000) == 0x00100000;
    data->rd = (raw & 0x000f0000) >> 16;
    data->rs = (raw & 0x00000f00) >> 8;
    data->rm = raw & 0x0000000f;
    instruction->instruction_type = mul;
    as_log(static_context, "Leaving MUL Decode\n",0);
    return instruction;
}

char * mul_to_string(ARMSIM_INSTRUCTION * instruction){
    as_log(static_context, "Entering MUL String\n",0);
    MUL_DATA * data = (MUL_DATA*)instruction->data;
    char * logstring = (char*)malloc(sizeof(char) * LOG_STRING_LENGTH);
//    return "{0:#010x}\t{1:#010x}\tmul{5}\t{2}, {3}, {4}".format(self._addr, self._raw,
//                                                                REG_NAMES[self._rd],
//                                                                REG_NAMES[self._rm],
//                                                                REG_NAMES[self._rs],
//                                                                self.cond_name)
    sprintf(logstring, "%#010x\t%#010x\tmul%x\t%x, %x, %x",instruction->address, instruction->raw_instruction,
            instruction->condition, data->rd, data->rm, data->rs);
    as_log(static_context, "Leaving MUL String\n",0);
    return logstring;
}

void mul_execute(ARMSIM_INSTRUCTION * instruction){
    as_log(static_context, "Entering MUL Execute\n",0);
    ARMSIM_REGISTERS * registers = static_context->registers;
    MUL_DATA * data = (MUL_DATA*)instruction->data;
    ARM_WORD value = get_register(registers, data->rm);
    value = value * get_register(registers, data->rs);
    set_register(registers, data->rd, value);
    as_log(static_context, "Leaving MUL Execute\n",0);
}

//MRS
typedef struct MRS_DATA{
    int rd, r;
} MRS_DATA;
ARMSIM_INSTRUCTION * decode_mrs(ARM_ADDRESS address, ARM_WORD raw){
    as_log(static_context, "Entering MRS Decode\n",0);
    ARMSIM_INSTRUCTION * instruction = create_base(address, raw);
    MRS_DATA * data = (MRS_DATA*)malloc(sizeof(MRS_DATA));
    data->rd = (raw & 0xF000) >> 12;
    data->r = (raw & 0x00400000) == 0x00400000;
    instruction->data = (void*)data;
    instruction->instruction_type = mrs;
    as_log(static_context, "Leaving MRS Decode\n",0);
    return instruction;
}

char * mrs_to_string(ARMSIM_INSTRUCTION * instruction){
    as_log(static_context, "Entering Mrs String\n",0);
    MRS_DATA * data = (MRS_DATA*)instruction->data;
//    src = "SPSR" if self._r else "CPSR"
//    return "{0:#010x}\t{1:#010x}\tmrs{2}\t{3}, {4}".format(self._addr, self._raw, self.cond_name,
//                                                           REG_NAMES[self._rd], src)
//    char * logstring = (char*)malloc(sizeof(char) * LOG_STRING_LENGTH);
//    sprintf(logstring, "%010x\t%#010x\tbx%x\t%x",instruction->address, instruction->raw_instruction, (int)(instruction->data), instruction->condition);
//    return logstring;
    const char * source = (data->r)?"SPSR":"CPSR";
    char * logstring = (char*)malloc(sizeof(char) * LOG_STRING_LENGTH);
    sprintf(logstring, "%#010x\t%#010x\tmrs%x\t%x, %s",instruction->address, instruction->raw_instruction,
            instruction->condition, data->rd, source);
    as_log(static_context, "Leaving Mrs String\n",0);
    return logstring;
}

void mrs_execute(ARMSIM_INSTRUCTION * instruction){
    as_log(static_context, "Entering MRS Execute\n",0);
    ARM_WORD value = 0;
    MRS_DATA * data = (MRS_DATA*)instruction->data;
    if(data->r != 0){
        value = get_spsr(static_context->registers);
    }
    else{
        value = get_cpsr(static_context->registers);
    }
    set_register(static_context->registers, (ARM_REG)data->rd, value);
    as_log(static_context, "Leaving MRS Execute\n",0);
}

const char * shift_names[] = {
        "lsl", "lsr", "asr", "ror", "rrx"
};

//OPERAND2
typedef struct OPERAND_2_DATA{
    int type;
    int rm;
    ARM_WORD immed;
    int shift_amount;
    int shift_function;
    int rs;
    int error;
    int sanity_number;
} OPERAND_2_DATA;
OPERAND_2_DATA * create_base_operand_2(ARM_WORD raw, int ibit){
    as_log(static_context, "Entering Operand2 Create\n",0);
    OPERAND_2_DATA * data = (OPERAND_2_DATA*)malloc(sizeof(OPERAND_2_DATA));
    data->error = 0;
    if(ibit != 0){
        data->type = 0;
        data->immed = raw & 0xff;
        int rot = (raw & 0xf00) >> 7;
        data->immed = ror(data->immed, rot);
    }
    else if((raw & 0x00000010) == 0){
        data->type = 1;
        data->rm = raw & 0xf;
        data->shift_amount = (raw & 0xF80) >> 7;
        data->shift_function = (raw & 0x60) >> 5;
        if(data->shift_amount == 0 && data->shift_function == 3){
            data->shift_function = 4;
        }
    }
    else if((raw & 0x00000090) == 0x10){
        data->type = 2;
        data->rm = raw & 0x00f;
        data->rs = (raw & 0xf00) >> 8;

        data->shift_function = (raw & 0x60) >> 5;
    }
    else{
        data->error = 1;
    }
    data->sanity_number = INSTRUCTION_SANITY_NUMBER;
    as_log(static_context, "Leaving Operand2 Create\n",0);
    return data;
}

char * operand_2_base_to_string(OPERAND_2_DATA * data){
    as_log(static_context, "Entering Operand2 String\n",0);
    char * logstring = (char*)malloc(sizeof(char) * LOG_STRING_LENGTH);
    if(data->type == 0)
        sprintf(logstring, "#%d",data->immed);
    else if (data->type == 1){
        if(data->shift_amount > 0){
            //return "{0} {1} #{2:d}".format(REG_NAMES[self._rm], _shnam, _shamt)
            sprintf(logstring, "%x, %d", data->rm, data->shift_amount);
        }
        else if(data->type == 3)
            sprintf(logstring, "%x rrx", data->rm);
        else
            sprintf(logstring, "%x", data->rm);
    }
    else
        sprintf(logstring, "%x %d %d", data->rm, data->shift_function, data->shift_amount);

    as_log(static_context, "Leaving Operand2 String\n",0);
    return logstring;
}

ARM_WORD evaluate_base_operand_2(OPERAND_2_DATA * data){
    as_log(static_context, "Entering Operand2 Evaluate\n",0);
    ARM_WORD reg_data;
    if(data->type == 0){
        as_log(static_context, "Leaving Operand2 Evaluate\n",0);
        return data->immed;
    }
    else if (data->type == 1){
        as_log(static_context, "Leaving Operand2 Evaluate\n",0);
        reg_data = get_register(static_context->registers, (ARM_REG)data->rm);
        if(data->shift_function == 0){
            as_log(static_context, "Leaving Operand2 Evaluate\n",0);
            return lsl(reg_data, data->shift_amount);
        }
        else if(data->shift_function == 1){
            as_log(static_context, "Leaving Operand2 Evaluate\n",0);
            return lsr(reg_data, data->shift_amount);
        }
        else if (data->shift_function == 2){
            as_log(static_context, "Leaving Operand2 Evaluate\n",0);
            return asr(reg_data, data->shift_amount);
        }
        else if (data->shift_function == 3){
            as_log(static_context, "Leaving Operand2 Evaluate\n",0);
            return ror(reg_data, data->shift_amount);
        }
        else{
            as_log(static_context, "Leaving Operand2 Evaluate\n",0);
            return reg_data;
        }
    }
    else if(data->type == 2){
        data->shift_amount = get_register(static_context->registers, (ARM_REG)data->rs);
        reg_data = get_register(static_context->registers, (ARM_REG)data->rm);
        if(data->shift_function == 0){
            as_log(static_context, "Leaving Operand2 Evaluate\n",0);
            return lsl(reg_data, data->shift_amount);
        }
        else if(data->shift_function == 1){
            as_log(static_context, "Leaving Operand2 Evaluate\n",0);
            return lsr(reg_data, data->shift_amount);
        }
        else if (data->shift_function == 2){
            as_log(static_context, "Leaving Operand2 Evaluate\n",0);
            return asr(reg_data, data->shift_amount);
        }
        else if (data->shift_function == 3){
            as_log(static_context, "Leaving Operand2 Evaluate\n",0);
            return ror(reg_data, data->shift_amount);
        }
        else{
            as_log(static_context, "Leaving Operand2 Evaluate\n",0);
            return reg_data;
        }
    }
    as_log(static_context, "Leaving Operand2 Evaluate...Badly\n",0);
    return 0;
}

//MSR
typedef struct MSR_DATA{
    int r;
    struct OPERAND_2_DATA * operand2Data;
    unsigned char fields;
} MSR_DATA; //needs freeing
ARMSIM_INSTRUCTION * decode_msr(ARM_ADDRESS address, ARM_WORD raw, int ibit){
    as_log(static_context, "Entering MSR Decode\n",0);
    ARMSIM_INSTRUCTION * instruction = create_base(address, raw);
    MSR_DATA * data = (MSR_DATA*)malloc(sizeof(MSR_DATA));
    instruction->data = data;
    data->operand2Data = create_base_operand_2(raw, ibit);
    data->r = (raw & 0x00400000) == 0x00400000;
    data->fields = (unsigned char)(0xF & (raw >> 16));
    instruction->instruction_type = msr;
    as_log(static_context, "Leaving MSR Decode\n",0);
    return instruction;
}

char * msr_to_string(ARMSIM_INSTRUCTION * instruction){
    as_log(static_context, "Entering MSR String\n",0);
    char * logstring = (char*)malloc(sizeof(char) * LOG_STRING_LENGTH);
    MSR_DATA * data = (MSR_DATA*)instruction->data;
//    _fields = self._fields
//    dest = "spsr" if self._r else "cpsr"
//    fields = ''.join("fsxc"[i] for i in range(4) if _fields[i])
//    src = str(self._op2)
    const char * dest = (data->r)? "spsr" : "cpsr";
    int fields = data->fields;
    char * src = operand_2_base_to_string(data->operand2Data);
    sprintf(logstring, "#010x\t#010x\tmsr%x\t%s_%x, %s", instruction->address, instruction->raw_instruction,
            instruction->condition,dest, fields, src);
    free(src);
    as_log(static_context, "Leaving MSR String\n",0);
    return logstring;
}

void msr_execute(ARMSIM_INSTRUCTION * instruction){
    as_log(static_context, "Entering MSR EXECUTE\n",0);
    MSR_DATA * data = (MSR_DATA*)instruction->data;
    unsigned int mask = 0;
    ARM_WORD new_cpsr, new_mode;
    ARM_WORD evaluated_op_2 = evaluate_base_operand_2(data->operand2Data);
    if(data->fields & 8){
        mask = mask | 0xf0000000;
    }
    if(data->fields & 2){
        mask = mask | 0xdf;
    }
    new_cpsr = (get_cpsr(static_context->registers) & ~mask) | (evaluated_op_2 & mask);
    new_mode = new_cpsr & 0x1f;
    if(new_mode != get_mode(static_context->registers)){
        set_mode_complex(static_context->registers, new_mode, 0);
    }
    set_cpsr(static_context->registers, new_cpsr);
    as_log(static_context, "Leaving MSR EXECUTE\n",0);
}

//DP
typedef struct DP_DATA{
    int opcode;
    int rn;
    int rd;
    OPERAND_2_DATA * data;
} DP_DATA; //needs freeing

void op_and(ARMSIM_INSTRUCTION * instruction){
    as_log(static_context, "Entering OP_AND\n",0);
    DP_DATA * data = (DP_DATA*)instruction->data;
    ARM_WORD value = get_register(static_context->registers, (ARM_REG)data->rn);
    ARM_WORD opValue = evaluate_base_operand_2(data->data);
    value = value & opValue;
    set_register(static_context->registers, (ARM_REG)data->rd, value);
    as_log(static_context, "Leaving  OP_AND\n",0);
}

void op_eor(ARMSIM_INSTRUCTION * instruction){
    as_log(static_context, "Entering OP_EOR\n",0);
    DP_DATA * data = (DP_DATA*)instruction->data;
    ARM_WORD value = get_register(static_context->registers, (ARM_REG)data->rn);
    ARM_WORD opValue = evaluate_base_operand_2(data->data);
    value = value ^ opValue;
    set_register(static_context->registers, (ARM_REG)data->rd, value);
    as_log(static_context, "Leaving OP_EOR\n",0);
}

void op_sub(ARMSIM_INSTRUCTION * instruction){
    as_log(static_context, "Entering OP_SUB\n",0);
    DP_DATA * data = (DP_DATA*)instruction->data;
    ARM_WORD value = get_register(static_context->registers, (ARM_REG)data->rn);
    ARM_WORD opValue = evaluate_base_operand_2(data->data);
    value = value - opValue;
    set_register(static_context->registers, (ARM_REG)data->rd, value);
    as_log(static_context, "Leaving OP_SUB\n",0);
}

void op_rsb(ARMSIM_INSTRUCTION * instruction){
    as_log(static_context, "Entering OP_RSB\n",0);
    DP_DATA * data = (DP_DATA*)instruction->data;
    ARM_WORD value = get_register(static_context->registers, (ARM_REG)data->rn);
    ARM_WORD opValue = evaluate_base_operand_2(data->data);
    value = opValue - value;
    set_register(static_context->registers, (ARM_REG)data->rd, value);
    as_log(static_context, "Leaving OP_RSB\n",0);
}

void op_add(ARMSIM_INSTRUCTION * instruction){
    as_log(static_context, "Entering OP_ADD\n",0);
    DP_DATA * data = (DP_DATA*)instruction->data;
    ARM_WORD value = get_register(static_context->registers, (ARM_REG)data->rn);
    ARM_WORD opValue = evaluate_base_operand_2(data->data);
    value = value + opValue;
    set_register(static_context->registers, (ARM_REG)data->rd, value);
    as_log(static_context, "Leaving OP_ADD\n",0);
}

void op_cmp(ARMSIM_INSTRUCTION * instruction){
    as_log(static_context, "Entering OP_CMP\n",0);
    DP_DATA * data = (DP_DATA*)instruction->data;
    ARM_WORD value = get_register(static_context->registers, (ARM_REG)data->rn);
    ARM_WORD opValue = evaluate_base_operand_2(data->data);

    int diff = value - opValue;
    int z = (diff == 0);
    int n = (diff & 0x80000000) > 0;

    int c = (opValue <= value);
    int v = 0;

    int valueSign = (value & 0x80000000) > 0;
    int opValueSign = (opValue & 0x80000000) > 0;

//    if a_sign and (not b_sign) and (not _N):
//         _V = True
//    elif (not a_sign) and b_sign and _N:
//         _V = True
//    else:
//          _V = False

    if(valueSign != 0 && opValueSign == 0 && n == 0){
        v = 1;
    }
    else if( valueSign == 0 && opValueSign != 0 && n != 0){
        v = 1;
    }

    set_flags(static_context->registers,n,z,c,v);
    as_log(static_context, "Leaving OP_CMP\n",0);
}

void op_orr(ARMSIM_INSTRUCTION * instruction){
    as_log(static_context, "Entering OP_ORR\n",0);
    DP_DATA * data = (DP_DATA*)instruction->data;
    ARM_WORD value = get_register(static_context->registers, (ARM_REG)data->rn);
    ARM_WORD opValue = evaluate_base_operand_2(data->data);
    value = value | opValue;
    set_register(static_context->registers, (ARM_REG)data->rd, value);
    as_log(static_context, "Leaving OP_ORR\n",0);
}

void op_mov(ARMSIM_INSTRUCTION * instruction){
    as_log(static_context, "Entering OP_MOV\n",0);
    DP_DATA * data = (DP_DATA*)instruction->data;
    ARM_WORD  opVal = evaluate_base_operand_2(data->data);
    if(data->rd == 15){
        branch_cpu(static_context, opVal);
        if(instruction->affects_status){
            revert_mode(static_context->registers);
        }
    }
    else{
        set_register(static_context->registers, (ARM_REG)data->rd, opVal);
    }
    as_log(static_context, "Leaving OP_MOV\n",0);
}

void op_bic(ARMSIM_INSTRUCTION * instruction){
    as_log(static_context, "Entering OP_BIC\n",0);
    DP_DATA * data = (DP_DATA*)instruction->data;
    ARM_WORD value = get_register(static_context->registers, (ARM_REG)data->rn);
    ARM_WORD opValue = evaluate_base_operand_2(data->data);
    value = value & ~opValue;
    set_register(static_context->registers, (ARM_REG)data->rd, value);
    as_log(static_context, "Leaving OP_BIC\n",0);
}

void op_mvn(ARMSIM_INSTRUCTION * instruction){
    as_log(static_context, "Entering OP_MVN\n",0);
    DP_DATA * data = (DP_DATA*)instruction->data;
    ARM_WORD value = 0;
    ARM_WORD opValue = evaluate_base_operand_2(data->data);
    value = ~opValue;
    set_register(static_context->registers, (ARM_REG)data->rd, value);
    as_log(static_context, "LEAVING OP_MVN\n",0);
}

ARMSIM_INSTRUCTION * decode_dp(ARM_ADDRESS address, ARM_WORD raw, int ibit){
    as_log(static_context, "Entering DP decode\n",0);
    ARMSIM_INSTRUCTION * instruction = create_base(address, raw);
    DP_DATA * data = (DP_DATA*)malloc(sizeof(DP_DATA));
    instruction->data = data;
    data->opcode = (raw & 0x01E00000) >> 21;
    instruction->affects_status = (raw & 0x00100000) == 0x00100000;
    data->rn = (raw & 0x000F0000) >> 16;
    data->rd = (raw & 0x0000F000) >> 12;
    OPERAND_2_DATA * operand2Data = create_base_operand_2(raw, ibit);
    data->data = operand2Data;
    instruction->instruction_type = data_processing;
    as_log(static_context, "Entering DP decode\n",0);
    return instruction;
}

char * dp_to_string(ARMSIM_INSTRUCTION * instruction){
    as_log(static_context, "Entering DP string\n",0);
    char * logstring = (char*)malloc(sizeof(char) * LOG_STRING_LENGTH);


    as_log(static_context, "Leaving DP string\n",0);
    return logstring;
}

void dp_execute(ARMSIM_INSTRUCTION * instruction){
    as_log(static_context, "Entering DP Execute\n",0);
    DP_DATA * data = (DP_DATA*)instruction->data;
    switch(data->opcode){
        case(0):
            op_and(instruction);
            break;
        case(1):
            op_eor(instruction);
            break;
        case(2):
            op_sub(instruction);
            break;
        case(3):
            op_rsb(instruction);
            break;
        case(4):
            op_add(instruction);
            break;
        case(10):
            op_cmp(instruction);
            break;
        case(12):
            op_orr(instruction);
            break;
        case(13):
            op_mov(instruction);
            break;
        case(14):
            op_bic(instruction);
            break;
        case(15):
            op_mvn(instruction);
            break;
        default:
            ;
            break;
    }
    as_log(static_context, "Leaving DP Execute\n",0);
}

//LS_OPERAND_2
typedef struct LS_OPERAND_2_DATA{
    int neg;
    int nil;
    int offset;
    int sanity_number;
    OPERAND_2_DATA * base;
} LS_OPERAND_2_DATA;

LS_OPERAND_2_DATA * create_ls_operand_2(ARM_WORD raw, int ibit){
    as_log(static_context, "Entering LS_OPERAND_2 create\n",0);
    LS_OPERAND_2_DATA * lsData = (LS_OPERAND_2_DATA*)malloc(sizeof(LS_OPERAND_2_DATA));
    lsData->sanity_number = INSTRUCTION_SANITY_NUMBER;
    lsData->neg = (raw & 0x00800000) == 0;
    if(ibit){
        lsData->base = create_base_operand_2(raw, ibit);
        lsData->base->type = 3;
        lsData->offset = raw & 0xfff;
        lsData->nil = lsData->offset == 0;
    }
    else{
        lsData->base = create_base_operand_2(raw, 0);
        lsData->nil = 0;
    }
    as_log(static_context, "Leaving LS_OPERAND_2 create\n",0);
    return lsData;
}

ARM_WORD evaluate_ls_operand_2(LS_OPERAND_2_DATA * data){
    as_log(static_context, "Entering LS_OPERAND_2 evaluate\n",0);
    OPERAND_2_DATA * operand2Data = data->base;
    as_log(static_context, "1\n",0);
    if(operand2Data == 0 || operand2Data->sanity_number != INSTRUCTION_SANITY_NUMBER){
        as_log(static_context, "No sanity 1 found in LS_OPERAND_2 Evaluate\n",0);
        return 0;
    }
    int type = operand2Data->type;
    as_log(static_context, "2\n",0);
    if(type == 3){
        return (ARM_WORD)data->offset;
    }
    as_log(static_context, "Leaving LS_OPERAND_2 evaluate\n",0);
    return evaluate_base_operand_2(data->base);
}

char * ls_operand_2_to_string(LS_OPERAND_2_DATA * data){
    as_log(static_context, "Entering LS_OPERAND_2 string\n",0);
    as_log(static_context, "Leaving LS_OPERAND_2 string\n",0);
    return 0;//FIX ME
}

//LDSTR
typedef struct LDSTR_DATA{
    int flags;
    int rn;
    int rd;
    LS_OPERAND_2_DATA * data;
} LDSTR_DATA;

ARMSIM_INSTRUCTION * decode_ldstr(ARM_ADDRESS address, ARM_WORD raw, int ibit){
    as_log(static_context, "Entering LDSTR decode\n",0);
    ARMSIM_INSTRUCTION * instruction = create_base(address, raw);
    LDSTR_DATA * data = (LDSTR_DATA*)malloc(sizeof(LDSTR_DATA));
    instruction->data = data;
    data->data = create_ls_operand_2(raw, ibit);
    data->flags = (raw >> 20) & 0x1F;
    data->rn = (raw & 0x000f0000) >> 16;
    data->rd = (raw & 0x0000f000) >> 12;
    instruction->instruction_type = ld_str;
    as_log(static_context, "Leaving LDSTR decode\n",0);
    return instruction;
} //needs freeing

char * ldstr_to_string(ARMSIM_INSTRUCTION * instruction){
    as_log(static_context, "Entering LDSTR String\n",0);
    as_log(static_context, "Leaving LDSTR String\n",0);
    return 0;//FIX ME
}

void ldstr_execute(ARMSIM_INSTRUCTION * instruction){
    as_log(static_context, "Entering LDSTR execute\n",0);
    ARMSIM_REGISTERS * registers = static_context->registers;
    as_log(static_context, "-2\n",0);
    LDSTR_DATA * data = (LDSTR_DATA*)instruction->data;
    as_log(static_context, "-1\n",0);
    int p, u, b, w, l;
    p = data->flags & 0x10;
    u = data->flags & 0x08;
    b = data->flags & 0x04;
    w = data->flags & 0x02;
    l = data->flags & 0x01;
    as_log(static_context, "0\n",0);
    ARM_WORD opvalue = evaluate_ls_operand_2(data->data);
    as_log(static_context, "1\n",0);
    ARM_ADDRESS ea = get_register(registers, (ARM_REG)data->rn);
    if(p){
        ea = ea + (u)?opvalue:(0-opvalue);
    }
    as_log(static_context, "2\n",0);
    if(l){
        as_log(static_context, "3\n",0);
        if(b){
            set_register(registers, (ARM_REG)data->rd, get_byte_from_memory(static_context, ea));
        }
        else{
            set_register(registers, (ARM_REG)data->rd, get_word_from_memory(static_context, ea));
        }
    }
    else{
        as_log(static_context, "4\n",0);
        if(b){
            set_byte_in_memory(static_context, ea, get_register(registers, (ARM_REG)data->rd) & 0xFF);
        }
        else{
            set_word_in_memory(static_context, ea, get_register(registers, (ARM_REG)data->rd));
        }
    }

    if(p && w){
        as_log(static_context, "5\n",0);
        set_register(registers, (ARM_REG)data->rn, ea);
    }
    else if (!p){
        as_log(static_context, "6\n",0);
        ARM_WORD value = get_register(registers, (ARM_REG)data->rn);
        value = value + (u)? opvalue : (0-opvalue);
        set_register(registers, (ARM_REG)data->rn, value);
    }
    as_log(static_context, "7\n",0);
    as_log(static_context, "Leaving LDSTR execute\n",0);
}

//LDSTR_M
typedef struct LDSTR_M_DATA{
    int flags;
    int rn;
    int regs;
} LDSTR_M_DATA;

ARMSIM_INSTRUCTION * decode_ldstr_m(ARM_ADDRESS address, ARM_WORD raw){
    as_log(static_context, "Entering LDSTR_M DECODE\n",0);
    ARMSIM_INSTRUCTION * instruction = create_base(address, raw);
    LDSTR_M_DATA * data = (LDSTR_M_DATA*)malloc(sizeof(LDSTR_M_DATA));
    instruction->data = data;
    instruction->instruction_type = ld_str_multi;
    data->flags = (raw >> 20) & 0x1F;
    data->rn = (raw & 0x000f0000) >> 16;
    data->regs = (raw & 0xffff);
    as_log(static_context, "LEAVING LDSTR_M DECODE\n",0);
    return instruction;
}

char * ldstr_m_to_string(ARMSIM_INSTRUCTION * instruction){
    as_log(static_context, "Entering LDSTR_M STRING\n",0);
    as_log(static_context, "Entering LDSTR_M STRING\n",0);
    return 0;//FIX ME
}

void * ldstr_m_execute(ARMSIM_INSTRUCTION * instruction){
    as_log(static_context, "Entering LDSTR_M execute\n",0);
    LDSTR_M_DATA * data = (LDSTR_M_DATA*)instruction->data;
    ARMSIM_REGISTERS * registers = static_context->registers;
    int p, u, b, w, l;
    p = data->flags & 0x10;
    u = data->flags & 0x08;
    b = data->flags & 0x04;
    w = data->flags & 0x02;
    l = data->flags & 0x01;
    int delta = 0;
    int backwards = 0;
    if(u){
        delta = 4;
        backwards = 0;
    }
    else{
        delta = -4;
        backwards = 1;
    }

    ARM_WORD ea = get_register(registers, (ARM_REG)data->rn) + (p)?(unsigned int)delta : 0;
    //32768 is 2^15
    int regNum = (backwards)? 15 : 0;
    int mask = (backwards)? 32768 : 1;
    int regIncrement = (backwards)? -1 : 1;
    if(l){
        for(; regNum >= 0 && regNum <= 15;regNum += regIncrement, mask = (backwards)? mask / 2 : mask * 2){
            if(data->regs & mask){
                set_register(registers, (ARM_REG)regNum, get_word_from_memory(static_context, ea));
                ea += delta;
            }
        }
    }
    else{
        for(; regNum >= 0 && regNum <= 15;regNum += regIncrement, mask = (backwards)? mask / 2 : mask * 2){
            if(data->regs & mask){
                //set_register(registers, (ARM_REG)regNum, get_word_from_memory(static_context, ea));
                set_word_in_memory(static_context, ea, get_register(registers, (ARM_REG)regNum));
                ea += delta;
            }
        }
    }

    int count = 0;
    mask = 1;
    regNum = 0;
    while(regNum <=15){
        if(data->regs & mask)
            count = count + 1;
        mask = mask * 2;
        regNum = regNum + 1;
    }
    if(w){
        ARM_WORD value = get_register(registers, (ARM_REG)data->rn);
        value = value + delta * count;
        set_register(registers, (ARM_REG)data->rn, value);
    }
    as_log(static_context, "Leaving LDSTR_M execute\n",0);
}

//BRANCH
typedef struct BRANCH_DATA{
    ARM_ADDRESS target;
    int link;
} BRANCH_DATA;

ARMSIM_INSTRUCTION * decode_branch(ARM_ADDRESS address, ARM_WORD raw){
    as_log(static_context, "Entering Branch Decode\n",0);
    ARMSIM_INSTRUCTION * instruction = create_base(address, raw);
    BRANCH_DATA * data = (BRANCH_DATA*)malloc(sizeof(BRANCH_DATA));
    instruction->data = (void*)data;
    instruction->instruction_type = branch;
    int roff, soff;
    roff = (raw & 0x00ffffff);
    soff = (!(roff & 0x00800000))? roff : -((~roff & 0xffffff) + 1);

    data->target = (address + 8) + (soff << 2);
    data->link = ((raw & 0x01000000) == 0x01000000);
    as_log(static_context, "Leaving Branch Decode\n",0);
    return instruction;
}

char * branch_to_string(ARMSIM_INSTRUCTION * instruction){
    as_log(static_context, "Entering Branch string\n",0);
    as_log(static_context, "Leaving Branch string\n",0);
    return 0;//FIX ME
}

void branch_execute(ARMSIM_INSTRUCTION * instruction){
    as_log(static_context, "Entering Branch execute\n",0);
    BRANCH_DATA * data = (BRANCH_DATA*)instruction->data;
    if(data->link){
        set_register(static_context->registers, AR_r14, instruction->address + 4);
    }
    branch_cpu(static_context, data->target);
    as_log(static_context, "Leaving Branch execute\n",0);
}

//SWI
ARMSIM_INSTRUCTION * decode_swi(ARM_ADDRESS address, ARM_WORD raw){
    as_log(static_context, "Entering SWI decode\n",0);
    ARMSIM_INSTRUCTION * instruction = create_base(address, raw);
    instruction->data = (void*)(raw & 0xffffff);
    instruction->instruction_type = swi;
    as_log(static_context, "Leaving SWI decode\n",0);
    return instruction;
}

char * swi_to_string(ARMSIM_INSTRUCTION * instruction){
    as_log(static_context, "Entering SWI STRING\n",0);
    as_log(static_context, "Leaving SWI STRING\n",0);
    return 0; //FIX ME
}

void swi_execute(ARMSIM_INSTRUCTION * instruction){
    as_log(static_context, "Entering SWI execute\n",0);
    ARM_WORD data = (ARM_WORD)instruction->data;
    if(data == 0)
        halt(static_context);
    else if(data == 0xff){
        ; //where i would slow stuff down.... maybe i should include a timer...
    }
    else{
        set_mode(static_context->registers, AM_SVC);
        ARM_WORD cpsr = get_cpsr(static_context->registers);
        set_cpsr(static_context->registers, cpsr | 0x80);

        set_register(static_context->registers, AR_r14, instruction->address + 4);

        branch_cpu(static_context, 0x8);
    }
    as_log(static_context, "Leaving SWI execute\n",0);
}

//PUBLIC FUNCTIONS (DEFINE THESE AT THE BOTTOM TO SO AS TO AVOID FORWARD DECLARATION (for the non-publics...)

char * instruction_to_string(ARMSIM_INSTRUCTION * instruction){
    as_log(static_context, "Entering ITS\n",0);
    switch (instruction->instruction_type) {
        case data_processing:;
            return dp_to_string(instruction);
            break;
        case mul:;
            return mul_to_string(instruction);
            break;
        case ld_str:;
            return ldstr_to_string(instruction);
            break;
        case ld_str_multi:;
            return ldstr_m_to_string(instruction);
            break;
        case branch:;
            return branch_to_string(instruction);
            break;
        case branch_xchg:;
            return branchxchg_to_string(instruction);
            break;
        case swi:;
            return swi_to_string(instruction);
            break;
        case mrs:;
            return mrs_to_string(instruction);
            break;
        case msr:;
            return msr_to_string(instruction);
            break;
        default:
            return base_to_string(instruction);
            break;
    }
    as_log(static_context, "Leaving ITS\n",0);
}

ARMSIM_INSTRUCTION * master_decode(ARM_ADDRESS address, ARM_WORD raw_instruction){
    as_log(static_context, "Entering master decode\n",0);
    if((raw_instruction & 0x0ff000f0) == 0x01200010){
        return decode_branchxchg(address, raw_instruction);
    }

    unsigned int style = (raw_instruction & 0x0E000000) >> 25;
    unsigned int op_s;

    if(style == 0b000){
        if((raw_instruction & 0x90) == 0x90){
            if((raw_instruction & 0x01000020) == 0){
                return decode_mul(address, raw_instruction);
            }
        }
        op_s = raw_instruction & 0x01F00000;
        if(op_s == 0x01000000 || op_s == 0x01400000){
            return decode_mrs(address, raw_instruction);
        }
        else if (op_s == 0x01200000 || op_s == 0x01600000){
            return decode_msr(address, raw_instruction, 0);
        }
        else
            return decode_dp(address, raw_instruction, 0);
    }
    else if(style == 0b001){
        op_s = raw_instruction & 0x01B00000;
        if(op_s == 0x01200000){
            return decode_msr(address, raw_instruction, 1);
        }
        else if (op_s == 0x01000000){
            ;
        }
        else{
            return decode_dp(address, raw_instruction, 1);
        }
    }
    else if(style == 0b010){
        return decode_ldstr(address, raw_instruction, 1);
    }
    else if(style == 0b011){
        return decode_ldstr(address, raw_instruction, 0);
    }
    else if(style == 0b100){
        return decode_ldstr_m(address, raw_instruction);
    }
    else if(style == 0b101){
        return decode_branch(address, raw_instruction);
    }
    else if(style == 0b111){
        if((raw_instruction & 0x01000000) != 0){
            return decode_swi(address, raw_instruction);
        }
    }
    as_log(static_context, "Leaving master_decode\n",0);
    return create_base(address, raw_instruction);
}

void actually_execute_instruction(ARMSIM_INSTRUCTION * instruction) {
    if(instruction!= 0 && instruction->sanity_test != INSTRUCTION_SANITY_NUMBER){
        as_log(static_context, "ACTUAL EXECUTE: Sanity not found\n",0);
    }
    else if(instruction != 0 && instruction->sanity_test == INSTRUCTION_SANITY_NUMBER){
        as_log(static_context, "ACTUAL EXECUTE: Sanity found\n",0);
    }
    else{
        as_log(static_context, "ACTUAL EXECUTE: ZERO INSTRUCTION\n",0);
    }
    as_log(static_context, "Entering actual execute instruction\n",0);
    switch (instruction->instruction_type) {
        case data_processing:;
            dp_execute(instruction);
            break;
        case mul:;
            mul_execute(instruction);
            break;
        case ld_str:;
            ldstr_execute(instruction);
            break;
        case ld_str_multi:;
            ldstr_m_execute(instruction);
            break;
        case branch:;
            branch_execute(instruction);
            break;
        case branch_xchg:;
            branchxchg_execute(instruction);
            break;
        case swi:;
            swi_execute(instruction);
            break;
        case mrs:;
            mrs_execute(instruction);
            break;
        case msr:;
            msr_execute(instruction);
            break;
        default:
            base_execute(instruction);
            break;
    }
    as_log(static_context, "Leaving actual execute instruction\n",0);
}

void free_instruction(ARMSIM_INSTRUCTION * instruction){
    if(instruction!= 0 && instruction->sanity_test != INSTRUCTION_SANITY_NUMBER){
        as_log(static_context, "FREE INSTRUCTION: Sanity not found\n",0);
    }
    else if(instruction != 0 && instruction->sanity_test == INSTRUCTION_SANITY_NUMBER){
        as_log(static_context, "FREE INSTRUCTION: Sanity found\n",0);
    }
    else{
        as_log(static_context, "FREE INSTRUCTION: ZERO INSTRUCTION\n",0);
    }
    //return; //to debug other parts of the program, rather than my (failing) memory management
    as_log(static_context, "Entering free_instruction\n",0);
    switch (instruction->instruction_type) {
        case data_processing:;
            as_log(static_context, "Entering free data_processing\n",0);
            free((((DP_DATA*)instruction->data)->data));
            free((DP_DATA*)instruction->data);
            free(instruction);
            as_log(static_context, "Leaving free data processing\n",0);
            break;
        case mul:;
            as_log(static_context, "Entering free mul\n",0);
            free(instruction->data);
            free(instruction);
            as_log(static_context, "Leaving free mul\n",0);
            break;
        case ld_str:;
            as_log(static_context, "Entering free ld_str\n",0);
            LDSTR_DATA * ldstr_data = (LDSTR_DATA*)instruction->data;
            as_log(static_context, "1\n",0);
            LS_OPERAND_2_DATA * ls_operand_2_data = ldstr_data->data;
            as_log(static_context, "2\n",0);
            OPERAND_2_DATA * operand_2_data = ls_operand_2_data->base;
            as_log(static_context, "3\n",0);
            free(operand_2_data);
            as_log(static_context, "4\n",0);
            free(ls_operand_2_data);
            as_log(static_context, "5\n",0);
            free(ldstr_data);
            as_log(static_context, "6\n",0);
            free(instruction);
            as_log(static_context, "Leaving free ld_str\n",0);
            break;
        case ld_str_multi:;
            as_log(static_context, "Entering free ld_str_m\n",0);
            LDSTR_M_DATA * data4 = (LDSTR_M_DATA*)instruction->data;
            free(data4);
            free(instruction);
            as_log(static_context, "Leaving free ld_str_m\n",0);
            break;
        case branch:;
            as_log(static_context, "Entering free branch\n",0);
            BRANCH_DATA * branch_data = (BRANCH_DATA*)instruction->data;
            free(branch_data);
            free(instruction);
            as_log(static_context, "Leaving free branch\n",0);
            break;
        case branch_xchg:;
            as_log(static_context, "Entering free branchxchg\n",0);
            free(instruction);
            as_log(static_context, "Leaving free branchxchg\n",0);
            break;
        case swi:;
            as_log(static_context, "Entering free swi\n",0);
            free(instruction);
            as_log(static_context, "Leaving free swi\n",0);
            break;
        case mrs:;
            as_log(static_context, "Entering free mrs\n",0);
            MRS_DATA * data = (MRS_DATA*)instruction->data;
            free(data);
            free(instruction);
            as_log(static_context, "Leaving free mrs\n",0);
            break;
        case msr:;
            as_log(static_context, "Entering free msr\n",0);
            MSR_DATA * msr_data = (MSR_DATA*)instruction->data;
            free(data);
            free(instruction);
            as_log(static_context, "Leaving free msr\n",0);
            break;
        default:
            as_log(static_context, "Entering free default\n",0);
            free(instruction);
            as_log(static_context, "Leaving free default\n",0);
            break;
    }
    as_log(static_context, "Leaving free instruction\n",0);
}