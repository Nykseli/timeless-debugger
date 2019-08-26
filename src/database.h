/**
 * Simple database for storing the program information in memory
 */
#ifndef REALTIME_DEBUG_DATABASE_H_
#define REALTIME_DEBUG_DATABASE_H_

#include "elfparser.h"

void init_database();
void free_database();

/**
 * Contains the info about the excecute binary
 */
typedef struct db_program_info_t {
    name_symbol_arr* name_symbols;
    elf_instruction_arr* instrucitons;
} db_program_info;

void db_set_program_info_name_symbols(name_symbol_arr* name_symbols);
name_symbol_arr* db_get_program_info_name_symbols();
void db_set_program_info_instruction_array(elf_instruction_arr* instr_arr);
elf_instruction_arr* db_get_program_info_instruction_array();

/**
 * Contains the data of the executed program
 */
typedef struct db_program_excute_t {
    elf_instruction_arr* instrucitons;
} db_program_execute;

void db_set_program_execute_instruction_array(elf_instruction_arr* instr_arr);
elf_instruction_arr* db_get_program_execute_instruction_array();

#endif
