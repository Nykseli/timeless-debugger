#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>

#include "decompiler.h"
#include "elfparser.h"
#include "memory.h"

#include "../udis86/udis86.h"

static void add_trace_instruction(elf_instruction_arr* arr, Elf_Off offset, size_t size, uint8_t* bits)
{
    //TODO: find a faster way to make sure we copy only the needed bytes. this is really slow
    const uint8_t* instr = asm_buffer(bits, size);
    uint32_t instr_len = asm_buffer_len(bits, size);
    elf_instruction ei;
    ei.offset = offset;
    ei.size = instr_len;
    ei.bits = malloc(instr_len);
    memmove(ei.bits, instr, ei.size);

    ARRAY_APPEND(arr, ei, elf_instruction, list);
}

static int read_data(int pid, uint64_t addr, uint8_t* mem)
{
    uint64_t* out = (uint64_t*)mem;

    errno = 0;
    uint64_t data = ptrace(PTRACE_PEEKTEXT, pid, addr, NULL);

    if (errno != 0) {
        return -1;
    }

    *out++ = data;
    return 0;
}

static int exec(const char* argv[], const char* argp[])
{
    // must be called in order to allow the
    // control over the child process
    ptrace(0, 0, 0, 0);
    // executes the debugged program and causes
    // the child to stop and send a signal
    // to the parent, the parent can now
    // switch to PTRACE_SINGLESTEP
    return execl(argv[1], argv[1], NULL);
}

static void control(elf_instruction_arr* arr, int pid)
{
    // child's return value
    int wait_val;
    // registers structure, contains regs.rip (instruction address)
    struct user_regs_struct regs;

    wait(&wait_val);

    // the child is finished; wait_val != 1407
    while (WIFSTOPPED(wait_val)) {

        // get address of instruction (regs.rip for 64 bit)
        ptrace(PTRACE_GETREGS, pid, NULL, &regs);

        uint8_t buff[8];
        if (read_data(pid, regs.rip, buff) == -1) {
            printf("(Can't read)\n");
        }

        add_trace_instruction(arr, regs.rip, 16, buff);

        /* Make the child execute another instruction */
        if (ptrace(PTRACE_SINGLESTEP, pid, 0, 0) != 0) {
            perror("ptrace");
        }

        wait(&wait_val);
    }
}

int ptracer(elf_instruction_arr* arr, int argc, const char* argv[], const char* argp[])
{

    int pid = fork(); // child's process id

    switch (pid) {
    case -1:
        perror("fork");
        break;
    case 0:
        if (exec(argv, argp) == -1) {
            fprintf(stderr, "could not execute %s\n", argv[1]);
            return 1;
        }
        break;
    default:
        control(arr, pid);
    }

    return 0;
}
