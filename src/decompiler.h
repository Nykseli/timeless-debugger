#ifndef REALTIME_DEBUG_DECOMPILER_H_
#define REALTIME_DEBUG_DECOMPILER_H_
#include <stddef.h>
#include <stdint.h>

#include "elfparser.h"

void disassemble(uint8_t* buffer, size_t buf_len, int start, name_symbol_arr* nsa);

// translate buffer into parsed asm hex codes
const uint8_t* asm_buffer(uint8_t* buffer, uint64_t size);
uint32_t asm_buffer_len(uint8_t* buffer, uint64_t size);

#endif
