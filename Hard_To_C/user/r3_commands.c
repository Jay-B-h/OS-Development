#include <mpx/io.h>
#include <mpx/serial.h>
#include <sys_req.h>
#include <string.h>
#include <mpx/r3_commands.h>
#include <mpx/pcb.h>
#include <mpx/sys_call.h>
#include <processes.h>

/**
 * @name CPU_yield
 * @brief If any processes are in queue, they will execute 
*/
void CPU_yield(void) {
    sys_req(IDLE);
}


/**
 * @name IdleProc
 * @brief 
*/
void idleProc(void) {
    
    // CHECK: Remove Idle Proc Print
    // char mess[] = "Idling...";
    // sys_req(WRITE, COM1, mess, sizeof(mess));
    
    sys_req(IDLE);
}

/**
 * @name load_process
 * @brief Each process is loaded and queued in a non_suspended ready state
*/
void load_process(void) {

    //Process 1
    char name1[12] = "Process 1";
    pcbStruct* process1 = pcb_setup(name1,1,1);

    // Check to see if process has already been 
    // Do not load if the process exists
    if (process1 != NULL){
    process1->suspendStatus = ACTIVE;
    process1->execution_state = READY;

    // Set the context of process 1 to the corresponding PCBs stack pointer
    context* cont1 = (context*)(process1->stackPtr);
    memset(cont1, 0, sizeof(context));

    // Set context register values
    cont1->cs = 0x08;

    cont1->ds = 0x10;
    cont1->es = 0x10;
    cont1->fs = 0x10;       
    cont1->gs = 0x10;
    cont1->ss = 0x10;
    
    cont1->ebp = (int)process1->stack;                
    cont1->eip = (int) proc1;

    cont1->eflags = 0x0202;

    cont1->eax = 0x00;
    cont1->ebx = 0x00;
    cont1->ecx = 0x00;
    cont1->edx = 0x00;
    cont1->esi = 0x00;
    cont1->edi = 0x00;

    //Insert process into into queue
    pcb_insert(process1);

    // END OF PROCESS 1
    }

    //Process 2
    char name2[12] = "Process 2";
    pcbStruct* process2 = pcb_setup(name2,1,1);

    // Check to see if process has already been 
    // Do not load if the process exists
    if (process2 != NULL){
    process2->suspendStatus = ACTIVE;
    process2->execution_state = READY;

    // Set the context of process 2 to the corresponding PCBs stack pointer
    context* cont2 = (context*)(process2->stackPtr);
    memset(cont2, 0, sizeof(context));

    // Set context register values
    cont2->cs = 0x08;

    cont2->ds = 0x10;
    cont2->es = 0x10;
    cont2->fs = 0x10;
    cont2->gs = 0x10;
    cont2->ss = 0x10;

    cont2->ebp = (int)process2->stack;                  
    cont2->eip = (int) proc2;

    cont2->eflags = 0x0202;

    cont2->eax = 0x00;
    cont2->ebx = 0x00;
    cont2->ecx = 0x00;
    cont2->edx = 0x00;
    cont2->esi = 0x00;
    cont2->edi = 0x00;

    //Insert process into into queue
    pcb_insert(process2);

    // END OF PROCESS 2
    }

    //Process 3
    char name3[12] = "Process 3";
    pcbStruct* process3 = pcb_setup(name3,1,1);
    
    // Check to see if process has already been 
    // Do not load if the process exists
    if (process3 != NULL){
    process3->suspendStatus = ACTIVE;
    process3->execution_state = READY;

    // Set the context of process 3 to the corresponding PCBs stack pointer
    context* cont3 = (context*)(process3->stackPtr);
    memset(cont3, 0, sizeof(context));

    // Set context register values
    cont3->cs = 0x08;

    cont3->ds = 0x10;
    cont3->es = 0x10;
    cont3->fs = 0x10;
    cont3->gs = 0x10;
    cont3->ss = 0x10;
    
    cont3->ebp = (int)process3->stack;                  
    cont3->eip = (int) proc3; 

    cont3->eflags = 0x0202;

    cont3->eax = 0x00;
    cont3->ebx = 0x00;
    cont3->ecx = 0x00;
    cont3->edx = 0x00;
    cont3->esi = 0x00;
    cont3->edi = 0x00;

    //Insert process into into queue
    pcb_insert(process3);

    // END OF PROCESS 3
    }

    //Process 4
    char name4[12] = "Process 4";
    pcbStruct* process4 = pcb_setup(name4,1,1);

    // Check to see if process has already been 
    // Do not load if the process exists
    if (process4 != NULL){
    process4->suspendStatus = ACTIVE;
    process4->execution_state = READY;

    // Set the context of process 4 to the corresponding PCBs stack pointer
    context* cont4 = (context*)(process4->stackPtr);
    memset(cont4, 0, sizeof(context));

    // Set context register values
    cont4->cs = 0x08;

    cont4->ds = 0x10;
    cont4->es = 0x10;
    cont4->fs = 0x10;
    cont4->gs = 0x10;
    cont4->ss = 0x10;
    
    cont4->ebp = (int)process4->stack;                  
    cont4->eip = (int) proc4;

    cont4->eflags = 0x0202;

    cont4->eax = 0x00;
    cont4->ebx = 0x00;
    cont4->ecx = 0x00;
    cont4->edx = 0x00;
    cont4->esi = 0x00;
    cont4->edi = 0x00;

    //Insert process into into queue
    pcb_insert(process4);

    // END OF PROCESS 4
    }

    //Process 5
    char name5[12] = "Process 5";
    pcbStruct* process5 = pcb_setup(name5,1,3);

    // Check to see if process has already been 
    // Do not load if the process exists
    if (process5 != NULL){
    process5->suspendStatus = ACTIVE;
    process5->execution_state = READY;

    // Set the context of process 5 to the corresponding PCBs stack pointer
    context* cont5 = (context*)(process5->stackPtr);
    memset(cont5, 0, sizeof(context));


    // Set context register values
    cont5->cs = 0x08;

    cont5->ds = 0x10;
    cont5->es = 0x10;
    cont5->fs = 0x10;
    cont5->gs = 0x10;
    cont5->ss = 0x10;
    
    cont5->ebp = (int)process5->stack;                   
    cont5->eip = (int) proc5;

    cont5->eflags = 0x0202;

    cont5->eax = 0x00;
    cont5->ebx = 0x00;
    cont5->ecx = 0x00;
    cont5->edx = 0x00;
    cont5->esi = 0x00;
    cont5->edi = 0x00;

    //Insert process into into queue
    pcb_insert(process5);

    // END OF PROCESS 5
    }


    

}


