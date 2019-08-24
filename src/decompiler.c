#include <stdio.h>
#include <string.h>

#include "common.h"
#include "elfparser.h"

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

void disassemble(uint8_t* buffer, size_t buf_len, int start, name_symbol_arr* nsa)
{
    ud_t ud_obj;
    ud_init(&ud_obj);
    ud_set_input_buffer(&ud_obj, buffer, buf_len);
    ud_set_mode(&ud_obj, UD_MODE);
    ud_set_syntax(&ud_obj, UD_SYN_ATT);

    while (ud_disassemble(&ud_obj)) {
        size_t offset = start + ud_insn_off(&ud_obj);
        if(nsa != NULL)
            print_sym_name(offset, nsa);
        printf("0x%08lx ", offset);
        const char *hex1, *hex2;
        hex1 = ud_insn_hex(&ud_obj);
        size_t hex1_len = strlen(hex1);
        hex2 = hex1 + 16;
        printf("%-16.16s %-24s", hex1, ud_insn_asm(&ud_obj));
        if (hex1_len > 16) {
            printf("\n");
            printf("0x%08lx ", offset + (((hex1_len - strlen(hex2)) / 2)));
            printf("%-16s", hex2);
        }
        printf("\n");
    }
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
