#ifndef DEBUGGER_ELFPARSER_H_
#define DEBUGGER_ELFPARSER_H_
#include <elf.h>

#ifdef ENV_64BIT
// E_MASHINE is used to check the elf file instuction set
#define E_MASHINE EM_X86_64
// ELFCLASS is used to check the 64 or 32 bit file type
#define ELFCLASS ELFCLASS64

typedef Elf64_Ehdr Elf_Ehdr;
typedef Elf64_Phdr Elf_Phdr;
typedef Elf64_Shdr Elf_Shdr;
typedef Elf64_Sym Elf_Sym;

typedef Elf64_Half Elf_Half;
typedef Elf64_Off Elf_Off;
typedef Elf64_Addr Elf_Addr;
typedef Elf64_Word Elf_Word;
typedef Elf64_Sword Elf_Sword;

#elif defined(ENV_32BIT)
// E_MASHINE is used to check the elf file instuction set
#define E_MASHINE EM_386
// ELFCLASS is used to check the 64 or 32 bit file type
#define ELFCLASS ELFCLASS32

typedef Elf32_Ehdr Elf_Ehdr;
typedef Elf32_Phdr Elf_Phdr;
typedef Elf32_Shdr Elf_Shdr;
typedef Elf32_Sym Elf_Sym;

typedef Elf32_Half Elf_Half;
typedef Elf32_Off Elf_Off;
typedef Elf32_Addr Elf_Addr;
typedef Elf32_Word Elf_Word;
typedef Elf32_Sword Elf_Sword;

#endif

typedef struct elf_instruction_t {
    Elf_Off offset;
    size_t size;
    uint8_t* bits;
} elf_instruction;

typedef struct elf_instruction_arr_t {
    size_t size;
    elf_instruction* list;
} elf_instruction_arr;

typedef struct name_symbol_t {
    Elf_Addr st_value;
    size_t size;
    char* name;
} name_symbol;

typedef struct name_symbol_arr_t {
    size_t size;
    name_symbol* list;
} name_symbol_arr;

#endif
