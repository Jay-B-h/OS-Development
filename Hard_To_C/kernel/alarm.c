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
#include <mpx/alarm.h>
#include <mpx/r3_commands.h>

struct alarm_Queue alarm_queue = {.front = NULL, .rear = NULL};

/**
 * @name create_alarm
 * @brief Creates a user defined alarm
 * @return void
*/
void create_alarm(void) {
    const char name[] = "alarm_pcb";

    // allocate memory for new alarm
    alarmStruct *new_alarm = sys_alloc_mem(sizeof(alarmStruct));
    if (new_alarm == NULL) {
        printf("Failed to allocate memory for alarm",sizeof(char[36]));

        return;
    }

    // set date and time for alarm struct
    set_alarm_time(new_alarm);
    
    // prompt user to enter alarm message
    printf("\nEnter an alarm message: ", sizeof(char[26]));

    char message[50] = {0};
    sys_req(READ, COM1, message, sizeof(message));

    // Added another parameter
    memcpy(new_alarm->message,message,sizeof(new_alarm->message));

    // enqueue alarm 
    enqueue_alarm(&alarm_queue, new_alarm);

    pcbStruct* alarm_pcb = pcb_find(name);

    // if pcb doesn't exist, create one
    if (alarm_pcb == NULL) {
        alarm_pcb = pcb_setup(name, 0, 8);

        alarm_pcb->suspendStatus = ACTIVE;
        alarm_pcb->execution_state = READY;

        // Set the context of process 1 to the corresponding PCBs stack pointer
        context* cont1 = (context*)(alarm_pcb->stackPtr);
        memset(cont1, 0, sizeof(context));

        // Set context register values
        cont1->cs = 0x08;

        cont1->ds = 0x10;
        cont1->es = 0x10;
        cont1->fs = 0x10;       
        cont1->gs = 0x10;
        cont1->ss = 0x10;
        
        cont1->ebp = (int)alarm_pcb->stack;                
        cont1->eip = (int) check_alarm;

        cont1->eflags = 0x0202;

        cont1->eax = 0x00;
        cont1->ebx = 0x00;
        cont1->ecx = 0x00;
        cont1->edx = 0x00;
        cont1->esi = 0x00;
        cont1->edi = 0x00;
        pcb_insert(alarm_pcb);
    }

    return;

}


/**
 * @name check_alarm
 * @brief Checks list of alarms to determine if a given alarms set time has passed. Prints off alarms message and removes them from queue.
 * @return void
*/
void check_alarm(void) {

    while(1) {
        //Check to see if there are any alarms in the queue
        if (alarm_queue.front == NULL) {
            sys_req(EXIT);
            return;
        }

        //Creates variables for the date or time
        timeStruct currentTime = getTime();
        dateStruct currentDate = getDate();

        //Set temp
        alarmStruct* temp = alarm_queue.front;
        while (temp != NULL) {
            //Check to see if the current date and time has passed the alarm's date and time 
            if (temp->date.year == currentDate.year) {

                if (temp->date.month == currentDate.month) {

                    if (temp->date.day == currentDate.day) {

                        if (temp->time.hour == currentTime.hour) {

                            if (temp->time.minute == currentTime.minute) {
                                
                                if (temp->time.second == currentTime.second) {
                                    printf(temp->message, sizeof(char[50]));
                                    //Delete alarm
                                    dequeue_alarm(&alarm_queue, temp);
                                    sys_free_mem(&temp);

                                } else if (temp->time.second < currentTime.second) {
                                    printf(temp->message, sizeof(char[50]));
                                    //Delete alarm
                                    dequeue_alarm(&alarm_queue, temp);
                                    sys_free_mem(&temp);
                                }

                            } else if (temp->time.minute < currentTime.minute) {
                                printf(temp->message, sizeof(char[50]));
                                //Delete alarm
                                dequeue_alarm(&alarm_queue, temp);
                                sys_free_mem(&temp);
                            }

                        } else if (temp->time.hour < currentTime.hour) {
                            printf(temp->message, sizeof(char[50]));
                            //Delete alarm
                            dequeue_alarm(&alarm_queue, temp);
                            sys_free_mem(&temp);
                        }

                    } else if (temp->date.day < currentDate.day) {
                        printf(temp->message, sizeof(char[50]));
                        //Delete alarm
                        dequeue_alarm(&alarm_queue, temp);
                        sys_free_mem(&temp);
                    }

                } else if (temp->date.month < currentDate.month) {
                    printf(temp->message, sizeof(char[50]));
                    //Delete alarm
                    dequeue_alarm(&alarm_queue, temp);
                    sys_free_mem(&temp);
                }

            } else if (temp->date.year < currentDate.year) {
                printf(temp->message, sizeof(char[50]));
                //Delete alarm
                dequeue_alarm(&alarm_queue, temp);
                sys_free_mem(&temp);
            }

            temp = temp->next;

        } //end of while

        //If the queue is empty after traversing exit the process, if not idle the process
        if (alarm_queue.front == NULL) {
            sys_req(EXIT);
        } else {
            sys_req(IDLE);
        }
    }


   return;
}

