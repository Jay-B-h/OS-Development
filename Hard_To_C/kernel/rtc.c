// Created by Jordan Dennison on 1/12/24

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

// Instantiate global variables
timeStruct returnTime;
dateStruct returnDate;

/**
 * @name bcdToD
 * @brief Converts binary coded decimal values to decimal
 * @param bcd Binary coded decimal value to be converted
 * @return Decimal equivalent to the binary coded decimal
*/
int bcdToD(int bcd){

    // Return decimal eqivalent to BCD
    return (((bcd>>4)*10) + (bcd&0x0F));

}

/**
 * @name dToBCD
 * @brief Converts decimal values to Binary Coded Decimal
 * @param decimal value to be converted
 * @return The binary coded decimal equivalent to the decimal
*/
int dToBCD(int decimal){

    // Calculate the digit in the "ones" position
    int ones = decimal % 10;

    // Calculate the digit in the "tens" position
    int tens = decimal / 10;

    // Return binary coded decimal equivalent
    return ((tens<<4) + ones);

}

/**
 * @name read
 * @brief Reads the value in a particular index in the RTC
 * @param index The index of the value to be read
 * @return The value of the index in binary coded decimal
*/
int read(rtcIndex index){
    
    // Write index to the RTC index register
    outb(0x70, index);

    // Read the result from the RTC data register
    return inb(0x71);

}

/**
 * @name write
 * @brief Writes a value to a particular index in the RTC
 * @param index index for the value to be written into
 * @param value value to write into the desired index
 * @return N/A
*/
void write(rtcIndex index, int value){

    // Disable interupts
    cli();

    // Write index to the RTC
    outb(0x70, index);

    // Write new value for the index
    outb(0x71, value);

    // Enable Interupts
    sti();

}

/**
 * @name setTime
 * @brief Sets the RTC registers for the given time
 * @param hour Value for the hour to be set to
 * @param minute Value for the minute to be set to
 * @param second Value for the second to be set to
 * @return N/A
*/
void setTime(int hour, int minute, int second){

    // Write hour value to the Hours RTC index 
    write(Hours, dToBCD(hour));

    // Write minute value to the Minutes RTC index 
    write(Minutes, dToBCD(minute));

    // Write second value to the Seconds RTC index 
    write(Seconds, dToBCD(second));

}

/**
 * @name setDate
 * @brief Sets the RTC registers for the given date
 * @param day Value for the day register to be set to
 * @param month Value for the month register to be set to
 * @param year Value for the year register to be set to
 * @return N/A
*/
void setDate(int day, int month, int year){

    // Write day value to the day of the month RTC index 
    write(DayOfMonth, dToBCD(day));

    // Write month value to the month RTC index 
    write(Month, dToBCD(month));

    // Write year value to the year RTC index 
    write(Year, dToBCD(year));

}

/**
 * @name getTime
 * @brief Returns a struct containing the current RTC register values for hour, minute, and second
 * @return Struct containing the current RTC register values for hour, minute, and second
*/
timeStruct getTime(void){

    // Create timeStruct to store RTC data
    timeStruct returnTime;
    
    // Store current hour, minute, and second in struct
    returnTime.hour = bcdToD(read(Hours));
    returnTime.minute = bcdToD(read(Minutes));
    returnTime.second = bcdToD(read(Seconds));

    // Return timeStruct
    return returnTime;


}

/**
 * @name getDate
 * @brief Returns a struct containing the current RTC register values for day, month, and year
 * @return Struct containing the current RTC register values for day, month, and year
*/
dateStruct getDate(void){

    // Create a dateStruct to store RTC data
    dateStruct returnDate;

    // Store current day, month, and year in struct
    returnDate.day = bcdToD(read(DayOfMonth));
    returnDate.month = bcdToD(read(Month));
    returnDate.year = bcdToD(read(Year));
    
    // Return dateStruct
   return returnDate; 

}

