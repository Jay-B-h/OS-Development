#ifndef MPX_MEM_H
#define MPX_MEM_H

#include <stddef.h>

typedef enum memIndex{
    FREE = 1,
    ALLOCATE = 2
}memIndex;

typedef struct mcbStruct {
    int start;
    size_t size;
    int type;
    struct mcbStruct* next;
    struct mcbStruct* prev;
} mcbStruct;

/**
 * @name initalize_heap
 * @brief Allocates all memory available to the memory manager as a single, large free block
 * @param size_t size: The total size of the heap
*/
void initialize_heap(size_t size);

/**
 * @name allocate_memory
 * @brief Allocates memory from the heap
 * @param size_t size: The size, in bytes, of the requested allocation
 * @return void* address: Pointer to the starting address of the allocated block
*/
void *allocate_memory(size_t size);

/**
 * @name free_memory
 * @brief Frees allocated memory, placing the associated block on the free list
 * @param void *start: A pointer to the start address of an allocated block
 * @return int result: Integer determining if the memory was sucessfully freed
*/
int free_memory(void *start);

/**
 * @name htoi
 * @brief Converts hexadecimal number to integer. To be used in free() in comhand.c
 * @param char* hexNumber: Character pointer representing 
 * @return int result: The converted hexadecimal to integer value 
 */
int htoi(char* hexNumber);

/**
 * @name itoh
 * @brief Converts integer number inot a char* hexadecimal number
 * @param int intNum: 
 * @param char* strArray:
 * @return char* strArray: 
*/
char* itoh(int intNum, char* strArray);

#endif