/**
 * @name load_suspended_process
 * @brief Each process is loaded and queued in a suspended ready state 
*/
void load_suspended_process(void) {
    
    //Process 1
    char name1[12] = "Process 1";
    pcbStruct* process1 = pcb_setup(name1,1,1);

    // Check to see if process has already been 
    // Do not load if the process exists
    if (process1 != NULL){
    process1->suspendStatus = SUSPENDED;
    process1->execution_state = READY;

    // Set the context of process 1 to the corresponding PCBs stack pointer
    context* cont1 = (context*)(process1->stackPtr);
    memset(cont1, 0, sizeof(context));

    // Set context register values
    cont1->cs = 0x08;

    cont1->ds = 0x10;
    cont1->es = 0x10;
    cont1->fs = 0x10;       
    cont1->gs = 0x10;
    cont1->ss = 0x10;
    
    cont1->ebp = (int)process1->stack;                
    cont1->eip = (int) proc1;

    cont1->eflags = 0x0202;

    cont1->eax = 0x00;
    cont1->ebx = 0x00;
    cont1->ecx = 0x00;
    cont1->edx = 0x00;
    cont1->esi = 0x00;
    cont1->edi = 0x00;

    //Insert process into into queue
    pcb_insert(process1);

    // END OF PROCESS 1
    }

    //Process 2
    char name2[12] = "Process 2";
    pcbStruct* process2 = pcb_setup(name2,1,1);

    // Check to see if process has already been 
    // Do not load if the process exists
    if (process2 != NULL){
    process2->suspendStatus = SUSPENDED;
    process2->execution_state = READY;

    // Set the context of process 2 to the corresponding PCBs stack pointer
    context* cont2 = (context*)(process2->stackPtr);
    memset(cont2, 0, sizeof(context));

    // Set context register values
    cont2->cs = 0x08;

    cont2->ds = 0x10;
    cont2->es = 0x10;
    cont2->fs = 0x10;
    cont2->gs = 0x10;
    cont2->ss = 0x10;

    cont2->ebp = (int)process2->stack;                  
    cont2->eip = (int) proc2;

    cont2->eflags = 0x0202;

    cont2->eax = 0x00;
    cont2->ebx = 0x00;
    cont2->ecx = 0x00;
    cont2->edx = 0x00;
    cont2->esi = 0x00;
    cont2->edi = 0x00;

    //Insert process into into queue
    pcb_insert(process2);

    // END OF PROCESS 2
    }

    //Process 3
    char name3[12] = "Process 3";
    pcbStruct* process3 = pcb_setup(name3,1,1);
    
    // Check to see if process has already been 
    // Do not load if the process exists
    if (process3 != NULL){
    process3->suspendStatus = SUSPENDED;
    process3->execution_state = READY;

    // Set the context of process 3 to the corresponding PCBs stack pointer
    context* cont3 = (context*)(process3->stackPtr);
    memset(cont3, 0, sizeof(context));

    // Set context register values
    cont3->cs = 0x08;

    cont3->ds = 0x10;
    cont3->es = 0x10;
    cont3->fs = 0x10;
    cont3->gs = 0x10;
    cont3->ss = 0x10;
    
    cont3->ebp = (int)process3->stack;                  
    cont3->eip = (int) proc3; 

    cont3->eflags = 0x0202;

    cont3->eax = 0x00;
    cont3->ebx = 0x00;
    cont3->ecx = 0x00;
    cont3->edx = 0x00;
    cont3->esi = 0x00;
    cont3->edi = 0x00;

    //Insert process into into queue
    pcb_insert(process3);

    // END OF PROCESS 3
    }

    //Process 4
    char name4[12] = "Process 4";
    pcbStruct* process4 = pcb_setup(name4,1,1);

    // Check to see if process has already been 
    // Do not load if the process exists
    if (process4 != NULL){
    process4->suspendStatus = SUSPENDED;
    process4->execution_state = READY;

    // Set the context of process 4 to the corresponding PCBs stack pointer
    context* cont4 = (context*)(process4->stackPtr);
    memset(cont4, 0, sizeof(context));

    // Set context register values
    cont4->cs = 0x08;

    cont4->ds = 0x10;
    cont4->es = 0x10;
    cont4->fs = 0x10;
    cont4->gs = 0x10;
    cont4->ss = 0x10;
    
    cont4->ebp = (int)process4->stack;                  
    cont4->eip = (int) proc4;

    cont4->eflags = 0x0202;

    cont4->eax = 0x00;
    cont4->ebx = 0x00;
    cont4->ecx = 0x00;
    cont4->edx = 0x00;
    cont4->esi = 0x00;
    cont4->edi = 0x00;

    //Insert process into into queue
    pcb_insert(process4);

    // END OF PROCESS 4
    }

    //Process 5
    char name5[12] = "Process 5";
    pcbStruct* process5 = pcb_setup(name5,1,3);

    // Check to see if process has already been 
    // Do not load if the process exists
    if (process5 != NULL){
    process5->suspendStatus = SUSPENDED;
    process5->execution_state = READY;

    // Set the context of process 5 to the corresponding PCBs stack pointer
    context* cont5 = (context*)(process5->stackPtr);
    memset(cont5, 0, sizeof(context));


    // Set context register values
    cont5->cs = 0x08;

    cont5->ds = 0x10;
    cont5->es = 0x10;
    cont5->fs = 0x10;
    cont5->gs = 0x10;
    cont5->ss = 0x10;
    
    cont5->ebp = (int)process5->stack;                   
    cont5->eip = (int) proc5;

    cont5->eflags = 0x0202;

    cont5->eax = 0x00;
    cont5->ebx = 0x00;
    cont5->ecx = 0x00;
    cont5->edx = 0x00;
    cont5->esi = 0x00;
    cont5->edi = 0x00;

    //Insert process into into queue
    pcb_insert(process5);

    // END OF PROCESS 5
    }

}