/**
 * @name set_alarm_time
 * @brief Prompts user for the date and time to store in a alarm struct
 * @param alarmStruct *passed_alarm: Struct to hold values for the date and time 
*/
void set_alarm_time(alarmStruct *passed_alarm) {

    // Variable declaration
    int day = -1;
    int month = -1;
    int year = -1;

    // Input and validate month
    while (1){ 

    // Prompt user to enter month
    printf("\n\nPlease input the month (MM): ",sizeof(char[32]));
    
    // Store entry in monthBuf
    char monthBuf[5] = { 0 };
    sys_req(READ, COM1, monthBuf, sizeof(monthBuf));
    printf("\n\n", sizeof(char[3]));

    //Yield CPU before prompting for and reading user input
    CPU_yield();

    // Convert entry to integer
    month = atoi(monthBuf);

    // Input validation
    if ((month >= 1) && (month <= 12)){
        
        // Valid entry 
        break;
    }
    
    // Otherwise, print invalid entry statement
    printf("\nInput invalid!\n",sizeof(char[17]));

    }

    // Input and validate day
    while(1){     

    // Prompt user to enter day
    printf("\n\nPlease input the day (DD): ",sizeof(char[30]));

    // Store entry in dayBuf
    char dayBuf[5] = { 0 };
    sys_req(READ, COM1, dayBuf, sizeof(dayBuf));
    printf("\n\n", sizeof(char[3]));

    //Yield CPU before prompting for and reading user input
    CPU_yield();

    // Convert entry to integer
    day = atoi(dayBuf);

    // Input validation 31 day months 
    if ((month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12) && (day > 0) && (day <= 31)){
        
        break;

    }

    // Input validation 30 day months
    if ((month == 4 || month == 6 || month == 9 || month == 11) && (day > 0) && (day <= 30)){
        
        break;

    }

    // Input validaition Febuary
    if ((month == 2) && (day > 0) && (day <= 28)){

        break;

    }    
    
    // Otherwise, print invalid entry statement
    printf("\nInput invalid!\n",sizeof(char[17]));

    }

    // Input and validate year
    while(1){
    
    // Prompt user to enter the year
    printf("\n\nPlease input the year (YY): ",sizeof(char[31]));
    
    // Store input in yearBuf
    char yearBuf[5] = { 0 };
    sys_req(READ, COM1, yearBuf, sizeof(yearBuf));
    printf("\n\n", sizeof(char[3]));


    //Yield CPU before prompting for and reading user input
    CPU_yield();

    // Convert yearBuf value to integer
    year = atoi(yearBuf);

    // Input validation
    if ((year >= 0) && (year <= 99)){
        
        // Valid input
        break;
    }
    
    // Otherwise, print invalid entry statement
    printf("\nInput invalid!\n",sizeof(char[17]));

    }

    // Assign variables values
    passed_alarm->date.day = day;
    passed_alarm->date.month = month;
    passed_alarm->date.year = year;

    // Instantiate values
    int hours = -1;
    int minutes = -1;
    int seconds = -1;

    // Input and validate hour
    while (1){     

    // Prompt user to enter the hour
    printf("\nPlease input the hour (HH): ",sizeof(char[31]));

    // Write to the hour buffer
    char hourBuf[5] = { 0 };
    sys_req(READ, COM1, hourBuf, sizeof(hourBuf));
    printf("\n\n", sizeof(char[3]));
    
    //Yield CPU after reading user input
    CPU_yield();

    // Convert hour buffer to an integer
    hours = atoi(hourBuf);  

    // Validate entry
    if ((hours >= 0) && (hours < 24)){
        
        //Valid Entry
        break;
    }
    
    // Otherwise, print invalid entry
    printf("\nInput invalid!",sizeof(char[16]));

    }

    // Input and validate minute
    while(1){     

    // Prompt user to enter minute
    printf("\n\nPlease input the minute (MM): ",sizeof(char[33]));
    
    // Store entry in minuteBuf
    char minuteBuf[5] = { 0 };
    sys_req(READ, COM1, minuteBuf, sizeof(minuteBuf));
    printf("\n\n", sizeof(char[3]));

    //Yield CPU after reading user input
    CPU_yield();

    // Convert entry to intger
    minutes = atoi(minuteBuf);

    // Input validation
    if ((minutes >= 0) && (minutes < 60)){
        
        // Valid input
        break;
    }
    
    // Otherwise, print invalid entry statement
    printf("\nInput invalid!\n",sizeof(char[17]));

    }

    // Input and validate second
    while(1) {

    // Prompt user to input seconds
    printf("\n\nPlease input the second (SS): ",sizeof(char[33]));
    
    // Store entry in secondsBuf
    char secondsBuf[5] = { 0 };
    sys_req(READ, COM1, secondsBuf, sizeof(secondsBuf));
    printf("\n\n", sizeof(char[3]));

    //Yield CPU before prompting for and reading user input
    CPU_yield();

    // Convert entry to integer
    seconds = atoi(secondsBuf);

    // Input validation
    if ((seconds >= 0) && (seconds < 60)){
        
        // Valid entry
        break;
    }
    
    // Otherwise, print invalid entry statement
    printf("\nInput invalid!\n",sizeof(char[17]));

    }

    // Assign variables values
    passed_alarm->time.hour = hours;
    passed_alarm->time.minute = minutes;
    passed_alarm->time.second = seconds;

}
