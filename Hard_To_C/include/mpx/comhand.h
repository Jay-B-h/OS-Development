#ifndef MPX_COMHAND_H
#define MPX_COMHAND_H

#include <stddef.h>

extern struct MCB_Queue memory_list;

/*
@name comhand
@brief Outputs a menu with a list of commands. Depending on user input it executes certain commands 
@return N/A
*/
void comhand( void );

/**
 * @name shutdown_command
 * @brief Prompts the user to confirm if they want to shutdown the system. If confirmed, the system will be shutdown. Otherwise, user will be returned to main menu.
 * @return Integer to determine whether or not the system should be shutdown
*/
int shutdown_command( void );

/**
 * @name help_command
 * @brief Prints the available commands menu with a brief description of each function to the terminal
 * @return N/A
*/
void help_command( void );

/**
 * @name gett_command
 * @brief Function to print the currently set time to the terminal
 * @return N/A
*/
void gett_command( void );

/**
 * @name getd_command
 * @brief Prints the currently set date from the RTC to the terminal
 * @return N/A
*/
void getd_command( void );

/**
 * @name sett_command
 * @brief Prompts user to enter values for hour, minute, and second to set the RTC registers
 * @return N/A
*/
void sett_command( void );

/**
 * @name setd_command
 * @brief Prompt user to enter value for day, month, and year and set the RTC date registers
 * @return N/A
*/
void setd_command( void );

/**
 * @name version_command
 * @brief Prints the current version of the MPX system and the compliation date to the terminal
 * @return N/A
*/
void version_command( void );

/**
 * @name itoa
 * @brief Converts integer values to ascii
 * @param value Integer to convert to ascii
 * @param strArray Array to store the converted value in
 * @return Converted integer value in ascii
*/
char* itoa(int value, char* strArray);

/**
 * @name titleCard
 * @brief Prints the teams logo to the terminal
 * @return N/A
*/
void titleCard(void);

/**
 * @name deletePCB
 * @brief Locates a process, deletes it from the queue and frees all associated memory
 * @return N/A
*/
void deletePCB(void);

/**
 * @name blockPCB
 * @brief Puts a process in the blocked state and moves it into the blocked queue
 * @return N/A
*/
//void blockPCB(void);

/**
 * @name unblockPCB
 * @brief Puts a process into the unblocked state and moves it into the unblocked queue
 * @return N/A
*/
//void unblockPCB(void);
/**
 * @name suspendPCB
 * @brief Puts the process in the suspended state and moves it to the correct queue
 * @return N/A
*/
void suspendPCB(void);

/**
 * @name resumePCB
 * @brief Puts the process in the non suspended state and moves it to hte correct queue
 * @return N/A
*/
void resumePCB(void);

/**
 * @name setPcbPriority
 * @brief Changes the priority level of a process and moves it to the correct queue
 * @return N/A
*/
void setPcbPriority(void);

/**
 * @name pcbMenu
 * @brief Displays another menu with options for all the pcb commands 
 * @return N/A
*/
void pcbMenu(void);

/**
  * @name pcbShow
  * @brief Displays the appropriate process control block based on user's input.
  * @param int type Terminates the appropriate command. type 0: show specific pcb, type 1: show ready pcb, type 2: show blocked pcb, type 3: show all pcb.
  * @return N/A
*/
void pcbShow(int type);

/**
 * @name pcbMenu_help_command
 * @brief Prints the available PCB commands menu with a brief description of each function to the terminal
 * @return N/A
*/
void pcbMenu_help_command(void);

/**
 * @name allocate
 * @brief Allocates heap memory and prints the address of the newly allocated block
*/
void allocate( void );

/**
 * @name free
 * @brief Frees heap memory 
*/
void free( void );

/**
 * @name show_allocated_memory
 * @brief Prints all the information from each block of allocated memory
 * @param int address: The start address of the block
 * @param size_t size: The size of the block
*/
void show_allocated_memory(int address, size_t size);

/**
 * @name show_free_memory
 * @brief Prints all the information from each block of free memory
 * @param int address: The start address of the block
 * @param size_t size: The size of the block
*/
void show_free_memory(int address, size_t size);


/**
 * @name show_mem
 * @brief shows allocated/free memory details as requested by user
 * @param func 2 for allocated memory and 1 for free memory. Follows memIndex
 */
void show_mem(int func);

/**
 * @name init_Heap
 * @brief User function to initalize the heap to the input size
 */
void init_Heap( void );

#endif
