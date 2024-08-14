#include <mpx/io.h>
#include <mpx/serial.h>
#include <sys_req.h>
#include <string.h>
#include <mpx/sys_call.h>
#include <mpx/pcb.h>
#include <mpx/interrupts.h>

// Global pcb pointer for currently executing process
pcbStruct *curPCB = NULL;

// Pointer representing the context from the first time sys_call() is entered 
context *firstContext = NULL; 

/**
 * @name sys_call
 * @brief Calls the process to be loaded 
 * @param struct context *currContext Represents the context of the current process
 * @return Pointer to the context of the process to be loaded 
*/
struct context *sys_call(struct context *currContext) {

	// call IO Completes
	// CHECK: added event flag check
	if (eventFlag == YES){
		io_completion();
	}

	// checks event flag to see if the current process is done

	// TODO: R6 NEED TO UPDATE CONDITIONAL
	//  && (currContext->eax != READ) && (currContext->eax != WRITE)
	if ((currContext->eax != IDLE) && (currContext->eax != EXIT) && (currContext->eax != READ) && (currContext->eax != WRITE)) {
		
		// Set the current context's eax to -1
		currContext->eax = -1;

		// Return the current context
		return currContext;
	}


	if (currContext->eax == IDLE) {

		//Checks if first is NULL, if not sets first to the current process' context
		if (firstContext == NULL) {
			firstContext = currContext;
		}

		// Dequeue the first PCB from the ready queue and store in tempPCB
		pcbStruct *tempPCB = dequeue(&ready_Queue);

		// if PCBs in ready queue, remove the first one and store it in a temp variable
		if (tempPCB != NULL)
		{
			//currently running process
			if (curPCB != NULL) {

				//set the execution state
				curPCB->execution_state = READY;

				// Set the curPCB's stack pointer to the current context
				curPCB->stackPtr = (char*)currContext;

				// insert PCB into ready queue
				pcb_insert(curPCB);

			}
			
			//update the curPCB to the new process
			curPCB = tempPCB;

			// return value seen by sys_req should be 0
			currContext->eax = 0;	 

			//update execution state to running
			curPCB->execution_state = RUNNING;

			//return context of new process
			return (context*)curPCB->stackPtr;
			

		} else {

			// else, if ready queue is empty, continue with current running process
			return currContext;
		}
	}

	if (currContext->eax == EXIT) {

		// delete currently running PCB and free its allocated memory
		pcb_remove(curPCB);	
		pcb_free(curPCB);

		// Dequeue from the ready queue and store PCB in curPCB
		curPCB = dequeue(&ready_Queue);

		// if PCBs in ready queue, load first one in
		if (curPCB != NULL) {

			//update execution state
			curPCB->execution_state = RUNNING;

			//return context of new process
			return (context*)curPCB->stackPtr;

		} else {

			// return value seen by sys_req should be 0
			currContext->eax = 0;

			// else if ready queue is empty, load original context
			return firstContext;
		}
	}

	// R6 NEED TO DO 
	if (currContext->eax == READ || currContext->eax == WRITE) {
		
		// CHECK: Change to device
		/*device dev = currContext->ebx;
		char* buf = (char*) currContext->ecx;
		size_t size = (size_t) currContext->edx;*/

		// request must be scheduled by I/O scheduler

		/*	if (firstContext == NULL) {
				firstContext = currContext;
			}*/
			
			// Dequeue the first PCB from the ready queue and store in tempPCB
			pcbStruct *tempPCB = dequeue(&ready_Queue);

					if (curPCB != NULL){
					//set the execution state
					curPCB->execution_state = BLOCKED;

					// Set the curPCB's stack pointer to the current context
					curPCB->stackPtr = (char*)currContext;

					// insert PCB into blocked queue
					pcb_insert(curPCB);
					}

					if (currContext->eax == READ) {
						io_scheduler(currContext->ebx, curPCB, READ, (char*)currContext->ecx, currContext->edx);
					}

					if (currContext->eax == WRITE) {
						io_scheduler(currContext->ebx, curPCB, WRITE, (char*)currContext->ecx, currContext->edx);
					}

			// if PCBs in ready queue, remove the first one and store it in a temp variable
			if (tempPCB != NULL)
			{
				//update the curPCB to the new process
				curPCB = tempPCB;

				// return value seen by sys_req should be 0
				currContext->eax = 0;	 

				//update execution state to running
				curPCB->execution_state = RUNNING;



				//return context of new process
				return (context*)curPCB->stackPtr; // TODO: NEED TO FIX

			}

		


	}
	

	// Return the current context
	return currContext;
}
