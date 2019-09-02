/**
 * Options are defined in main.c.
 * option variables should always start with option_
 */
#ifndef TIMELESS_DEBUG_OPTIONS_H_
#define TIMELESS_DEBUG_OPTIONS_H_

// Option used for controlling program stdout printing
extern int option_verbose_output;
// Option for assembly syntax, 0 is att, 1 is intel
// Defaults to 0
extern int option_asm_syntax;

#endif
