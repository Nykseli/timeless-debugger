#include <stdio.h>
#include <string.h>

#include "common.h"

#include "../udis86/udis86.h"

// Definition for 64 or 32 bit elf file type
#ifdef ENV_64BIT
#define UD_MODE 64
#else
#define UD_MODE 32
#endif

void disassemble(uint8_t* buffer, size_t buf_len, int start)
{
    ud_t ud_obj;
    ud_init(&ud_obj);
    ud_set_input_buffer(&ud_obj, buffer, buf_len);
    ud_set_mode(&ud_obj, UD_MODE);
    ud_set_syntax(&ud_obj, UD_SYN_ATT);

    while (ud_disassemble(&ud_obj)) {
        size_t offset = start + ud_insn_off(&ud_obj);
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
