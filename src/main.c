
#include <stdio.h>
#include <unistd.h>

#include "common.h"
#include "database.h"
#include "decompiler.h"
#include "elfparser.h"
#include "options.h"
#include "ptracer.h"
#include "restapi.h"

// By default we don't need verbose output
int option_verbose_output = 0;

void print_usage()
{
    printf("\
usage: debugger [-options] excutable [executable options]\n\
options:\n\
\t-h\tprint this\n\
\t-v\tverbose output\n");
}

int main(int argc, const char* argv[], const char* argp[])
{
    // optind defines the last index of the debugger options
    // so after optind is the executable and it's options
    int optind = 0;
    for (optind = 1; optind < argc && argv[optind][0] == '-'; optind++) {
        switch (argv[optind][1]) {
        case 'h':
            print_usage();
            return 0;
            break;
        case 'v':
            option_verbose_output = 1;
            break;
        default:
            printf("invalid option %c\n", argv[optind][1]);
            print_usage();
            return 1;
            break;
        }
    }

    elf_instruction_arr ran_args;
    init_elf_instruction_arr(&ran_args);
    if (argc < 2) {
        print_usage();
        return 1;
    }

    init_database();

    if (option_verbose_output)
        printf("Program dissassembly: \n");
    if (parse_elf(argv[optind]) == 0) {
        // instrc_arr and name_arr are extern from elfparser.h
        dissassemble_instr_array(&instrc_arr, &name_arr);
        db_set_program_info_instruction_array(&instrc_arr);
        db_set_program_info_name_symbols(&name_arr);
    }

    printf("\nRun program...\n");
    ptracer(&ran_args, argc - optind, argv + optind, argp);

    if (option_verbose_output)
        printf("\nRan instructions:\n");

    dissassemble_instr_array(&ran_args, &name_arr);
    db_set_program_execute_instruction_array(&ran_args);

    restapi_main(NULL);

}
