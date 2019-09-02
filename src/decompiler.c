#include <stdio.h>
#include <string.h>

#include "common.h"
#include "elfparser.h"
#include "memory.h"
#include "options.h"

#include "../udis86/udis86.h"

// Definition for 64 or 32 bit elf file type
#ifdef ENV_64BIT
#define UD_MODE 64
#else
#define UD_MODE 32
#endif

static void print_sym_name(Elf_Addr st_val, name_symbol_arr* nsa)
{
    //TODO: is there a faster lookup than O(n)?
    char* name = NULL;
    for (size_t i = 0; i < nsa->size; i++) {
        if (nsa->list[i].st_value == st_val) {
            name = nsa->list[i].name;
        }
    }
    if (name != NULL)
        printf("\n0x%016lx <%s>:\n", st_val, name);
}

/**
 * Dissassemble the elf_instruction bits into single elf_instructions
 * to the target array
 */
static void disassemble(elf_instruction* src, elf_instruction_arr* target, name_symbol_arr* nsa)
{
    ud_t ud_obj;
    ud_init(&ud_obj);
    ud_set_input_buffer(&ud_obj, src->bits, src->size);
    ud_set_mode(&ud_obj, UD_MODE);
    if (option_asm_syntax == 0)
        ud_set_syntax(&ud_obj, UD_SYN_ATT);
    else if (option_asm_syntax == 0)
        ud_set_syntax(&ud_obj, UD_SYN_INTEL);

    while (ud_disassemble(&ud_obj)) {
        elf_instruction tmp_instr;
        const char *hex1, *hex2;
        const char* symbols = ud_insn_asm(&ud_obj);
        size_t sym_size = strlen(symbols);
        size_t offset = src->offset + ud_insn_off(&ud_obj);
        hex1 = ud_insn_hex(&ud_obj);

        tmp_instr.offset = offset;
        tmp_instr.size = ud_insn_len(&ud_obj);
        tmp_instr.symbols = ALLOCATE(char, sym_size + 1);
        memmove((void*)tmp_instr.symbols, (void*)symbols, sym_size);
        tmp_instr.symbols[sym_size] = '\0';
        tmp_instr.bits = ALLOCATE(uint8_t, tmp_instr.size);
        memmove(tmp_instr.bits, hex1, tmp_instr.size);
        ARRAY_APPEND(target, tmp_instr, elf_instruction, list);

        if (!option_verbose_output)
            continue;

        if (nsa != NULL)
            print_sym_name(offset, nsa);
        printf("0x%08lx ", offset);
        size_t hex1_len = strlen(hex1);
        hex2 = hex1 + 16;
        printf("%-16.16s %-24s", hex1, symbols);
        if (hex1_len > 16) {
            printf("\n");
            printf("0x%08lx ", offset + (((hex1_len - strlen(hex2)) / 2)));
            printf("%-16s", hex2);
        }
        printf("\n");
    }
}

/**
 * @brief Dissassemble the elf_instruction_arr list into elf_instructions
 * that only contain single instructions
 */
void dissassemble_instr_array(elf_instruction_arr* arr, name_symbol_arr* nsa)
{
    elf_instruction_arr tmp_arr;
    init_elf_instruction_arr(&tmp_arr);
    for (size_t i = 0; i < arr->size; i++) {
        disassemble(&arr->list[i], &tmp_arr, nsa);
    }
    // free arr and replace it with the dissasemble
    free_elf_instruction_arr(arr);
    arr->capacity = tmp_arr.capacity;
    arr->size = tmp_arr.size;
    arr->list = tmp_arr.list;
}

const uint8_t* asm_buffer(uint8_t* buffer, uint64_t size)
{
    ud_t ud_obj;
    ud_init(&ud_obj);
    ud_set_input_buffer(&ud_obj, buffer, size);
    ud_set_mode(&ud_obj, UD_MODE);
    ud_disassemble(&ud_obj);
    return ud_insn_ptr(&ud_obj);
}

uint32_t asm_buffer_len(uint8_t* buffer, uint64_t size)
{
    ud_t ud_obj;
    ud_init(&ud_obj);
    ud_set_input_buffer(&ud_obj, buffer, size);
    ud_set_mode(&ud_obj, UD_MODE);
    ud_disassemble(&ud_obj);
    return ud_insn_len(&ud_obj);
}
