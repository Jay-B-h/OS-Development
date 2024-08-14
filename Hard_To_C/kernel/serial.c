// Created by Jacob Comer on 1/12/24

#include <mpx/io.h>
#include <mpx/serial.h>
#include <sys_req.h>
#include <string.h>
#include <memory.h>
#include <mpx/interrupts.h>
#include <mpx/sys_call.h>

extern void serial_isr(void*);

dcbStruct *com1DCB = NULL;
dcbStruct *com2DCB = NULL;
dcbStruct *com3DCB = NULL;
dcbStruct *com4DCB = NULL;

struct iocbQueue com1Queue = {.head = NULL, .rear = NULL};

enum uart_registers {
	RBR = 0,	// Receive Buffer
	THR = 0,	// Transmitter Holding
	DLL = 0,	// Divisor Latch LSB
	IER = 1,	// Interrupt Enable
	DLM = 1,	// Divisor Latch MSB
	IIR = 2,	// Interrupt Identification
	FCR = 2,	// FIFO Control
	LCR = 3,	// Line Control
	MCR = 4,	// Modem Control
	LSR = 5,	// Line Status
	MSR = 6,	// Modem Status
	SCR = 7,	// Scratch
};

static int initialized[4] = { 0 };

static int serial_devno(device dev)
{
	switch (dev) {
	case COM1: return 0;
	case COM2: return 1;
	case COM3: return 2;
	case COM4: return 3;
	}
	return -1;
}

int serial_init(device dev)
{
	int dno = serial_devno(dev);
	if (dno == -1) {
		return -1;
	}
	outb(dev + IER, 0x00);	//disable interrupts
	outb(dev + LCR, 0x80);	//set line control register
	outb(dev + DLL, 115200 / 9600);	//set bsd least sig bit
	outb(dev + DLM, 0x00);	//brd most significant bit
	outb(dev + LCR, 0x03);	//lock divisor; 8bits, no parity, one stop
	outb(dev + FCR, 0xC7);	//enable fifo, clear, 14byte threshold
	outb(dev + MCR, 0x0B);	//enable interrupts, rts/dsr set
	(void)inb(dev);		//read bit to reset port
	initialized[dno] = 1;
	return 0;
}

int serial_out(device dev, const char *buffer, size_t len)
{
	int dno = serial_devno(dev);
	if (dno == -1 || initialized[dno] == 0) {
		return -1;
	}
	for (size_t i = 0; i < len; i++) {
		outb(dev, buffer[i]);
	}
	return (int)len;
}


/**
 * @name serial_poll
 * @brief Reads a string from a serial port
 * @param device The serial port to read data from
 * @param buffer A buffer to write data into as it is read from the serial port
 * @param len The maximum number of bytes to read
 * @return The number of bytes read on success, a negative number on failure
*/ 
int serial_poll(device dev, char *buffer, size_t len)
{

	// length of string in buffer received
	int cur_len = strlen(buffer);

	// tracks where cursor is in terminal
	int cursor_spot = 0;


	while(cur_len < (int)len - 1){    // May need to adjust iteration
		//serial_out(COM1, test, passlen);
		if(inb(dev + LSR) & 0x01) 
		{
			// read character
			char c = inb(dev);

			// if character is carriage return, exit while loop
			if (c == 0xD) {
				break;
			}

			// handles backspace
			if (c == 0x7F) { // NOTE: c is checking for ASCII for delete, terminal seems to recongize as backspace instead
				backspace(&cursor_spot, &cur_len, buffer, dev);
				continue;
			}

			// if statement segment checks all espace sequence special keys
			if (c == '\33') {
				c = inb(dev);
				if (c == '['){
				
					// gets next char after escape key sequence
					c = inb(dev);

					// handles back arrow
					if (c == 'D') {
						if (cursor_spot > 0) {
							back_arrow(dev);
							cursor_spot--;
						}
					}

					// handles forward arrow
					if (c == 'C') {
						if (cursor_spot < cur_len) {
							foward_arrow(dev);
							cursor_spot++;
						}

					}

					// handles delete
					if (c == '3') {
						c = inb(dev);
						if (c == '~') {
							delete(&cursor_spot, &cur_len, buffer, dev);
						}
					}

					// ignores insert
					if (c == '2') {
						c = inb(dev);
					}
				}

				// ignore other possible inputs
				continue;
			}
			
			// ensures all remaining characters can be represented as ascii or ignores them
			if (c > 0x0 && c < 0x7f) {

				if (cursor_spot == cur_len) { 	// behavior when adding on to end of string
					cur_len++;

					// add character to buffer
					buffer[cursor_spot] = c;

					// print out character
					outb(dev, c);
				} else { 						// behavior when in the middle of a string
					insert(&cursor_spot, &cur_len, buffer, dev, c);			
				}
				cursor_spot++;

			}

		}
	}

	return (int)cur_len;
}

