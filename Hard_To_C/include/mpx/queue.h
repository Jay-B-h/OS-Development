#ifndef MPX_QUEUE_H
#define MPX_QUEUE_H

//#include <stdio.h>
#include <stdlib.h>
#include <mpx/pcb.h>
#include <mpx/alarm.h>
#include <mpx/mem.h>


/**
 * Queue struct
 * Defines queue as struct containing pointers to front and rear PCBs
*/
typedef struct Queue {
    pcbStruct *front, *rear;
}Queue;

/**
 * Queue struct
 * Defines queue as struct containing pointers to front and rear PCBs
*/
typedef struct alarm_Queue {
    alarmStruct *front, *rear;
}alarm_Queue;

typedef struct MCB_Queue {
    mcbStruct *front, *rear;
}MCB_Queue;

/**
 * @name is_empty
 * @brief Checks if a passed queue is empty
 * @param Queue *q Passed queue
 * @return int Value representing if queue is empty or not
*/
int is_empty(Queue* q);

/**
 * @name enqueue
 * @brief inserts a PCB to the end of a queue
 * @param Queue* q Passed queue
 * @param pcbStruct* pcb Passed pcb
*/
void enqueue (Queue* q, pcbStruct* pcb);

/**
 * @name dequeue
 * @brief removes the pcb at the front of queue
 * @param Queue* q Passed queue
 * @return pcbStruct* pcb removed from queue
*/
pcbStruct* dequeue(Queue* q);

/**
 * @name enqueue_alarm
 * @brief inserts an alarm to the end of the alarm queue
 * @param alarm_Queue* pointer to global alarm queue
 * @param alarmStruct* pointer to alarm struct
*/
void enqueue_alarm (alarm_Queue* q, alarmStruct* alarm);

/**
 * @name dequeue_alarm
 * @brief removes the specified alarm
 * @param alarm_Queue* pointer to alarm queue
 * @param alarmStruct* pointer to alarm struct
*/
void dequeue_alarm(alarm_Queue* q, alarmStruct* alarm_in);

/**
 * @name is_alarm_empty
 * @param alarm_Queue* pointer to alarm queue
 * @return int 1 empty and 0 if not
*/
int is_alarm_empty(alarm_Queue* q);

int is_mcb_empty(MCB_Queue* q);

#endif
