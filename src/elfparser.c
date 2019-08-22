
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "common.h"
#include "debugger.h"
#include "decompiler.h"
#include "elfparser.h"

int instruction_virtual_addr = 0;
int fn_index = 0; // hodls the index for the symtable st_shndx witch contains the function / section names
elf_instruction_arr instrc_arr;
name_symbol_arr name_arr;

void init_elf_instruction_arr(elf_instruction_arr* i_arr)
{
    i_arr->size = 0;
    i_arr->list = NULL;
}

void add_instruction(uint8_t* bits, size_t size, Elf_Off offset)
{
    elf_instruction ei;
    ei.size = size;
    ei.offset = offset;
    ei.bits = malloc(size);
    memmove(ei.bits, bits, size);

    instrc_arr.list = realloc(instrc_arr.list, sizeof(elf_instruction) * (instrc_arr.size + 1));
    instrc_arr.list[instrc_arr.size] = ei;
    instrc_arr.size++;
}

void init_name_symbol_arr(name_symbol_arr* n_arr)
{
    n_arr->size = 0;
    n_arr->list = NULL;
}

void add_name_symbol(char* name, Elf_Addr st_value)
{
    name_symbol ns;
    ns.size = strlen(name);
    ns.st_value = st_value;
    ns.name = malloc(ns.size);
    memmove(ns.name, name, ns.size);

    name_arr.list = realloc(name_arr.list, sizeof(name_symbol) * (name_arr.size + 1));
    name_arr.list[name_arr.size] = ns;
    name_arr.size++;
}

int parse_elf(const char* file)
{

    int fd = open(file, O_RDONLY);
    if (fd < 0) {
        printf("error: file %s doesn't exists\n", file);
        return 1;
    }
    init_elf_instruction_arr(&instrc_arr);
    //
    struct stat fs;
    fstat(fd, &fs);
    // TODO: munmap later
    void* file_bytes = mmap(NULL, fs.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    Elf_Ehdr elf_ehdr;
    memmove(&elf_ehdr, file_bytes, sizeof(elf_ehdr));
    close(fd);

    // A lot faster way to compare the header than memcmp
    // See https://godbolt.org/z/z16qtz
    uint32_t elf_expectedheader = 0x464c457f;
    if (*(uint32_t*)&elf_ehdr.e_ident != elf_expectedheader) {
        printf("File is not an ELF executable\n");
        return 1;
    }

    if (elf_ehdr.e_ident[EI_CLASS] != ELFCLASS) {
        printf("ELF type is not supported\n");
        return 1;
    }
    if (elf_ehdr.e_machine != E_MASHINE) {
        printf("Instruction set is not supported\n");
        return 1;
    }

    for (int i = 0; i < elf_ehdr.e_phnum; i++) {
        size_t offset = elf_ehdr.e_phoff + (i * elf_ehdr.e_phentsize);
        Elf_Phdr elf_phdr;
        memmove(&elf_phdr, file_bytes + offset, elf_ehdr.e_phentsize);

        if (elf_phdr.p_type == PT_LOAD && elf_phdr.p_flags == PF_R + PF_X)
            instruction_virtual_addr = elf_phdr.p_vaddr;
    }
    size_t dynstr_off = 0;
    size_t dynsym_off = 0;
    size_t dynsym_sz = 0;

    for (int i = 0; i < elf_ehdr.e_shnum; i++) {
        size_t offset = elf_ehdr.e_shoff + i * elf_ehdr.e_shentsize;
        Elf_Shdr elf_shdr;
        memmove(&elf_shdr, file_bytes + offset, elf_ehdr.e_shentsize);
        switch (elf_shdr.sh_type) {
        case SHT_PROGBITS:
            if (elf_shdr.sh_flags == 6) {
                add_instruction(file_bytes + elf_shdr.sh_offset, elf_shdr.sh_size, instruction_virtual_addr | elf_shdr.sh_offset);
                fn_index = i;
            }
            break;
        case SHT_STRTAB: {
            if (dynstr_off == 0)
                dynstr_off = elf_shdr.sh_offset;

        } break;
        case SHT_SYMTAB: {
            dynsym_off = elf_shdr.sh_offset;
            dynsym_sz = elf_shdr.sh_size;
        } break;
        default:
            break;
        }
    }

    for (size_t j = 0; j * sizeof(Elf_Sym) < dynsym_sz; j++) {
        Elf_Sym sym;
        size_t absoffset = dynsym_off + j * sizeof(Elf_Sym);
        memmove(&sym, file_bytes + absoffset, sizeof(sym));
        if (sym.st_shndx == fn_index && sym.st_name != 0)
            add_name_symbol((char*)file_bytes + dynstr_off + sym.st_name, sym.st_value);
    }

    return 0;
}
