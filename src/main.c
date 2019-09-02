
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "common.h"
#include "database.h"
#include "decompiler.h"
#include "elfparser.h"
#include "options.h"
#include "ptracer.h"

// By default we don't need verbose output
int option_verbose_output = 0;
int option_asm_syntax = 0;

void print_usage()
{
    printf("usage: debugger [-options] excutable [executable options]\n");
    printf("options:\n");
    printf("\t-h\t\tprint this\n");
    printf("\t-s syntax\tset asm syntax. att for att, intel for intel. default: a\n");
    printf("\t-v\t\tverbose output\n");
}

void argument_error(char c)
{
    printf("invalid argument for option -%c. -h for usage\n", c);
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
        case 's': {
            int next = optind + 1;
            if (argv[next][0] == '-') {
                argument_error(argv[optind][1]);
                return 1;
            } else if (strcmp(argv[next], "intel") == 0) {
                option_asm_syntax = 1;
                optind++;
            }
            else if (strcmp(argv[next], "att") == 0) {
                option_asm_syntax = 0;
                optind++;
            }else{
                argument_error(argv[optind][1]);
                return 1;
            }
        }
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
}
