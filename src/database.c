#include <stddef.h>

#include "database.h"

static volatile db_program_info program_info;
static volatile db_program_execute program_execute;

void init_database()
{
    // init program_info
    program_info.name_symbols = NULL;
    program_info.instrucitons = NULL;

    // init program_execute
    program_execute.instrucitons = NULL;
}

void free_database()
{
    //TODO:
}

// ----- db_program_info start ------
void db_set_program_info_name_symbols(name_symbol_arr* name_symbols)
{
    program_info.name_symbols = name_symbols;
}

name_symbol_arr* db_get_program_info_name_symbols()
{
    return program_info.name_symbols;
}

void db_set_program_info_instruction_array(elf_instruction_arr* instr_arr)
{
    program_info.instrucitons = instr_arr;
}

elf_instruction_arr* db_get_program_info_instruction_array()
{
    return program_info.instrucitons;
}
// ----- db_program_info end --------

// ----- db_program_execute start ------
void db_set_program_execute_instruction_array(elf_instruction_arr* instr_arr)
{
    program_execute.instrucitons = instr_arr;
}

elf_instruction_arr* db_get_program_execute_instruction_array()
{
    return program_execute.instrucitons;
}
// ----- db_program_execute end --------
