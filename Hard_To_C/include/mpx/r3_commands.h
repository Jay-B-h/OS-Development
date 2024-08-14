#ifndef MPX_R3_COMMANDS_H
#define MPX_R3_COMMANDS_H

#include <stddef.h>

/**
 * @name CPU_yield
 * @brief If any processes are in queue, they will execute 
*/
void CPU_yield(void);

/**
 * @name IdleProc
 * @brief 
*/
void idleProc(void);

/**
 * @name load_process
 * @brief Each process is loaded and queued in a non_suspended ready state
*/
void load_process(void);

/**
 * @name load_suspended_process
 * @brief Each process is loaded and queued is suspended ready state 
*/
void load_suspended_process(void);

#endif

