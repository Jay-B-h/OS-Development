#include <mpx/gdt.h>
#include <mpx/interrupts.h>
#include <mpx/serial.h>
#include <mpx/vm.h>
#include <sys_req.h>
#include <string.h>
#include <memory.h>
#include <mpx/comhand.h>
#include <mpx/rtc.h>
#include <mpx/io.h>
#include <mpx/pcb.h>
#include <mpx/queue.h>
#include <sys_req.h>
#include <mpx/sys_call.h>

// Make sure we can use string.h
#include <string.h>

pcbStruct* pcb_allocate(void);

// Fix Declaration
struct Queue ready_Queue = {.front = NULL, .rear = NULL};
struct Queue blocked_Queue = {.front = NULL, .rear = NULL};
struct Queue ready_Suspended = {.front = NULL, .rear = NULL};
struct Queue blocked_Suspended = {.front = NULL, .rear = NULL};


/**
 * @name pcb_allocate
 * @brief Allocates memory for PCB
 * @return pcbStruct* Pointer to PCB
*/
pcbStruct* pcb_allocate(void) {

    // allocate memory for pcb
    pcbStruct *pcb =  sys_alloc_mem(sizeof(pcbStruct));

    if (pcb == NULL) {
        printf("Failed to allocate memory for PCB",sizeof(char[34]));

        return NULL;
    }

    // initalizes stack to be empty
    for(int i = 0; i < 3072; i++){
        pcb->stack[i] = 0;
    }

    // sets stack pointer to starting location
    pcb->stackPtr = (char*)(pcb->stack + 3070) - sizeof(context);


    return pcb;
} 


/**
 * @name pcb_remove
 * @brief Searches relevant queue and removes given PCB if match found
 * @param pcbStruct* pcb_in pointer to PCB
 * @return int Integer representing success with 0 and failure with 1
*/
int pcb_remove(struct pcbStruct* pcb_in) {

    // Conditional statements find which queue to search and passes along to helper function
    if (pcb_in->suspendStatus == ACTIVE) {
        if (pcb_in->execution_state == READY) {
            return remove_help(&ready_Queue, pcb_in);
        } else if (pcb_in->execution_state == BLOCKED) {
            return remove_help(&blocked_Queue, pcb_in);
        } else {
            return 1;
        }
    } else if (pcb_in->suspendStatus == SUSPENDED) {
        if (pcb_in->execution_state == READY) {
            return remove_help(&ready_Suspended, pcb_in);
        } else if (pcb_in->execution_state == BLOCKED) {
            return remove_help(&blocked_Suspended, pcb_in);
        } else {
            return 1;
        }
    }

    return 1;
}


/**
 * @name remove_help
 * @param Queue q Queue to search
 * @param pcbStruct* pcb_in pointer to PCB
 * @return int Integer representing success with 0 and failure with 1
*/
int remove_help(struct Queue* q, pcbStruct* pcb_in) {
    
    // return if queue is empty
    if (q->front == NULL) {
        return 1;
    }

    // removes pcb from front of queue and updates front pointer
    if (q->front == pcb_in) {
        if (q->front->next != NULL) {
            q->front = q->front->next;
        } else {
            q->front = NULL;
        }
        return 0;
    }

    // Node used to iterate through queue
    pcbStruct *curPCB = q->front;

    // iterate through queue until end is reached or match is found
    while (curPCB != NULL) {
        if (curPCB->next == pcb_in) {
            if (curPCB->next->next != NULL) {
                curPCB->next = curPCB->next->next;
                return 0;
            }
            else {
                curPCB->next = NULL;
                return 0;
            }
        }

        curPCB = curPCB->next;
    }

    return 1;
}


/**
 * @name pcb_free
 * @brief Free allocated memory associated with a PCB. Returns 0 on success, non-zero on error.
*/
int pcb_free(pcbStruct* pcbName){

    // Return code signifying success or failure
    return sys_free_mem(&pcbName);

}

/**
 * @name pcb_find
 * @param pcbName Name of the PCB to be searched for 
 * @brief Searches all queues for a desigated PCB based on name.
 * @return Pointer to the PCB if found, otherwise returns NULL
*/
pcbStruct* pcb_find(const char* pcbName){

    int foundFlag = 0;

    pcbStruct* currPCB;

    // Ready Queue
    currPCB = ready_Queue.front;
    while (currPCB != NULL){

        // If pcbName is equal to the current PCBs name
        // Return pointer to current PCB
        if (strcmp(currPCB->name,pcbName) == 0){
            return currPCB;
        }

        currPCB = currPCB->next;

    }

    // Blocked Queue
    currPCB = blocked_Queue.front;
    while ((currPCB != NULL) && (foundFlag == 0)){

        // If pcbName is equal to the current PCBs name
        // Return pointer to current PCB
        if (strcmp(currPCB->name,pcbName) == 0){
            return currPCB;
        }

        currPCB = currPCB->next;

    }

    // Ready Suspended
    currPCB = ready_Suspended.front;
    while ((currPCB != NULL) && (foundFlag == 0)){

        // If pcbName is equal to the current PCBs name
        // Return pointer to current PCB
        if (strcmp(currPCB->name,pcbName) == 0){
            return currPCB;
        }

        currPCB = currPCB->next;

    }

    // Blocked suspended
    currPCB = blocked_Suspended.front;
    while ((currPCB != NULL) && (foundFlag == 0)){

        // If pcbName is equal to the current PCBs name
        // Return pointer to current PCB
        if (strcmp(currPCB->name,pcbName) == 0){
            return currPCB;
        }

        currPCB = currPCB->next;

    }

    // Return Null if not found
    return currPCB;

}

