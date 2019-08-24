
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
        for (size_t i = 0; i < instrc_arr.size; i++) {
            disassemble(instrc_arr.list[i].bits, instrc_arr.list[i].size, instrc_arr.list[i].offset, &name_arr);
        }
    }

    printf("\nRun program...\n");
    ptracer(&ran_args, argc, argv, argp);

    printf("\nRan instructions:\n");

    for (size_t i = 0; i < ran_args.size; i++) {
        disassemble(ran_args.list[i].bits, ran_args.list[i].size, ran_args.list[i].offset, &name_arr);
    }
}