/**
 * @name back_arrow
 * @brief Moves the cursor backward in terminal
 * @param dev The serial port to read data from
 * @return N/A
*/
void back_arrow(device dev) {
	outb(dev, '\33');
	outb(dev, '[');
	outb(dev, 'D');

	return;
}

/**
 * @name forward_arrow
 * @brief Moves the cursor forward in terminal
 * @param dev The serial port to read data from
 * @return N/A
*/
void foward_arrow(device dev) {
	outb(dev, '\33');
	outb(dev, '[');
	outb(dev, 'C');

	return;
}

/**
 * @name delete
 * @brief Deletes a character and moves the remaining string back
 * @param cursor_ptr Pointer to cursor spot
 * @param cur_ptr Pointer to current length of string
 * @param buffer Buffer storing string
 * @param dev The serial port to read data from
 * @return N/A
*/
void delete(int* cursor_ptr, int* cur_ptr, char* buffer, device dev) {

	
	if ((*cursor_ptr) < (*cur_ptr)) { // check if cursor inside the string
		
		// decrement length of string
		(*cur_ptr)--;

		// starting at cursor, replace every character with the following character and print to terminal
		for (int i = (*cursor_ptr); i < (*cur_ptr); i++) {
			buffer[i] = buffer[i + 1];
			outb(dev, buffer[i]);
		}
		
		// deletes previously placed final char by replacing with space
		outb(dev, ' ');

		// back arrow until cursor spot placed at just after recently deleted character
		for (int i = (*cursor_ptr); i < (*cur_ptr) + 1; i++) {
			back_arrow(dev);
		}
		
	} 


	return;
}

/**
 * @name backspace
 * @brief Backspaces characters in terminal
 * @param cursor_ptr Pointer to cursor spot
 * @param cur_ptr Pointer to current length of string
 * @param buffer Buffer storing string
 * @param dev The serial port to read data from
 * @return N/A
*/
void backspace(int* cursor_ptr, int* cur_ptr, char* buffer, device dev) {

	if ((*cursor_ptr) > 0) {

					
		// output a backspace to terminal
		outb(dev, '\b');
		outb(dev, ' ');
		outb(dev, '\b');
		
		// decrement length and cursor indexes
		(*cursor_ptr)--;
		(*cur_ptr)--;  // this addition is the main reason for different behavior between delete and backspace
		
		// check if point cursor is placed within string
		if ((*cursor_ptr) < (*cur_ptr)) {
			
			// starting at cursor, replace every character with the following character and print to terminal
			for (int i = (*cursor_ptr); i < (*cur_ptr); i++) {
				buffer[i] = buffer[i + 1];
				outb(dev, buffer[i]);
			}

			// deletes previously placed final char by replacing with space
			outb(dev, ' ');

			// back arrow until cursor spot placed at just after recently deleted character
			for (int i = (*cursor_ptr); i < (*cur_ptr) + 1; i++) {
				back_arrow(dev);
			}
		}

	}

	return;
}

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
void insert(int* cursor_ptr, int* cur_ptr, char* buffer, device dev, char c) {

	// buffer to temporarily store portion of buffer
	char temp_buffer[100];

	// fill temporary buffer
	for (int i = (*cursor_ptr); i < (*cur_ptr); i++) {
		temp_buffer[i - (*cursor_ptr)] = buffer[i];
	}

	// add new character to buffer at cursor point
	buffer[(*cursor_ptr)] = c;

	// print character to terminal at cursor point and update cursor point
	outb(dev, c);
	(*cur_ptr)++;

	// add temp stored back into buffer offset ahead one index and print to terminal
	for (int i = (*cursor_ptr) + 1; i < (*cur_ptr); i++) {
		buffer[i] = temp_buffer[i - (*cursor_ptr) - 1];
		outb(dev, buffer[i]);
	}

	// back arrow until cursor spot placed at just after recently added character
	for (int i = (*cursor_ptr) + 1; i < (*cur_ptr); i++) {
		back_arrow(dev);
	}

	return;
}

