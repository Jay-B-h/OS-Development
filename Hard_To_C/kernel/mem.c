#include <mpx/mem.h>
#include <mpx/queue.h>
#include <mpx/vm.h>
#include <mpx/io.h>
#include <mpx/device.h>

struct MCB_Queue memory_list = {.front = NULL, .rear = NULL};

/**
 * @name initalize_heap
 * @brief Allocates all memory available to the memory manager as a single, large free block
 * @param size_t size: The total size of the heap
*/
void initialize_heap(size_t size) {
    //Store all available memory into a mcb struct
    mcbStruct* mcb = kmalloc(size, 0, NULL);

    //Initalize mcb values 
    mcb->start = (int)(mcb+1); // TODO: REVIEW********** added the int casting due to error
    mcb->size = size - sizeof(mcbStruct); 
    mcb->type = FREE;
    mcb->next = NULL;
    mcb->prev = NULL;

    //Add the free memory into the memory manager
    memory_list.front = mcb;
    memory_list.rear = mcb;
}

/**
 * @name allocate_memory
 * @brief Allocates memory from the heap
 * @param size_t size: The size, in bytes, of the requested allocation
 * @return void* address: Pointer to the starting address of the allocated block
*/
void *allocate_memory(size_t size) {

    if (is_mcb_empty(&memory_list)) {
        return NULL; // represents error

    }

    // set curMCB to the front of the list
    mcbStruct *curMCB = memory_list.front;

    while (curMCB != NULL) {
        if ((curMCB->type == FREE) && (curMCB->size >= (size + sizeof(mcbStruct)))) { // allocate here

            if (curMCB->size > (size + sizeof(mcbStruct))) {

                // Create new free MCB that begins where allocated newly allocated MCB will end
                mcbStruct *freeMCB = (mcbStruct*)(curMCB->start + size);


                // initialize values of newly partitioned free block
                freeMCB->start = curMCB->start + size + sizeof(mcbStruct); // new start is curMCB start, plus the size just allocated to the curMCB, plus the size of an MCB struct
                freeMCB->size = curMCB->size - size - sizeof(mcbStruct); // size is the size of curMCB minus the size allocated to a new block, minus the size of an MCB struct
                freeMCB->type = FREE;
                freeMCB->next = curMCB->next;
                freeMCB->prev = curMCB;

                // update rear of list if necessary
                if (memory_list.rear == curMCB) {
                    memory_list.rear = freeMCB;
                }
                
                // update curMCB to be allocated
                curMCB->size = size;
                curMCB->next = freeMCB;

            }
               
            // set curMCB to allocated
            curMCB->type = ALLOCATE;

            return (void*) curMCB->start;
        }

        curMCB = curMCB->next;
    }

    return NULL; // represents error in allocation
}

/**
 * @name free_memory
 * @brief Frees allocated memory, placing the associated block on the free list
 * @param void *start: A pointer to the start address of an allocated block
 * @return int result: Integer determining if the memory was sucessfully freed
*/
int free_memory(void *start) {
    
    if (is_mcb_empty(&memory_list)) {
        return -1; // represents error, can be adjusted
    }

    mcbStruct *curMCB = memory_list.front;
    // mcbStruct curMCB = (int) start - sizeof(mcbStruct);

    // convert void pointer to integer
    int *intPtr = (int*) start;
    int intNum = *intPtr;

    while (curMCB != NULL) {
        
        if (curMCB->start == intNum) {
            curMCB->type = FREE;

            if (curMCB->prev != NULL) {
                if (curMCB->prev->type == FREE) {
                    //Merge with previous
                    mcbStruct* prevMCB = curMCB->prev;

                    //Refactor size of merged mcb 
                    prevMCB->size = prevMCB->size + curMCB->size + sizeof(mcbStruct);

                    //Set current mbc's next's previous mcb to the current mcb's previous mcb
                    curMCB->next->prev = prevMCB;

                    //Set previous mcb's next mcb to current mcb's next mcb
                    prevMCB->next = curMCB->next;

                    //Set current mcb to previous mcb, merging the two together
                    curMCB = prevMCB;
                }
            }
            
            if (curMCB->next != NULL) {
                if (curMCB->next->type == FREE) {
                    // merge with next
                    curMCB->size = curMCB->size + curMCB->next->size + sizeof(mcbStruct);

                    // sets the curMCB's next MCB to the next MCB's next MCB
                    curMCB->next = curMCB->next->next;

                    if (curMCB->next != NULL){

                        // sets curMCB's next's previous MCB to the current MCB
                        curMCB->next->prev = curMCB;

                    }
                    
                }
            }

            // Return Successful Code
            return 1;
        }


        curMCB = curMCB->next;
    }

    return -1;

}

/**
 * @name htoi
 * @brief Converts hexadecimal number to integer. To be used in free() in comhand.c
 * @param char* hexNumber: Character pointer representing 
 * @return int result: The converted hexadecimal to integer value 
 */
int htoi(char* hexNumber) {

    int integer = 0;
    //int count = 0;

    // Iterate through each character in the string
    for (int i = 2; i < 9; i++) {
        int digit;

        // Convert hexadecimal digit to integer
        if (hexNumber[i] >= '0' && hexNumber[i] <= '9') {
            digit = hexNumber[i] - '0';
        } 
        
        else if (hexNumber[i] >= 'a' && hexNumber[i] <= 'f') {
            digit = hexNumber[i] - 'a' + 10;
        } 
        
        else if (hexNumber[i] >= 'A' && hexNumber[i] <= 'F') {
            digit = hexNumber[i] - 'A' + 10;
        } 
        
        else {
            // Invalid character
            return 0;
        }

        // Update the integer value
        integer = integer * 16 + digit;
        //count++;
    }

    return integer;
    
}//end htoi

/**
 * @name itoh
 * @brief Converts integer number inot a char* hexadecimal number
 * @param int intNum: 
 * @param char* strArray:
 * @return char* strArray: 
*/
char* itoh(int intNum, char* strArray){

    // Instantiate string array
    strArray[0] = '0';
    strArray[1] = 'x';
    strArray[2] = '0';
    strArray[3] = '0';
    strArray[4] = '0';
    strArray[5] = '0';
    strArray[6] = '0';
    strArray[7] = '0';
    strArray[8] = '0';

    // If the number entered is 0, return hex for 0
    if (intNum <= 0){
        
        return strArray;

    }

    // Instantiate counter variable
    int i = 8;


    // Loop until number entered is 0
    while (intNum > 0){

        // Convert from int to hex
        int digit = intNum%16;
        strArray[i] = '0' + (digit<10? 0 : 7) + digit;   

        // Divide intNum by 16 and store in intNum
        intNum /= 16; 

        // Decrement counter
        i--;

    }

    // Return hex number
    return strArray;
}

