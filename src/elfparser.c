
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

int main(int argc, const char* argv[])
{

    if (argc != 2) {
        printf("Usage ./debugger <file>");
        return 1;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        printf("error: file %s doesn't exists\n", argv[1]);
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

    DEBUG("Program header offset: %d\n", elf_ehdr.e_phoff);
    DEBUG("Program header num: %d\n", elf_ehdr.e_phnum);
    DEBUG("Section header num: %d\n", elf_ehdr.e_shnum);
    DEBUG("Section names index: %d\n\n", elf_ehdr.e_shstrndx);

    for (int i = 0; i < elf_ehdr.e_phnum; i++) {
        size_t offset = elf_ehdr.e_phoff + (i * elf_ehdr.e_phentsize);
        DEBUG("adder: %d * %d = %d\n", i, elf_ehdr.e_phentsize, i * elf_ehdr.e_phentsize);
        Elf_Phdr elf_phdr;
        memmove(&elf_phdr, file_bytes + offset, elf_ehdr.e_phentsize);
        DEBUG("Program header %d, offset: %08x\n", i, offset);
        DEBUG("-------------------------------------------\n");
        DEBUG("p_type: ");
        switch (elf_phdr.p_type) {
        case PT_NULL:
            DEBUG("PT_NULL\n");
            break;
        case PT_LOAD:
            DEBUG("PT_LOAD\n");
            break;
        case PT_DYNAMIC:
            DEBUG("PT_DYNAMIC\n");
            break;
        case PT_INTERP:
            DEBUG("PT_INTERP\n");
            break;
        case PT_NOTE:
            DEBUG("PT_NOTE\n");
            break;
        case PT_SHLIB:
            DEBUG("PT_SHLIB\n");
            break;
        case PT_PHDR:
            DEBUG("PT_PHDR\n");
            break;
        case PT_LOOS:
            DEBUG("PT_LOOS\n");
            break;
        case PT_HIOS:
            DEBUG("PT_HIOS\n");
            break;
        case PT_LOPROC:
            DEBUG("PT_LOPROC\n");
            break;
        case PT_HIPROC:
            DEBUG("PT_HIPROC\n");
            break;
        case PT_GNU_STACK:
            DEBUG("PT_GNU_STACK\n");
            break;
        default:
            DEBUG("UNKNOWN\n");
            break;
        }

        DEBUG("p_offset: %zd\n", elf_phdr.p_offset);
        DEBUG("p_vaddr: %08x\n", elf_phdr.p_vaddr);
        DEBUG("p_paddr: %08x\n", elf_phdr.p_paddr);
        DEBUG("p_filesz: %zd\n", elf_phdr.p_filesz);
        DEBUG("p_memsz: %zd\n", elf_phdr.p_memsz);
        DEBUG("p_flags: %zd\n", elf_phdr.p_flags);
        DEBUG("p_align: %zd\n\n", elf_phdr.p_align);

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
        DEBUG("Section header %d, offset: %08x\n", i, offset);
        DEBUG("-------------------------------------------\n");
        if (elf_shdr.sh_type == SHT_NULL) {
            DEBUG("sh_type: NULL\n\n");
            continue;
        }
        DEBUG("sh_name: %d\n", elf_shdr.sh_name);
        DEBUG("sh_type: ");
        switch (elf_shdr.sh_type) {
        case SHT_PROGBITS:
            DEBUG("SHT_PROGBITS\n");
            if ((elf_shdr.sh_flags & 0x20) == SHF_STRINGS) {
                DEBUG("SHF_STRINGs\n");
            }
            for (int j = 0; j < elf_shdr.sh_size; j++)
                DEBUG("0x%02x,", ((uint8_t*)file_bytes + elf_shdr.sh_offset)[j]);
            DEBUG("\n");
            DEBUG("sh string: %s\n", (char*)file_bytes + elf_shdr.sh_offset);
            if (elf_shdr.sh_flags == 6) {
                add_instruction(file_bytes + elf_shdr.sh_offset, elf_shdr.sh_size, instruction_virtual_addr | elf_shdr.sh_offset);
                fn_index = i;
            }
            break;
        case SHT_STRTAB: {
            DEBUG("SHT_STRTAB\n");
            DEBUG("sh string: %s\n", (char*)file_bytes + elf_shdr.sh_offset + 1);
            if (dynstr_off == 0)
                dynstr_off = elf_shdr.sh_offset;

        } break;
        case SHT_SYMTAB: {
            DEBUG("SHT_SYMTAB\n");
            dynsym_off = elf_shdr.sh_offset;
            dynsym_sz = elf_shdr.sh_size;
        } break;
        case SHT_NOTE:
            DEBUG("SHT_NOTE\n");
            break;
        default:
            DEBUG("UNKNOWN: 0x%x\n", elf_shdr.sh_type);
            break;
        }
        DEBUG("sh_flags: %08x\n", elf_shdr.sh_flags);
        DEBUG("sh_offset: %08x\n", elf_shdr.sh_offset);
        DEBUG("\n");
    }

    DEBUG("SHT_OFFSET: %d\n", dynstr_off);
    for (size_t j = 0; j * sizeof(Elf_Sym) < dynsym_sz; j++) {
        Elf_Sym sym;
        size_t absoffset = dynsym_off + j * sizeof(Elf_Sym);
        memmove(&sym, file_bytes + absoffset, sizeof(sym));
        DEBUG("SYMBOL TABLE ENTRY %zd\n", j);
        DEBUG("st_name: %d", sym.st_name);
        if (sym.st_name != 0) {
            DEBUG(" (%s)", (char*)file_bytes + dynstr_off + sym.st_name);
        }
        DEBUG("\n");
        DEBUG("st_info: %d\n", sym.st_info);
        DEBUG("st_other: %d\n", sym.st_other);
        DEBUG("st_shndx: %d\n", sym.st_shndx);
        DEBUG("st_value: %p\n", (void*)sym.st_value);
        DEBUG("st_size: %zd\n", sym.st_size);
        DEBUG("\n");
        if (sym.st_shndx == fn_index && sym.st_name != 0)
            add_name_symbol((char*)file_bytes + dynstr_off + sym.st_name, sym.st_value);
    }
    DEBUG("\n");

    for (int i = 0; i < instrc_arr.size; i++) {
        disassemble(instrc_arr.list[i].bits, instrc_arr.list[i].size, instrc_arr.list[i].offset, &name_arr);
    }

    return 0;
}
