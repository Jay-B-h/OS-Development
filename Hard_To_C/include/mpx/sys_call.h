#ifndef MPX_SYS_CALL_H
#define MPX_SYS_CALL_H

#include <stddef.h>

// Context struct
typedef struct context{
    int ds;
    int es;
    int fs;
    int gs;
    int ss;
    int eax;
    int ebx;
    int ecx;
    int edx;
    int esi;
    int edi;
    int ebp;
    int eip;
    int cs;
    int eflags;
}context;

/**
 * @name sys_call
 * @brief Calls the process to be loaded 
 * @param struct context curr* Represents the context of the current process
 * @return Pointer to the context of the process to be loaded 
*/
struct context *sys_call(struct context *curr);

#endif

