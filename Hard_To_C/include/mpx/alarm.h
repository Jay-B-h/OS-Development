#ifndef MPX_ALARM_H
#define MPX_ALARM_H

#include <stddef.h>
#include <mpx/rtc.h>

typedef struct alarmStruct {
    dateStruct date;
    timeStruct time;
    char message[50];
    struct alarmStruct *next;

}alarmStruct;


/**
 * @name create_alarm
 * @brief Creates a user defined alarm
 * @return void
*/
void create_alarm(void);

/**
 * @name check_alarm
 * @brief Checks list of alarms to determine if a given alarms set time has passed. Prints off alarms message and removes them from queue.
 * @return void
*/
void check_alarm(void);

/**
 * @name set_alarm_time
 * @brief Prompts user for the date and time to store in a alarm struct
 * @param alarmStruct *passed_alarm: Struct to hold values for the date and time 
*/
void set_alarm_time(alarmStruct *passed_alarm);

#endif
