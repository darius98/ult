#pragma once

#if defined(__x86_64__)
#  define ULT_SET_STACK_POINTER(top) asm volatile("mov %[rs], %%rsp \n" : [ rs ] "+r"(top)::)
#else
#  error "Platform not supported"
#endif