/**
 * @name serial_open
 * @brief Initalizes the serial port associated with the dev parameter
 * @param device dev: Device of the port to open 
 * @param int speed: Speed used to calculate the baud rate
 * @return int: Success on 0, error for anything else 
*/
int serial_open(device dev, int speed) {
	if (serial_init(dev) != 0) {
		//error opening device
	}


	//Check to see if the event flag is NULL
    if (eventFlag != YES && eventFlag != NO) {
        return -101;
    }

    //Check to see the baud rate is valid
    if (speed <= 0) {
        return -102;
    }

    //Check to see if the dcb exists and if the device is already open
    if (serial_devno(dev) == 0) {
        if (com1DCB != NULL) {
            if (com1DCB->allocStatus == OPEN) {
                return -103;
            } 

        } else {
			//Allocate memory if dcb has not been initialized yet 
            com1DCB = allocate_memory(sizeof(dcbStruct));

   			com1DCB->userWriteBuffer = allocate_memory(sizeof(com1DCB->userWriteBuffer));

			if (com1DCB == NULL){
				// PRINT DEBUG
				// outb(COM1,'x');
			}

        }

    } else {
        //Return 1 if device is not COM1
        return 1;
    }
    
        
    //Initalize the dcb of the given device
    com1DCB->allocStatus = OPEN;
    com1DCB->currOperation = IDLE;
    eventFlag = NO;
    com1DCB->ringIn = 0;            
    com1DCB->ringOut = 0;

    //com1DCB->count = 0;

	// Install new handler in the interrupt vector
	idt_install(0x24, serial_isr); // TODO: MAY NEED FIXED

    //Baud rate divisor
    int baud_rate_div = 115200 / speed;
    
    outb(dev + LCR, 0x80);  //set line control register 
    outb(dev + DLM, (unsigned char) ((baud_rate_div & 0xFF00)>>8));
	outb(dev + DLL, (unsigned char)(baud_rate_div & 0x00FF)); //set bsd least sig bit 
    
	// 
	outb(dev + LCR, 0x03);  //lock divisor; 8bits, no parity, one stop
	
	// ELEC
	outb(dev + FCR, 0xC7);
    
    // PIC mask enabled
    cli();
    int mask = inb(0x21);
    mask &= ~(0x10); // change to something like (ob04)
    outb(0x21, mask);
    sti();

	// ELEC
    outb(dev + MCR, 0x0B);  //enable overall serial port interrupts 
    outb(dev + IER, 0x01);  //enable input ready interrupts only 
    (void)inb(dev);     //read bit to reset port


    //Assembly Interrupt Handler

    // *** REVIEW SERIAL OPEN IN R6 DETAILED ***
    
    return 0;
}

/**
 * @name serial_close
 * @brief Ends a serial port session
 * @param device dev: Device associated with the port to close 
 * @return int: 0 on success, error on anything else 
*/
int serial_close(device dev) {
	
	//Check to see if the correct serial port is open
	if (dev == 0x3f8) {
		
		// Check if the DCB exists 
		if (com1DCB != NULL){

			// If the allocation is closed, return -201
			if (com1DCB->allocStatus == CLOSE){
				return -201;
			}

			// Ensure the port is currently open
			if (com1DCB->allocStatus == OPEN){
				
				// Clear the open indicator in the DCB
				com1DCB->allocStatus = CLOSE;

				// Disable the appropriate level in the PIC mask register
				cli();
				int mask = inb(0x21);
				mask |= (1<<3);
				outb(0x21, mask);
				sti();

				// Disable all interrupts in the ACC
				outb(dev + MSR, 0x00);		// Set the Modem Status Register to 0x00
				outb(dev + IER, 0x00);		// Set the Interrupt Enable Register to 0x00

				return 0;
			}
		}			
	} 


	return 1;
}

