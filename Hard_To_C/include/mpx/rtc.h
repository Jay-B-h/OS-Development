#ifndef MPX_RTC_H
#define MPX_RTC_H

#include <stddef.h>

// RTC Indexs
typedef enum rtcIndex{
    Seconds = 0x00,
    Minutes = 0x02,
    Hours = 0x04,
    DayOfWeek = 0x06,
    DayOfMonth = 0x07,
    Month = 0x08,
    Year = 0x09
}rtcIndex;

// timeStruct declaration
typedef struct timeStuct{
    int hour;
    int minute;
    int second;
}timeStruct;

// dateStruct declaration
typedef struct dateStruct{
    int day;
    int month;
    int year;
}dateStruct;

/**
 * @name bcdToD
 * @brief Converts binary coded decimal values to decimal
 * @param bcd Binary coded decimal value to be converted
 * @return Decimal equivalent to the binary coded decimal
*/
int bcdToD(int bcd);

/**
 * @name dToBCD
 * @brief Converts decimal values to Binary Coded Decimal
 * @param decimal value to be converted
 * @return The binary coded decimal equivalent to the decimal
*/
int dToBCD(int decimal);

/**
 * @name read
 * @brief Reads the value in a particular index in the RTC
 * @param index The index of the value to be read
 * @return The value of the index in binary coded decimal
*/
int read(rtcIndex index);

/**
 * @name write
 * @brief Writes a value to a particular index in the RTC
 * @param index index for the value to be written into
 * @param value value to write into the desired index
 * @return N/A
*/
void write(rtcIndex index, int value);

/**
 * @name setTime
 * @brief Sets the RTC registers for the given time
 * @param hour Value for the hour to be set to
 * @param minute Value for the minute to be set to
 * @param second Value for the second to be set to
 * @return N/A
*/
void setTime(int hour, int minute, int second);

/**
 * @name setDate
 * @brief Sets the RTC registers for the given date
 * @param day Value for the day register to be set to
 * @param month Value for the month register to be set to
 * @param year Value for the year register to be set to
 * @return N/A
*/
void setDate(int day, int month, int year);

/**
 * @name getTime
 * @brief Returns a struct containing the current RTC register values for hour, minute, and second
 * @return Struct containing the current RTC register values for hour, minute, and second
*/
timeStruct getTime( void );

/**
 * @name getDate
 * @brief Returns a struct containing the current RTC register values for day, month, and year
 * @return Struct containing the current RTC register values for day, month, and year
*/
dateStruct getDate( void );

#endif
