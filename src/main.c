
#include <stdio.h>

#include "common.h"
#include "decompiler.h"
#include "elfparser.h"
#include "ptracer.h"

int main(int argc, const char* argv[], const char* argp[])
{

    elf_instruction_arr ran_args;
    init_elf_instruction_arr(&ran_args);
    if (argc < 2) {
        printf("Usage ./debugger <file>");
        return 1;
    }

    printf("Program dissassembly: \n");
    if (parse_elf(argv[1]) == 0) {
        // instrc_arr and name_arr are extern from elfparser.h
        dissassemble_instr_array(&instrc_arr, &name_arr);
    }

    printf("\nRun program...\n");
    ptracer(&ran_args, argc, argv, argp);

    printf("\nRan instructions:\n");

    dissassemble_instr_array(&ran_args, &name_arr);
}
