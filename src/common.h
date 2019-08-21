#ifndef REALTIME_DEBUG_COMMON_H_
#define REALTIME_DEBUG_COMMON_H_

#if defined(__x86_64__)
#define ENV_64BIT
#else
#error "Currently only x86_64 compiler is supported"
#endif

#endif
