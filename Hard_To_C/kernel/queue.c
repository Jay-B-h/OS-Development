//#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <sys_req.h>
#include <mpx/comhand.h>
#include <mpx/pcb.h>
#include <mpx/queue.h>
#include <mpx/alarm.h>


/**
 * PCB Queues
*/

/**
 * @name is_empty
 * @brief Checks if a passed queue is empty
 * @param Queue *q Passed queue
 * @return int Value representing if queue is empty or not
*/
int is_empty(Queue* q) {
  
   if (q->front == NULL || q->rear == NULL) {
       return 1;
   }


   return 0;
}

/**
 * @name enqueue
 * @brief inserts a PCB to the end of a queue
 * @param Queue* q Passed queue
 * @param pcbStruct* pcb Passed pcb
*/
void enqueue (Queue* q, pcbStruct* pcb) {
   if (pcb == NULL) {
       //printf("Failed to allocate memory for pcb\n");
       return;
   }

   if (is_empty(q)) {
       q->front = q->rear = pcb;
   } else {
       pcbStruct* curNode = q->front;
       while (curNode->next != NULL) {
           curNode = curNode->next;
       }


       curNode->next = pcb;
       q->rear = pcb;
   }
   return;
}



/**
 * @name dequeue
 * @brief removes the pcb at the front of queue
 * @param Queue* q Passed queue
 * @return pcbStruct* pcb removed from queue
*/
pcbStruct* dequeue(Queue* q) {


    if (is_empty(q)) {
        return NULL;
    }

    pcbStruct* remNode = q->front;


    q->front = q->front->next;


    return remNode;
}


/**
 * Alarm queues
*/

/**
 * @name enqueue_alarm
 * @brief inserts an alarm to the end of the alarm queue
 * @param alarm_Queue* pointer to global alarm queue
 * @param alarmStruct* pointer to alarm struct
*/
void enqueue_alarm (alarm_Queue* q, alarmStruct* alarm) {
   
   // Check if the alarm exists 
   if (alarm == NULL) {
       return;
   }

    // If the queue is empty, set the front and rear of 
    // the queue to the alarm struct
   if (is_alarm_empty(q)) {
       q->front = q->rear = alarm;
   } else {

        // Otherwise find the last node in the queue
       alarmStruct* curNode = q->front;
       while (curNode->next != NULL) {
           curNode = curNode->next;
       }

        // Set the last nodes next value to the passes alarm struct
       curNode->next = alarm;
       q->rear = alarm;
   }
   
   return;
}


/**
 * @name dequeue_alarm
 * @brief removes the specified alarm
 * @param alarm_Queue* pointer to alarm queue
 * @param alarmStruct* pointer to alarm struct
*/
void dequeue_alarm(alarm_Queue* q, alarmStruct* alarm_in) {


    if (is_alarm_empty(q)) {
        return;
    }


        // removes pcb from front of queue and updates front pointer
    if (q->front == alarm_in) {
        if (q->front->next != NULL) {
            q->front = q->front->next;
        } else {
            q->front = NULL;
        }
        return;
    }

    // Node used to iterate through queue
    alarmStruct *curAlarm = q->front;

    // iterate through queue until end is reached or match is found
    while (curAlarm != NULL) {
        if (curAlarm->next == alarm_in) {
            if (curAlarm->next->next != NULL) {
                curAlarm->next = curAlarm->next->next;
                return;
            }
            else {
                curAlarm->next = NULL;
                return;
            }
        }

        curAlarm = curAlarm->next;
    }


}

/**
 * @name is_alarm_empty
 * @param alarm_Queue* pointer to alarm queue
 * @return int 1 empty and 0 if not
*/
int is_alarm_empty(alarm_Queue* q) {
  
   if (q->front == NULL || q->rear == NULL) {
       return 1;
   }


   return 0;
}

/**
 * @name is_mcb_empty
 * @param MCB_Queue* pointer to alarm queue
 * @return int 1 empty and 0 if not
*/
int is_mcb_empty(MCB_Queue* q) {
  
   if (q->front == NULL || q->rear == NULL) {
       return 1;
   }


   return 0;
}