/**
* @name pcb_insert
* @brief Inserts a PCB into the appropriate queue, based on state and priority
* @param pcbStruct* pcb_in Pointer to a PCB to be inserted into designated queue
*/
void pcb_insert(pcbStruct* pcb_in) {

    //checks suspend status is active
    if (pcb_in->suspendStatus == ACTIVE) {
        //checks if execution state is ready
        if (pcb_in->execution_state == READY) {
            insert_help(&ready_Queue, pcb_in);
        //checks if execution state is blocked
        } else if (pcb_in->execution_state == BLOCKED) {
            
            //enqueue blocked PCB
            enqueue(&blocked_Queue, pcb_in);

        } else {
            //error message
            char error_msg1[] = "\nInvalid PCB\n";

            //display error message if PCB's suspend status is active but execution state is not ready or blocked
            sys_req(WRITE, COM1, error_msg1, sizeof(error_msg1));
        }

    //checks if suspend status is suspended
    } else if (pcb_in->suspendStatus == SUSPENDED) {
        //checks if execution state is ready 
        if (pcb_in->execution_state == READY) {
            insert_help(&ready_Suspended, pcb_in);
        //checks if execution state is blocked
        } else if (pcb_in->execution_state == BLOCKED) {
            //enqueue blocked suspended PCB
            enqueue(&blocked_Suspended, pcb_in);

        } else {
            //error message
            char error_msg2[] = "\nInvalid PCB\n";

            //display error message if PCB's suspend status is active but execution state is not ready or blocked
            sys_req(WRITE, COM1, error_msg2, sizeof(error_msg2));
        }
    }

}

/**
* @name insert_help
* @brief Helper function to pcb_insert. Assists in placing nodes in the proper position within the queue.
* @param Queue* q Queue for the passed pcb to be placed in to
* @param pcbStruct* pcb_in Pointer to a PCB to be inserted into designated queue
*/
void insert_help(struct Queue* q, pcbStruct* pcb_in) {

    pcbStruct* curPCB = q->front;

    //might save our lives
    pcb_in-> next = NULL;

    // if queue is empty, add incoming pcb to front of queue
    if (q->front == NULL) {
        q->front = pcb_in;
        q->rear = pcb_in;

        return;
    }

    while(curPCB != NULL) {
        

        // if pcb is higher priority than head of queue, set to head
        if (pcb_in->priority < q->front->priority) {
            pcb_in->next = q->front;
            q->front = pcb_in;

            break;
        }

        // if end of queue is reached, insert pcb at the end
        if (curPCB->next == NULL) {
            curPCB->next = pcb_in;

            break;
        }

        // inserts by priority
        if (pcb_in->priority < curPCB->next->priority) {
                
            pcb_in->next = curPCB->next;

            curPCB->next = pcb_in;

            break;
        }
        //traverse to the next node in the list
        curPCB = curPCB->next;
    }

    return;
}

/**
 * @brief sets up pcb. assigns the name, class and priority
 * @param name PCB Name (Up to 12 characters)
 * @param class The Class of the PCB (KERNEL or USER)
 * @param priority Priority Level of the PCB (0-9)
 * @return struct pcb* 
 */
struct pcbStruct* pcb_setup(const char name[], int class ,int priority){

// Changed max_length to 12
// If the process fails validation print error message
if(pcb_find(name) != NULL || strlen(name)>12 || priority < 0 || priority >9 || (class != 1 && class != 0)){
    
    char error_msg2[] = "\n";
    sys_req(WRITE, COM1, error_msg2, sizeof(error_msg2));

    sys_req(WRITE, COM1, name, 12);

    char error_msg1[] = " failed validation check! Process will not be loaded!\n";
    sys_req(WRITE, COM1, error_msg1, sizeof(error_msg1));

    return NULL;
}

//call pcb_allocate
pcbStruct* newPcb = pcb_allocate();
newPcb->next = NULL;

if(newPcb == NULL){

    char error_msg1[] = "\nPCB NULL\n";
    sys_req(WRITE, COM1, error_msg1, sizeof(error_msg1));

    return NULL;
}

// Added another parameter
memcpy(newPcb->name,name,sizeof(newPcb->name));

// Set the class and priority of the PCB
newPcb->classType = class; 
newPcb->priority = priority;

// By default set to ACTIVE and READY
newPcb->suspendStatus = ACTIVE;
newPcb->execution_state = READY;

return newPcb;

}//end pcb_setup

/**
 * @name getProcess
 * @brief Returns a pointer to the designated queue based on the input
 * @param type integer value designating which queue the user would like to return a pointer to (1 - Ready, 2 - Blocked, 3 - Ready Suspended, 4 - Blocked Suspended)
 * @return Queue* Returns pointer to the specified queue
 */
Queue* getProcess(int type){
    
    // Reuturn pointer to ready queue
    if(type == 1){
        return &ready_Queue;
    }

    // Return pointer to blocked queue
    if (type == 2){
        return &blocked_Queue;
    }

    // Return pointer to ready suspended queue
    if (type == 3){
        return &ready_Suspended;
    }

    // Return pointer to blocked suspended queue
    if(type == 4){
        return &blocked_Suspended;
    }
    return NULL;
    
}//end getProcess
