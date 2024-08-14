#ifndef MPX_SERIAL_H
#define MPX_SERIAL_H

#include <stddef.h>
#include <mpx/device.h>
#include <mpx/pcb.h>

extern int eventFlag;

// Status Index
typedef enum statusIndex{
    MAYBE = 2,
    YES = 1,
    NO = 0
}statusIndex;

// Allocation Status Index
typedef enum allocStatusIndex{
    OPEN = 1,
    CLOSE = 2
}allocStatusIndex;

// Device Control Block Struct
typedef struct dcbStruct{
    // Check slides 6 & 7

    // Status
    int allocStatus;            // Is the resource in use?
    int currOperation;          // Read, write, etc.
    
    // Internal Ring
    char ringBuffer[80];        // Circular queue of characters
    int bufferSize;             // Size of valid data
    int ringIn;                 // Next empty slot in the buffer to fill
    int ringOut;                // Next character to remove from the buffer
    int count;                  // # chars in the buffer 

    //User's Buffers
    char* userReadBuffer;       // User input buffer
    int userBufferSize;         // Max buffer size
    int charsRead;              // # of characters transferred so far

    char* userWriteBuffer;      // User output buffer
    int numTransfer;            // # of bytes to write
    int numWrite;               // # written so far 

}dcbStruct;

// I/O Control Block Struct
typedef struct iocbStruct{
    
    struct pcbStruct* associatedPCB;     // PCB associated with the IOCB    
    device dev;                 // Device associated with the IOCB
    int operationType;          // Read, write, etc.
    char* buffer;               // Character buffer
    int buffSize;               // Buffer size
    struct iocbStruct* next;           // Pointer to the next iocb in queue 
    
}iocbStruct;

// I/O Control Block Queue
typedef struct iocbQueue {

    struct iocbStruct *head, *rear;

} iocbQueue;

/**
 @file mpx/serial.h
 @brief Kernel functions and constants for handling serial I/O
*/

/**
 Initializes devices for user input and output
 @param device A serial port to initialize (COM1, COM2, COM3, or COM4)
 @return 0 on success, non-zero on failure
*/
int serial_init(device dev);

/**
 Writes a buffer to a serial port
 @param device The serial port to output to
 @param buffer A pointer to an array of characters to output
 @param len The number of bytes to write
 @return The number of bytes written
*/
int serial_out(device dev, const char *buffer, size_t len);

/**
 * @name serial_poll
 * @brief Reads a string from a serial port
 * @param device The serial port to read data from
 * @param buffer A buffer to write data into as it is read from the serial port
 * @param len The maximum number of bytes to read
 * @return The number of bytes read on success, a negative number on failure
*/   		   
int serial_poll(device dev, char *buffer, size_t len);

/**
 * @name back_arrow
 * @brief Moves the cursor backward in terminal
 * @param dev The serial port to read data from
 * @return N/A
*/
void back_arrow(device dev);

/**
 * @name forward_arrow
 * @brief Moves the cursor forward in terminal
 * @param dev The serial port to read data from
 * @return N/A
*/
void foward_arrow(device dev);

/**
 * @name delete
 * @brief Deletes a character and moves the remaining string back
 * @param cursor_ptr Pointer to cursor spot
 * @param cur_ptr Pointer to current length of string
 * @param buffer Buffer storing string
 * @param dev The serial port to read data from
 * @return N/A
*/
void delete(int* cursor_ptr, int* cur_ptr, char* buffer, device dev);

/**
 * @name backspace
 * @brief Backspaces characters in terminal
 * @param cursor_ptr Pointer to cursor spot
 * @param cur_ptr Pointer to current length of string
 * @param buffer Buffer storing string
 * @param dev The serial port to read data from
 * @return N/A
*/
void backspace(int* cursor_ptr, int* cur_ptr, char* buffer, device dev);

/**
 * @name insert
 * @brief Inserts a character in the middle of a string
 * @param cursor_ptr Pointer to cursor spot
 * @param cur_ptr Pointer to current length of string
 * @param buffer Buffer storing string
 * @param dev The serial port to read data from
 * @param c Character passed from inb
 * @return N/A
*/
void insert(int* cursor_ptr, int* cur_ptr, char* buffer, device dev, char c);

/**
 * @name serial_open
 * @brief Initalizes the serial port associated with the dev parameter
 * @param device dev: Device of the port to open 
 * @param int speed: Speed used to calculate the baud rate
 * @return int: Success on 0, error for anything else 
*/
int serial_open(device dev, int speed);

/**
 * @name serial_close
 * @brief Ends a serial port session
 * @param device dev: Device associated with the port to close 
 * @return int: 0 on success, error on anything else 
*/
int serial_close(device dev);

/**
 * @name serial_read
 * @brief Begins reading input from the serial device dev
 * @param device dev: Device driver used to write to the serial port 
 * @param char *buf: Character buffer from the requestor
 * @param size_t size_t len: Length of the characters to read 
 * @return int: 0 on success, anything else is an error 
*/
int serial_read(device dev, char *buf, size_t len);

/**
 * @name serial_write
 * @brief Begins writing data to the serial port dev 
 * @param device dev: Device driver used to write to the serial port
 * @param char *buf: Character buffer from the requestor
 * @param size_t size_t len: Length of the characters to written 
 * @return int: 0 on success, anything else is an error 
*/
int serial_write(device dev, char *buf, size_t len);

/**
 * @name serial_interrupt
 * @brief Disables interrupts
*/
void serial_interrupt(void);

/**
 * @name serial_input_interrupt
 * @brief Reads a character from the devices
*/
void serial_input_interrupt(void);

/**
 * @name serial_output_interrupt
 * @brief Writes characters to the device
*/
void serial_output_interrupt(void);

/**
 * @name is_empty_iocb
 * @brief Checks if a passed queue is empty
 * @param Queue *q Passed queue
 * @return int Value representing if queue is empty or not
*/
int is_empty_iocb(iocbQueue* q);

/**
 * @name enqueue_iocb
 * @brief inserts an IOCB to the end of a queue
 * @param Queue* q Passed queue
 * @param pcbStruct* iocb Passed iocb
*/
void enqueue_iocb (iocbQueue* q, iocbStruct* iocb);


/**
 * @name dequeue_iocb
 * @brief removes the iocb at the front of queue
 * @param Queue* q Passed queue
 * @return iocbStruct* iocb removed from queue
*/
iocbStruct* dequeue_iocb(iocbQueue* q);

/**
 * @name io_scheduler
 * @brief Determines if an I/O interrupt should be serviced or placed onto the waiting queue 
 * @param device dev: Device driver used to write to the serial port
 * @param pcbStruct* pcb: PCB of the process associated wiht the interrupt
 * @param int op_code: The operation to be performed 
 * @param char* buffer: Character buffer from the requestor 
 * @param int size: Size of the characters contined in the buffer
 * @return int: 0 on success, -1 on failure 
*/
int io_scheduler(device dev, pcbStruct* pcb, int op_code, char* buffer, int size);

/**
 * @name io_completeion
 * @brief Performs a completion sequence anytime an event flag is set 
*/
void io_completion (void);

#endif