/**
 * @name serial_read
 * @brief Begins reading input from the serial device dev
 * @param device dev: Device driver used to write to the serial port 
 * @param char *buf: Character buffer from the requestor
 * @param size_t size_t len: Length of the characters to read 
 * @return int: 0 on success, anything else is an error 
*/
int serial_read(device dev, char *buf, size_t len) {
	
	// Check for invalid buffer address
	if (buf == NULL){
		return -302;
	}
 
	//Check to see if the correct serial port is open
	if (dev == 0x3f8) {
		
		// Check if the DCB exists 
		if (com1DCB != NULL){

			// If the allocation is closed, return -301
			if (com1DCB->allocStatus == CLOSE){
				return -301;
			}

			// Check invalid count or count address
			//Count less than zero = error
			if (com1DCB->count < 0){
				return -303;
			}

			// Check if the device is busy
			if (com1DCB->currOperation != IDLE){
				return -304;
			}

			// COME BACK TO FINISH

			// Validate supplied parameters 
			if (len > 0){

				// Initialize input buffer variables
				//memcpy(com1DCB->userReadBuffer, buf, len);
				com1DCB->userReadBuffer = buf;
				com1DCB->userBufferSize = len;
				com1DCB->charsRead = 0;

				// Clear the callers event flag
				eventFlag = NO;  //??????????????????????????

				// Ring buffer tomfoolery
				// Must disable interrupts when accessing ring buffer
				cli();

				char tempBuffer[80];
				int i = 0;

				// while ((com1DCB->charsRead <= com1DCB->count) || (com1DCB->count > 0) || (com1DCB->ringBuffer[] == "/0")){
				// CHECK: Added &
				// CHECK: Logic
				while ((com1DCB->count > 0) || (com1DCB->numWrite == com1DCB->numTransfer) || (strcmp(&com1DCB->ringBuffer[com1DCB->ringOut],"/0") == 0)){

					if ((com1DCB->ringOut <= 79) && (com1DCB->ringOut != com1DCB->ringIn)){

						tempBuffer[i] = com1DCB->ringBuffer[com1DCB->ringOut];

						com1DCB->ringOut++;
						i++;

					// Set index back to 0 to wrap around
					} else if (com1DCB->ringOut == 80){

						com1DCB->ringOut = 0;

					// Head equals tail means there is no room left in the queue
					// Unless this is how it always starts??
					// IDK, check back later, may need more characters
					} else if (com1DCB->ringIn == com1DCB->ringOut){

						// May need to change return number
						return -1;

					}

				// Maybe add an if statement to copy over the null terminator
				}

				// Copy temp buffer into read buffer
				memcpy(com1DCB->userReadBuffer,tempBuffer,sizeof(tempBuffer));

				// Set chars read to the size read in
				com1DCB->charsRead = i;

				sti();

				// Reset the DCB status to IDLE
				com1DCB->currOperation = IDLE;

				// Set the event flag
				eventFlag = NO;




			}

			

			return 0;

		}

	}

	return 1;
}


