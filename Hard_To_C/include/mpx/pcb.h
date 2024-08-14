#ifndef MPX_PCB_H
#define MPX_PCB_H

#include <stddef.h>


typedef enum execIndex{
    READY = 1,
    RUNNING = 2,
    BLOCKED = 3
}execIndex;

typedef enum disIndex{
    SUSPENDED = 1,
    ACTIVE = 2
}disIndex;

typedef enum classIndex{
    KERNEL = 0,
    USER = 1
} classIndex;

// pcbStruct declaration
typedef struct pcbStruct{
    char name[12];
    int classType;
    int execution_state;
    int priority;
    int suspendStatus;
    //Changed
    char stack[3072];
    char* stackPtr;
    struct pcbStruct* next;
}pcbStruct;
#include <mpx/queue.h>
// Delete before merge
extern struct Queue ready_Queue;
extern struct Queue blocked_Queue;
extern struct Queue ready_Suspended;
extern struct Queue blocked_Suspended;

/**
 * @name pcb_allocate
 * @brief Allocates memory for PCB
 * @return pcbStruct* Pointer to PCB
*/
pcbStruct* pcb_allocate(void);

/**
 * @name pcb_remove
 * @brief Searches relevant queue and removes given PCB if match found
 * @param pcbStruct* pcb_in pointer to PCB
 * @return int Integer representing success with 0 and failure with 1
*/
int pcb_remove(struct pcbStruct* pcb_in);

/**
 * @name remove_help
 * @param Queue q Queue to search
 * @param pcbStruct* pcb_in pointer to PCB
 * @return int Integer representing success with 0 and failure with 1
*/
int remove_help(struct Queue* q, pcbStruct* pcb_in);

/**
 * @name pcb_free
 * @brief Free allocated memory associated with a PCB. Returns 0 on success, non-zero on error.
*/
int pcb_free(pcbStruct* pcbName);

/**
 * @name pcb_find
 * @param pcbName Name of the PCB to be searched for 
 * @brief Searches all queues for a desigated PCB based on name.
 * @return Pointer to the PCB if found, otherwise returns NULL
*/
pcbStruct* pcb_find(const char* pcbName);

/**
* @name pcb_insert
* @brief Inserts a PCB into the appropriate queue, based on state and priority
* @param pcbStruct* pcb_in Pointer to a PCB to be inserted into designated queue
*/
void pcb_insert(pcbStruct* pcb_in);

/**
 * @name getProcess
 * @brief Returns a pointer to the designated queue based on the input
 * @param type integer value designating which queue the user would like to return a pointer to (1 - Ready, 2 - Blocked, 3 - Ready Suspended, 4 - Blocked Suspended)
 * @return Queue* Returns pointer to the specified queue
 */
struct Queue* getProcess(int type);

/**
 * @brief sets up pcb. assigns the name, class and priority
 * @param name PCB Name (Up to 12 characters)
 * @param class The Class of the PCB (KERNEL or USER)
 * @param priority Priority Level of the PCB (0-9)
 * @return struct pcb* 
 */
struct pcbStruct* pcb_setup(const char* name, int class ,int priority);

/**
 * @name insert_help
 * @brief Iterates through priority queues to insert in proper location
 * @param Queue q Queue to insert into
 * @param pcbStruct* pcb_in PCB to insert
*/
void insert_help(struct Queue* q, pcbStruct* pcb_in);

#endif
