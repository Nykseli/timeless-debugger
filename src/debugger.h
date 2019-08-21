#ifndef REALTIME_DEBUG_DEBUGGER_H_
#define REALTIME_DEBUG_DEBUGGER_H_

#include <stdio.h>

#ifndef DEBUG_LEVEL
#define DEBUG(...) ;
#else
#define DEBUG(args...) fprintf(stderr, args)
#endif

#endif