/**
 * @name serial_write
 * @brief Begins writing data to the serial port dev 
 * @param device dev: Device driver used to write to the serial port
 * @param char *buf: Character buffer from the requestor
 * @param size_t size_t len: Length of the characters to written 
 * @return int: 0 on success, anything else is an error 
*/
int serial_write(device dev, char *buf, size_t len) {
	// Check to see if the buffer has a valid address
    if (buf == NULL) {
		return -402;
    }

    //Check to see if the device is COM1
    if (serial_devno(dev) == 0) {

		// Check to see if the device is currently open
        if (com1DCB != NULL) {

            if (com1DCB->allocStatus == CLOSE) {

				return -401;
            }

            //Check to see if count address and count value is valid
            if (len < 0) {

                return -403;        
            }

            //Check to see if the device is currently busy
            if (eventFlag == YES) {
    
				return -404;
            }

        } else {

            //If the dcb does not exist return error code -401
            return -401;
        }

    } else {
        //Return 1 if device is not COM1
        return 1;
    }

    //Install the buffer pointer and counters in the DCB
    //More steps may be needed for this!!!!!
	com1DCB->userWriteBuffer = buf;
	com1DCB->userBufferSize = len;
    com1DCB->numWrite = 0;

    //Set current status to writing
    com1DCB->currOperation = WRITE;

    //Clear event flag
    eventFlag = NO;

    //Stores first character from the buffer in the output register
    outb(dev, buf[0]);

	com1DCB->numWrite++;

    //Set bit 1 of the Interrupt Enable register
    //Set the register to the logical OR of previous value and 0x02
	cli();
	unsigned char mask = inb(dev + IER);
	mask |= 0x02;
    outb(dev + IER, mask); //DOUBLE CHECK
	sti();

    return 0;
}

/**
 * @name serial_interrupt
 * @brief Disables interrupts
*/
void serial_interrupt(void) {

	char interruptID = inb(COM1 + IIR);

	int mask = interruptID & 0x07;

	if (mask == 2) { // output
		serial_output_interrupt(); // pass params
	}

	if (mask == 4) {
		serial_input_interrupt(); //pass params
	}

	// issue EOI to PIC
	outb(0x20, 0x20); // ??


	return;
}

/**
 * @name serial_input_interrupt
 * @brief Reads a character from the devices
*/
void serial_input_interrupt(void) {

	//Step 1: read characters
	char character = inb(COM1);

	//Step 2
	if(com1DCB->currOperation != READ && ((com1DCB->count) <= 80)){
		
		//ring buffer stuff
		com1DCB->ringBuffer[com1DCB->ringIn] = character;
		com1DCB->ringIn++;

		com1DCB->count++;
			
	}// end if

	else {

		//Step 3
		if (com1DCB->currOperation ==  READ) {

			com1DCB->userReadBuffer[com1DCB->charsRead] = character; // or userBufferSize for index
			com1DCB->charsRead++;

			//check for completion??? - Step 4
			if( (com1DCB->count != 80) && (strcmp(&character, "/n") == 0) ){

				return; 

			}//end while 

			//Step 5
			//may need to move the three lines below outside of if because of the term "otherwise"
			com1DCB->allocStatus = IDLE;
			eventFlag = NO;
			// CHECK: Added &
			// CHECK: Removed return 
			return; //maybe that???
		}//end if 

	}//end else

	}//end function

/**
 * @name serial_output_interrupt
 * @brief Writes characters to the device
*/
void serial_output_interrupt(void) {

	if(com1DCB->currOperation != WRITE){
		
        return; //step 1

	}//endf if 

	else{
		//step 2
		if(com1DCB->numWrite < com1DCB->userBufferSize){
			//get the next character from requestors output buffer
			//com1DCB->userWriteBuffer[com1DCB->numWrite] = character; //or numTransfer for index
			//com1DCB->numWrite++;
			//use outb() to store in output register
			outb(COM1,com1DCB->userWriteBuffer[com1DCB->numWrite++]);
			return;

		}//end if

		//step 3
		//may need to move the three lines below outside of if branch
		com1DCB->allocStatus = OPEN;
		eventFlag = YES;

		cli();
		unsigned char mask = inb(COM1 + IER);
		mask |= 0x02;
		outb(COM1 + IER, mask); //DOUBLE CHECK
		sti();

		// CHECK: Removed return 
		return; //maybe 
	}

}

/**
 * @name is_empty_iocb
 * @brief Checks if a passed queue is empty
 * @param iocbQueue *q Passed queue
 * @return int Value representing if queue is empty or not
*/
int is_empty_iocb(iocbQueue* q) {
  
   if (q->head == NULL || q->rear == NULL) {
       return 1;
   }


   return 0;
}

