
#include <stdio.h>

#include "common.h"
#include "decompiler.h"
#include "elfparser.h"

int main(int argc, const char* argv[])
{

    if (argc != 2) {
        printf("Usage ./debugger <file>");
        return 1;
    }

    if (parse_elf(argv[1]) == 0) {
        // instrc_arr and name_arr are extern from elfparser.h
        for (size_t i = 0; i < instrc_arr.size; i++) {
            disassemble(instrc_arr.list[i].bits, instrc_arr.list[i].size, instrc_arr.list[i].offset, &name_arr);
        }
    }
}