/**
 * @name enqueue_iocb
 * @brief inserts an IOCB to the end of a queue
 * @param Queue* q Passed queue
 * @param pcbStruct* iocb Passed iocb
*/
void enqueue_iocb (iocbQueue* q, iocbStruct* iocb) {
   if (iocb == NULL) {
       return;
   }

   if (is_empty_iocb(q)) {
       q->head = q->rear = iocb;
	   iocb->next = NULL;
   } else {
       iocbStruct* curNode = q->head;
       while (curNode->next != NULL) {
           curNode = curNode->next;
	   }


       curNode->next = iocb;
       q->rear = iocb;
   }

   iocb->next = NULL;
   //return;
}



/**
 * @name dequeue_iocb
 * @brief removes the iocb at the front of queue
 * @param iocbQueue* q Passed queue
 * @return iocbStruct* iocb removed from queue
*/
iocbStruct* dequeue_iocb(iocbQueue* q) {


    if (is_empty_iocb(q)) {
        return NULL;
    }

    iocbStruct* remNode = q->head;


    q->head = q->head->next;

	remNode->next = NULL;

    return remNode;

}
 
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
int io_scheduler(device dev, pcbStruct* pcb, int op_code, char* buffer, int size) {
	//Check if the device is COM1
	if (serial_devno(dev) != 0) {
		return -1;
	}

	//Check if the device has an associated dcb
	if (com1DCB == NULL) {
		return -1;
	}

	//Check if the operation is read or write
	if ((op_code != READ) && (op_code != WRITE)) {
		return -1;
	}
	
	//Check if the buffer is NULL
	if (buffer == NULL) {
		return -1;
	}

	//Check if the buffer size is non-zero
	if (size <= 0) {
		return -1;
	}

	//Initialize iocb
	iocbStruct* iocb = allocate_memory(sizeof(iocbStruct));

	//Check if device is available
	if (eventFlag == NO) {

		//Requesting process is made the active one by installing the pcb
		iocb->associatedPCB = pcb;

		iocb->operationType = op_code;

		//Buffer address and length are placed into the iocb
		iocb->buffer = allocate_memory(sizeof(iocb->buffer));
		iocb->buffer = buffer;
		iocb->buffSize = size;

		//Request is installed on the waiting queue 
		enqueue_iocb(&com1Queue, iocb);

		//Dequeued the next iocb to be executed
		iocbStruct* iocb_ex = dequeue_iocb(&com1Queue);

		com1DCB->allocStatus = OPEN;

		//Begin processing request
		if (op_code == READ) {
				//Process the read request
				serial_read(dev, iocb_ex->buffer, iocb_ex->buffSize);

			} else {
				
				// PRINT DEBUG
				//outb(COM1,'f');

				//Process the write request
				serial_write(dev, iocb_ex->buffer, iocb_ex->buffSize);

			}
			

		} else {
			//Place pcb pointer, device ID, and operation code into the iocb
			iocb->associatedPCB = pcb;
			iocb->dev = dev;
			iocb->operationType = op_code;

			//Request is installed on the waiting queue
			enqueue_iocb(&com1Queue, iocb);
		} 

	//Return to sys_call
	sys_call((context*) pcb->stackPtr);

	return 0;
}

/**
 * @name io_completeion
 * @brief Performs a completion sequence anytime an event flag is set 
*/
void io_completion (void) {
	
	if (eventFlag == YES) {
		return;
	}

	iocbStruct *iocb = dequeue_iocb(&com1Queue);

	// if (iocb == NULL){
	// 	return;
	// }

	pcbStruct *pcb = iocb->associatedPCB;

	pcb_remove(pcb);

	pcb->execution_state = READY;

	pcb_insert(pcb);

	struct context* pcbContext = (context*)pcb->stackPtr;

	pcbContext->eax = strlen(iocb->buffer);

	pcb->stackPtr = (char*) pcbContext;

	sys_call(pcbContext);

	free_memory(iocb);
}
