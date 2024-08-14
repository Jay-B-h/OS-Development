// Created by Jack Kile on 1/12/24

#include <sys_req.h>
#include <mpx/comhand.h>
#include <string.h>
#include <mpx/rtc.h>
#include <mpx/io.h>
#include <stdlib.h>
#include <mpx/pcb.h>
#include <mpx/r3_commands.h>
#include <mpx/alarm.h>
#include <mpx/mem.h>

/**
 * @name comhand
 * @brief Outputs a menu with a list of commands. Depending on user input it executes certain commands 
 * @return N/A
*/
void comhand( void ) {
    
    int cont = 0;
    for (;;) {

        if (cont == 1) {

            printf("\n\nPress any key to continue\n\n",sizeof(char[30]));
            
            char buf[2] = {0};
            sys_req(READ, COM1, buf, sizeof(buf));

            //Yield CPU after reading user input
            CPU_yield();
        }

        cont = 0;

        //menu format 
        printf("\n\033[31mWelcome to MPX. Please enter the number associated with the following menu options:\
        \n1)  Help\
        \n2)  Version\
        \n3)  Get Time\
        \n4)  Set Time\
        \n5)  Get Date\
        \n6)  Set Date\
        \n7)  Initialize Heap\
        \n8)  Allocate Memory\
        \n9)  Free Memory\
        \n10) Show Free Memory\
        \n11) Show Allocated Memory\
        \n12) Show All Memory\
        \n13) Create Alarm\
        \n14) PCB Commands\
        \n15) Shutdown\
        \nEnter Prompt Below:\
        \n>\033[0m",sizeof(char[495]));

        //buffer to read/store input from terminal 
        char buf[100] = { 0 };
        int nread = sys_req(READ, COM1, buf, sizeof(buf));
        printf("\n\n", sizeof(char[3]));
        
        //Yield CPU after reading user input
        CPU_yield();

        //format response
        printf("\n\nYour Response: ",sizeof(char[18]));

        //output response to terminal 
        sys_req(WRITE, COM1, buf, nread);

        //input validation
        //must validate that input falls in range of 1-12
        if (atoi(buf) >= 1 && atoi(buf) <= 15) {

            //process the command
            if (strcmp(&buf[0], "15") == 0) {
                int response = shutdown_command();
           
                if (response == 1) {
                    //Remove all processes from every queue
                    while (ready_Queue.front != NULL) {
                        dequeue(&ready_Queue);
                    }

                    while (blocked_Queue.front != NULL) {
                        dequeue(&blocked_Queue);
                    }

                    while (ready_Suspended.front != NULL) {
                        dequeue(&ready_Suspended);
                    }

                    while (blocked_Suspended.front != NULL) {
                        dequeue(&blocked_Suspended);
                    }
                    
                    //Terminate the command handler process
                    sys_req(EXIT);
                }
                
            } else if (strcmp(&buf[0], "1") == 0) {
            help_command();
            cont = 1;

            } else if (strcmp(&buf[0], "2") == 0) {
            version_command();
            cont = 1;

            } else if (strcmp(&buf[0], "3") == 0) {
            gett_command();
            cont = 1;

            } else if (strcmp(&buf[0], "4") == 0) {
            sett_command();
            cont = 1;

            } else if (strcmp(&buf[0], "5") == 0) {
            getd_command();
            cont = 1;

            } else if (strcmp(&buf[0], "6") == 0) {
            setd_command();
            cont = 1;

            } else if (strcmp(&buf[0], "7") == 0) {
            init_Heap();
            cont = 1;

            } else if (strcmp(&buf[0], "8") == 0) {
            allocate();
            cont = 1;

            } else if (strcmp(&buf[0], "9") == 0) {
            free();
            cont = 1;

            } else if (strcmp(&buf[0], "10") == 0) {
            show_mem(1);
            cont = 1;

            } else if (strcmp(&buf[0], "11") == 0) {
            show_mem(2);
            cont = 1;

            } else if (strcmp(&buf[0], "12") == 0) {
            
            show_mem(0);

            cont = 1;

            } else if (strcmp(&buf[0], "13") == 0) {
            create_alarm();

            } else if (strcmp(&buf[0], "14") == 0) {
            pcbMenu();

            }

        } else {
            
            //display error message for invalid input and loop enters next iteration
            printf("\nInvalid Input. Menu will be reloaded.\n", sizeof(char[40]));

            // 
            continue;
        }
    } 
}

/**
 * @name pcbMenu
 * @brief Displays another menu with options for all the pcb commands 
 * @return N/A
*/
void pcbMenu(void){

   
   int cont = 0;
    for (;;) {

        // If reentering the menu after a function call
        // Prompt user to enter key to continue
        if (cont == 1) {

            printf("\n\nPress any key to continue\n\n", sizeof(char[30]));

            char buf[2] = {0};
            sys_req(READ, COM1, buf, sizeof(buf));

            //Yield CPU after reading user input
            CPU_yield();
        }

        cont = 0;

       // CPU_yield();

        /* menu format */
        printf("\nPCB Functions Menu:\
        \n1)  Help\
        \n2)  Load Process\
        \n3)  Load Suspended Process\
        \n4)  Delete PCB\
        \n5)  Suspend PCB\
        \n6)  Resume PCB\
        \n7)  Set PCB Priority\
        \n8) Show PCB\
        \n9) Show Ready\
        \n10) Show Blocked\
        \n11) Show All\
        \n12) Return to previous menu\
        \nEnter Prompt Below:\
        \n>", sizeof(char[359]));

        //buffer to read/store input from terminal 
        char buf[100] = { 0 };
        int nread = sys_req(READ, COM1, buf, sizeof(buf));
        printf("\n\n>:", sizeof(char[5]));
        
        //Yield CPU after reading user input
        CPU_yield();

        //format response
        printf("\n\nYour Response: ", sizeof(char[18]));

        //output response to terminal 
        sys_req(WRITE, COM1, buf, nread);

        //menu formatting
        printf("\n", sizeof(char[2]));

        // Input validation
        // Must validate that input falls in range of 1-13
        // Run the corresponding function that is assigned to the selection
        if (atoi(buf) >= 1 && atoi(buf) <= 14) {

            //process the command
            if (strcmp(&buf[0], "12") == 0) {

                return; 
                
            } else if (strcmp(&buf[0], "1") == 0) {
                pcbMenu_help_command();
                cont = 1;

            } else if (strcmp(&buf[0], "2") == 0) {
                load_process();
                cont = 1;

            } else if (strcmp(&buf[0], "3") == 0) {
                load_suspended_process();
                cont = 1;

            } else if (strcmp(&buf[0], "4") == 0) {
                deletePCB();
                cont = 1;

            } else if (strcmp(&buf[0], "5") == 0) {
                suspendPCB();
                cont = 1;

            } else if (strcmp(&buf[0], "6") == 0) {
                resumePCB();
                cont = 1;

            } else if (strcmp(&buf[0], "7") == 0) {
                setPcbPriority();
                cont = 1;
                

            } else if (strcmp(&buf[0], "8") == 0) {
                pcbShow(0);
                cont = 1;

            } else if (strcmp(&buf[0], "9") == 0) {
                pcbShow(1);
                cont = 1;

            } else if (strcmp(&buf[0], "10") == 0) {
                pcbShow(2);
                cont = 1;

            } else if (strcmp(&buf[0], "11") == 0) {
                //checks to see if the ready queues contain pcbs
                if((getProcess(1)->front != NULL) || (getProcess(3)->front != NULL)){
                    pcbShow(1);
                }
                
                //checks to see if the blocked queues contain pcbs
                if((getProcess(2)->front != NULL) || (getProcess(4)->front != NULL)){
                    pcbShow(2);
                }

                //displays message to user if all queues are empty 
                if((getProcess(1)->front == NULL) && (getProcess(2)->front == NULL) && (getProcess(3)->front == NULL) && (getProcess(4)->front == NULL)){

                    printf("\nAll Queues Are Empty\n",sizeof(char[23]));
                
                }


                
                cont = 1;

        } else {

            //display error message for invalid input and loop enters next iteration
            char error[] = "\nInvalid Input. Menu will be reloaded.\n";
            sys_req(WRITE, COM1, error, sizeof(error));
            continue;
        }
    }  
}
}

/**
 * @name titleCard
 * @brief Prints the teams logo to the terminal
 * @return N/A
*/
void titleCard( void ){

    // String of ascii art and print statement to print the art
    printf("\n ________  ___  ___  ________  ________             ___          ________  ________      \n|\\   __  \\|\\  \\|\\  \\|\\   __  \\|\\   ___ \\           |\\  \\        |\\   __  \\|\\   ____\\  \n\\ \\  \\|\\  \\ \\  \\\\\\  \\ \\  \\|\\  \\ \\  \\_|\\ \\          \\ \\  \\       \\ \\  \\|\\  \\ \\  \\___|_    \n \\ \\  \\\\\\  \\ \\  \\\\\\  \\ \\   __  \\ \\  \\ \\\\ \\       __ \\ \\  \\       \\ \\  \\\\\\  \\ \\_____  \\   \n  \\ \\  \\\\\\  \\ \\  \\\\\\  \\ \\  \\ \\  \\ \\  \\_\\\\ \\     |\\  \\\\_\\  \\       \\ \\  \\\\\\  \\|____|\\  \\  \n   \\ \\_____  \\ \\_______\\ \\__\\ \\__\\ \\_______\\    \\ \\________\\       \\ \\_______\\____\\_\\  \\ \n    \\|___| \\__\\|_______|\\|__|\\|__|\\|_______|     \\|________|        \\|_______|\\_________\\ \n          \\|__|                                                              \\|_________|\n",sizeof(char[720]));

}

/**
 * @name itoa
 * @brief Converts integer values to ascii
 * @param value Integer to convert to ascii
 * @param strArray Array to store the converted value in
 * @return Converted integer value in ascii
*/
char* itoa(int value, char* strArray){

    // Instantiate variables
    int i = 0;
    int isNegetive = 0;

    // Check if value to convert is negetive
    if (value < 0){
        isNegetive = 1;
        value = -value;
    }

    // Special case if the value is 0
    if (value == 0){
        strArray[0] = '0';
        strArray[1] = '0';
        return strArray;
    }

    // Loop to put digits into array in reverse order
    while (value != 0){
        strArray[i++] = value % 10 + '0';
        value /= 10;
    }

    // Add a "-" if the value is negetive
    if (isNegetive == 1){
        strArray[i++] = '-';
    }

    // Add a null terminator to the array
    strArray[i] = '\0';

    // Instantiate head and tail variables
    int head = 0;
    int tail = i - 1;

    // Reverse the string
    while (head < tail) {
        char temp = strArray[head];
        strArray[head] = strArray[tail];
        strArray[tail] = temp;
        head++;
        tail--;
    }

    // Return the array
    return strArray;

}

/**
 * @name version_command
 * @brief Prints the current version of the MPX system and the compliation date to the terminal
 * @return N/A
*/
void version_command( void ) {
    
    printf("\nVersion 5.0\
    \nCompilation Date: April 10, 2024\n", sizeof(char[51]));

    return;
}

/**
 * @name help_command
 * @brief Prints the available commands menu with a brief description of each function to the terminal
 * @return N/A
*/
void help_command(void) {
    printf(
        "\nAvailable Commands:"\
        "\n 1) Help - Displays all available commands."\
        "\n 2) Version - Displays the version of OS."\
        "\n 3) Get Time - Allows users to get the current time."\
        "\n 4) Set Time - Allows users to set a time of their choice."\
        "\n 5) Get Date - Allows users to get the date."\
        "\n 6) Set Date - Allows users to set a date of their choice."\
        "\n 7) Initalize Heap - Allows users to initalize the heap to a size of their choice."\
        "\n 8) Allocate Memory - Allows users to manually allocate a block of memory."\
        "\n 9) Free Memory - Allows users to manually free a block of memory."\
        "\n 10) Show Free Memory - Displays all the free blocks of memory in the system."\
        "\n 11) Show Allocated Memory - Displays all the allocated blocks of memory in the system."\
        "\n 11) Show All Memory - Displays all blocks of memory in the system."\
        "\n 13) Create Alarm - Allows users to set an alarm for a message to display."\
        "\n 14) PCB Commands - Displays all the available PCB functions."\
        "\n 15) Shutdown - Lets the user shut down MPX.",sizeof(char[964]));

}//end help_command

/**
 * @name shutdown_command
 * @brief Prompts the user to confirm if they want to shutdown the system. If confirmed, the system will be shutdown. Otherwise, user will be returned to main menu.
 * @return Integer to determine whether or not the system should be shutdown
*/
int shutdown_command( void ){

    printf("\n\nAre you sure you want to shutdown?"\
                "\n Enter 1 for yes or 0 for no."\
                "\n > ",sizeof(char[71]));

    //reads input
    char response[100] = {0};
    sys_req(READ, COM1, response, sizeof(response));
    
    //Yield CPU after reading user input
    CPU_yield();
    

    //if yes, starts shutdown procedure
    if(strcmp(response, "1") == 0){
        return 1;
    }//end if 
    //if no, returns back to menu
    else if (strcmp(response, "0") == 0){
        return 0;
    
    } else {
        printf("Invalid input",sizeof(char[14])); 
    }//end else

return 0;
}//end shutdown_command


/**
 * @name gett_command
 * @brief Function to print the currently set time to the terminal
 * @return N/A
*/
void gett_command( void ){

    // Instantiate a struct and store the returned values of the getTime function
    timeStruct currTime = getTime();

    // Print a statement to start time output
    printf("\nCurrent Time: ", sizeof(char[16]));

    // Print hour
    char hour[5];
    itoa(currTime.hour, hour);
    sys_req(WRITE, COM1, hour, 2);

    // Print a colon divider
    char colon[] = ":";
    sys_req(WRITE, COM1, colon, sizeof(colon));

    // Print minute
    char minute[5];
    itoa(currTime.minute, minute);
    sys_req(WRITE, COM1, minute, 2);

    // Print a colon divider
    sys_req(WRITE, COM1, colon, sizeof(colon));

    // Print second
    char second[5];
    itoa(currTime.second, second);
    sys_req(WRITE, COM1, second, 2);

    // Add a line after print
    char space[] = "\n";
    sys_req(WRITE, COM1, space, sizeof(space));
}

/**
 * @name getd_command
 * @brief Prints the currently set date from the RTC to the terminal
 * @return N/A
*/
void getd_command( void ){

    // Create a struct and store the results of getDate
    dateStruct currDate = getDate();

    // Print date statement
    printf("\nCurrent Date: ",sizeof(char[16]));

    // Print month
    char month[4];
    itoa(currDate.month, month);
    sys_req(WRITE, COM1, month, 2);

    // Print divider
    char slash[] = "/";
    sys_req(WRITE, COM1, slash, sizeof(slash));

    // Print day
    char day[4];
    itoa(currDate.day, day);
    sys_req(WRITE, COM1, day, 2);

    // Print divider
    sys_req(WRITE, COM1, slash, sizeof(slash));

    // Print year
    char year[3];
    itoa(currDate.year, year);
    sys_req(WRITE, COM1, year, 2);

    // Add a line after print
    char space[] = "\n";
    sys_req(WRITE, COM1, space, sizeof(space));

}

/**
 * @name sett_command
 * @brief Prompts user to enter values for hour, minute, and second to set the RTC registers
 * @return N/A
*/
void sett_command( void ){

    // Instantiate values
    int hours = -1;
    int minutes = -1;
    int seconds = -1;

    // Invalid input statement
    char invalidInput[] = "\nInput invalid!";

    // Input and validate hour
    while (1){

    //Yield CPU before prompting for and reading user input
     

    // Primpt user to enter the hour
    printf("\nPlease input the hour (HH): ",sizeof(char[31]));

    // Write to the hour buffer
    char hourBuf[5] = { 0 };
    sys_req(READ, COM1, hourBuf, sizeof(hourBuf));
    
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
    sys_req(WRITE, COM1, invalidInput, sizeof(invalidInput));

    }

    // Input and validate minute
    while(1){

    //Yield CPU before prompting for and reading user input
     

    // Prompt user to enter minute
    char prompt2[] = "\n\nPlease input the minute (MM): ";
    sys_req(WRITE, COM1, prompt2, sizeof(prompt2));
    
    // Store entry in minuteBuf
    char minuteBuf[5] = { 0 };
    sys_req(READ, COM1, minuteBuf, sizeof(minuteBuf));

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
    sys_req(WRITE, COM1, invalidInput, sizeof(invalidInput));

    }

    // Input and validate second
    while(1) {

    // Prompt user to input seconds
    char prompt3[] = "\n\nPlease input the second (SS): ";
    sys_req(WRITE, COM1, prompt3, sizeof(prompt3));
    
    // Store entry in secondsBuf
    char secondsBuf[5] = { 0 };
    sys_req(READ, COM1, secondsBuf, sizeof(secondsBuf));

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
    sys_req(WRITE, COM1, invalidInput, sizeof(invalidInput));

    }

    // Set time using entries
    setTime(hours, minutes, seconds);

    // Prompt user that date has been accepted
    char acceptedStatement[] = "\n\nTime has been set!";
    sys_req(WRITE, COM1, acceptedStatement, sizeof(acceptedStatement));

    // Print the written time to the terminal
    gett_command();

}

/**
 * @name setd_command
 * @brief Prompt user to enter value for day, month, and year and set the RTC date registers
 * @return N/A
*/
void setd_command( void ){

    // Variable declaration
    int day = -1;
    int month = -1;
    int year = -1;
    

    // Invalid input statement
    char invalidInput[] = "\nInput invalid!"; 

    // Input and validate month
    while (1){

    //Yield CPU before prompting for and reading user input
     

    // Prompt user to enter month
    char prompt1[] = "\n\nPlease input the month (MM): ";
    sys_req(WRITE, COM1, prompt1, sizeof(prompt1));
    
    // Store entry in monthBuf
    char monthBuf[5] = { 0 };
    sys_req(READ, COM1, monthBuf, sizeof(monthBuf));

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
    sys_req(WRITE, COM1, invalidInput, sizeof(invalidInput));

    }

    // Input and validate day
    while(1){

    //Yield CPU before prompting for and reading user input
     

    // Prompt user to enter day
    char prompt2[] = "\n\nPlease input the day (DD): ";
    sys_req(WRITE, COM1, prompt2, sizeof(prompt2));

    // Store entry in dayBuf
    char dayBuf[5] = { 0 };
    sys_req(READ, COM1, dayBuf, sizeof(dayBuf));

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
    
    // Otherwise, print invalid input statement
    sys_req(WRITE, COM1, invalidInput, sizeof(invalidInput));

    }

    // Input and validate year
    while(1){

    //Yield CPU before prompting for and reading user input
     

    // Prompt user to enter the year
    char prompt3[] = "\n\nPlease input the year (YY): ";
    sys_req(WRITE, COM1, prompt3, sizeof(prompt3));
    
    // Store input in yearBuf
    char yearBuf[5] = { 0 };
    sys_req(READ, COM1, yearBuf, sizeof(yearBuf));

    //Yield CPU before prompting for and reading user input
    CPU_yield();

    // Convert yearBuf value to integer
    year = atoi(yearBuf);

    // Input validation
    if ((year >= 0) && (year <= 99)){
        
        // Valid input
        break;
    }
    
    // Otherwise, print invalid input statement
    sys_req(WRITE, COM1, invalidInput, sizeof(invalidInput));

    }

    // Set the date registers using setDate
    setDate(day, month, year);

    // Prompt user that date has been accepted
    char acceptedStatement[] = "\n\nDate has been set!";
    sys_req(WRITE, COM1, acceptedStatement, sizeof(acceptedStatement));

    // Print the current date to the terminal
    getd_command();

}

/**
 * @name pcbMenu_help_command
 * @brief Prints the available PCB commands menu with a brief description of each function to the terminal
 * @return N/A
*/
void pcbMenu_help_command(void) {
    printf("\nAvailable Commands:"\
        "\n1)  Help - Displays description of each function in the PCB Menu"\
        "\n2)  Load Process - Loads all active ready processess into the active ready queue "\
        "\n3)  Load Suspended Process - Load all ready suspended processes into the ready suspended queue"\
        "\n4)  Delete PCB - Deletes the entered PCB from its queue"\
        "\n5)  Suspend PCB - Sets the entered PCB to the suspended state and inserts in to the corresponding queue"\
        "\n6)  Resume PCB - Sets the entered PCB to the active state and inserts in to the corresponding queue"\
        "\n7)  Set PCB Priority - Sets the Priority level of the entered PCB"\
        "\n8) Show PCB - Displays the information related to the entered PCB"\
        "\n9) Show Ready - Displays all PCBs in the ready queue"\
        "\n10) Show Blocked - Displays all PCBs in the blocked queue"\
        "\n11) Show All - Displays all PCBs in the ready and blocked queues"\
        "\n12) Return to previous menu",sizeof(char[859]));

}//end help_command

/**
 * @name deletePCB
 * @brief Locates a process, deletes it from the queue and frees all associated memory
 * @return N/A
*/
void deletePCB(void){
     
    // Prompt user to enter pcb name
    char pcb_request[] = "\n\nEnter the name of the PCB to delete\n>";
    sys_req(WRITE, COM1, pcb_request, sizeof(pcb_request));

    // Store user entry into process_name
    char process_name[30] = { 0 };
    sys_req(READ, COM1, process_name, sizeof(process_name));

    //Yield CPU before prompting for and reading user input
    CPU_yield();

    // Displays error message if the given pcb name is invalid 
    if (strlen(process_name) > 12) {
        char str_error[] = "\n\nInvalid pcb name.  Cannot exceed 12 characters.";
        sys_req(WRITE, COM1, str_error, sizeof(str_error));
        
        return;
    }

    // Searches for the pcb associated with the given process name 
    pcbStruct* pcb = pcb_find(process_name);

    // Displays error message if the given process is not found 
    if (pcb == NULL) {
        char null_error[] = "\n\nProcess not found.";
        sys_req(WRITE, COM1, null_error, sizeof(null_error));

        return;
    }

    // Displays an error message if the given process is a system process 
    if (pcb->classType == KERNEL) {
        char kern_error[] = "\n\nCannot remove system process.";
        sys_req(WRITE, COM1, kern_error, sizeof(kern_error));

        return;
    }

    // Remove the pcb from the queue 
    int success_code = pcb_remove(pcb);

    // Set next pcb to NULL
    pcb->next = NULL;

    // Display error message if the pcb could not be removed from the queue 
    if (success_code == 1) {
        char error[] = "\n\nError: Failed to remove PCB from queue.";
        sys_req(WRITE, COM1, error, sizeof(error));
        return;
    }

    // Free the memory allocated to the removed pcb 
    int free_code = pcb_free(pcb);

    // Test to see if the memory was correctly freed 
    if (free_code == 1) {
        char error[] = "\n\nError: Failed to remove PCB from memory.";
        sys_req(WRITE, COM1, error, sizeof(error));
        return;
    }


    char success[] = "\n\nPCB successfully deleted.";
    sys_req(WRITE, COM1, success, sizeof(success));

    return;

}

/**
 * @name blockPCB
 * @brief Puts a process in the blocked state and moves it into the blocked queue
 * @return N/A
*/
/**
void blockPCB(void) {

    char procName[30] = {0};

    //Yield CPU before prompting for and reading user input
     

    // Prompt user to enter process name
    char prompt1[] = "\n\nPlease input the process name\n>";
    sys_req(WRITE, COM1, prompt1, sizeof(prompt1));
        
    // Store entry in process name
    char processName[30] = { 0 };
    sys_req(READ, COM1, processName, sizeof(processName));

    //Yield CPU before prompting for and reading user input
    CPU_yield();

    // Check if the PCB already exists
    if (pcb_find(processName) != NULL){
            
        // Copies the memory of processName into procName
        memcpy(procName,processName,sizeof(procName));

    } else {
        // Print error statement
        char prompt2[] = "\n\nProcess name does not exist!";
        sys_req(WRITE, COM1, prompt2, sizeof(prompt2));

        return;
    }

    // Retrive the given pcb 
    pcbStruct* pcb = pcb_find(procName);

    if (pcb->execution_state == BLOCKED) {
        char kern_error[] = "\n\nProcess already blocked.";
        sys_req(WRITE, COM1, kern_error, sizeof(kern_error));

        return;
    }

    // Displays error message if pcb cannot unblock system process
    if (pcb->classType == KERNEL) {
        char kern_error[] = "\n\nCannot block system process.";
        sys_req(WRITE, COM1, kern_error, sizeof(kern_error));

        return;
    }

    // Remove pcb from the previous queue
    int success_code = pcb_remove(pcb);

    // Display error message if the pcb could not be removed from the queue 
    if (success_code == 1) {
        char error[] = "\n\nError: Failed to remove PCB from queue.";
        sys_req(WRITE, COM1, error, sizeof(error));
        return;
    }

    // Set next pcb to NULL
    pcb->next = NULL;

    // Sets pcb execution state to blocked
    pcb->execution_state = BLOCKED;

    // Inserts pcb into its designated queue 
    pcb_insert(pcb);

    char success[] = "\n\nPCB successfully blocked.";
    sys_req(WRITE, COM1, success, sizeof(success));

    return;
}
*/

/**
 * @name unblockPCB
 * @brief Puts a process into the unblocked state and moves it into the unblocked queue
 * @return N/A
*/
/**
void unblockPCB(void) {
    
    char procName[30] = {0};

    //Yield CPU before prompting for and reading user input
     

    // Prompt user to enter process name
    char prompt1[] = "\n\nPlease input the process name\n>";
    sys_req(WRITE, COM1, prompt1, sizeof(prompt1));
        
    // Store entry in process name
    char processName[30] = { 0 };
    sys_req(READ, COM1, processName, sizeof(processName));

    //Yield CPU before prompting for and reading user input
    CPU_yield();

    // Check if the PCB already exists
    if (pcb_find(processName) != NULL){

        // Copies the memory of processName into procName
        memcpy(procName,processName,sizeof(procName));

    } else {
        // Print error statement
        char prompt2[] = "\n\nProcess name does not exist!";
        sys_req(WRITE, COM1, prompt2, sizeof(prompt2));

        return;
    }

    // Retrive the given pcb 
    pcbStruct* pcb = pcb_find(procName);

    if (pcb->execution_state == READY) {
        char kern_error[] = "\n\nProcess already ready.";
        sys_req(WRITE, COM1, kern_error, sizeof(kern_error));

        return;
    }

    // Remove pcb from the previous queue
    int success_code = pcb_remove(pcb);

    // Display error message if the pcb could not be removed from the queue 
    if (success_code == 1) {
        char error[] = "\n\nError: Failed to remove PCB from queue.";
        sys_req(WRITE, COM1, error, sizeof(error));
        return;
    }

    // Set next pcb to NULL
    pcb->next = NULL;

    // Sets pcb execution state to ready
    pcb->execution_state = READY;

    // Inserts pcb into its designated queue 
    pcb_insert(pcb);

    char success[] = "\n\nPCB successfully unblocked.";
    sys_req(WRITE, COM1, success, sizeof(success));

    return;
}
*/

/**
 * @name suspendPCB
 * @brief Puts the process in the suspended state and moves it to the correct queue
 * @return N/A
*/
void suspendPCB(void) {
    
    char procName[30] ={0};

    //Yield CPU before prompting for and reading user input
     

    // Prompt user to enter process name
    char prompt1[] = "\n\nPlease input the process name\n>";
    sys_req(WRITE, COM1, prompt1, sizeof(prompt1));
        
    // Store entry in process name
    char processName[30] = { 0 };
    sys_req(READ, COM1, processName, sizeof(processName));

    //Yield CPU before prompting for and reading user input
    CPU_yield();

    // Check if the PCB already exists
    if (pcb_find(processName) != NULL){

        // Copies the memory of processName into procName
        memcpy(procName,processName,sizeof(procName));

    } else {
        // Print error statement
        char prompt2[] = "\n\nProcess name does not exist!";
        sys_req(WRITE, COM1, prompt2, sizeof(prompt2));

        return;
    }

    // Retrive the given pcb 
    pcbStruct* pcb = pcb_find(procName);

    if (pcb->suspendStatus == SUSPENDED) {
        char kern_error[] = "\n\nProcess already suspended.";
        sys_req(WRITE, COM1, kern_error, sizeof(kern_error));

        return;
    }

    // Displays error message if pcb cannot unblock system process
    if (pcb->classType == KERNEL) {
        char kern_error[] = "\n\nCannot suspend system process.";
        sys_req(WRITE, COM1, kern_error, sizeof(kern_error));

        return;
    }

    // Remove pcb from the previous queue
    int success_code = pcb_remove(pcb);

    // Display error message if the pcb could not be removed from the queue 
    if (success_code == 1) {
        char error[] = "\n\nError: Failed to remove PCB from queue.";
        sys_req(WRITE, COM1, error, sizeof(error));
        return;
    }

    // Set next pcb to NULL
    pcb->next = NULL;

    // Sets pcb execution state to blocked
    pcb->suspendStatus = SUSPENDED;

    // Inserts pcb into its designated queue 
    pcb_insert(pcb);

    char success[] = "\n\nPCB successfully suspended.";
    sys_req(WRITE, COM1, success, sizeof(success));

    return;
}

/**
 * @name resumePCB
 * @brief Puts the process in the non suspended state and moves it to the correct queue
 * @return N/A
*/
void resumePCB(void) {
    
    char procName[30] = {0};

    //Yield CPU before prompting for and reading user input
     

    // Prompt user to enter process name
    char prompt1[] = "\n\nPlease input the process name\n>";
    sys_req(WRITE, COM1, prompt1, sizeof(prompt1));
        
    // Store entry in process name
    char processName[30] = { 0 };
    sys_req(READ, COM1, processName, sizeof(processName));

    //Yield CPU before prompting for and reading user input
    CPU_yield();

    // Check if the PCB already exists
    if (pcb_find(processName) != NULL){

        // Copies the memory of processName into procName
        memcpy(procName,processName,sizeof(procName));

    } else {
        // Print error statement
        char prompt2[] = "\n\nProcess name does not exist!";
        sys_req(WRITE, COM1, prompt2, sizeof(prompt2));

        return;
    }

    // Retrive the given pcb 
    pcbStruct* pcb = pcb_find(procName);

    if (pcb->suspendStatus == ACTIVE) {
        char kern_error[] = "\n\nProcess already active.";
        sys_req(WRITE, COM1, kern_error, sizeof(kern_error));

        return;
    }

    // Remove pcb from the previous queue
    int success_code = pcb_remove(pcb);

    // Display error message if the pcb could not be removed from the queue 
    if (success_code == 1) {
        char error[] = "\n\nError: Failed to remove PCB from queue.";
        sys_req(WRITE, COM1, error, sizeof(error));
        return;
    }

    // Set next pcb to NULL
    pcb->next = NULL;

    // Sets pcb execution state to blocked
    pcb->suspendStatus = ACTIVE;

    // Inserts pcb into its designated queue 
    pcb_insert(pcb);

    char success[] = "\n\nPCB successfully resumed.";
    sys_req(WRITE, COM1, success, sizeof(success));

    return;
}

/**
 * @name setPcbPriority
 * @brief Changes the priority level of a process and moves it to the correct queue
 * @return N/A
*/
void setPcbPriority(void){

    char procName[30] = {0};

    //Yield CPU before prompting for and reading user input
     

    // Prompt user to enter process name
    char prompt1[] = "\n\nPlease input the process name\n>";
    sys_req(WRITE, COM1, prompt1, sizeof(prompt1));
        
    // Store entry in process name
    char processName[30] = { 0 };
    sys_req(READ, COM1, processName, sizeof(processName));

    //Yield CPU before prompting for and reading user input
    CPU_yield();

    // Check if the PCB already exists
    if (pcb_find(processName) != NULL){

        // Copies the memory of processName into procName
        memcpy(procName,processName,sizeof(procName));

    } else {
        // Print error statement
        char prompt2[] = "\n\nProcess name does not exist!";
        sys_req(WRITE, COM1, prompt2, sizeof(prompt2));

        return;
    }

    // Store the given pcb
    pcbStruct* pcb = pcb_find(procName);

    char new_priority[15] = { 0 };

    while (1) {
        // Prompt user to enter a new priority 
        char pcb_error_request[] = "\n\nEnter the new priority\n>";
        sys_req(WRITE, COM1, pcb_error_request, sizeof(pcb_error_request));

        // Store user entry into new_priority
        sys_req(READ, COM1, new_priority, sizeof(new_priority));

        //Yield CPU before prompting for and reading user input
        CPU_yield();

        // Checks to see if user input is between 0-9 
        if(atoi(new_priority) < 0 || atoi(new_priority) < 9) {
            if (atoi(new_priority) == 0) {
                if (pcb->classType == USER) {
                    // Display error if a user tries to set a user process to 0 
                    char pcb_error_request[] = "\n\nCannot set user level function to priority 0.";
                    sys_req(WRITE, COM1, pcb_error_request, sizeof(pcb_error_request));

                    continue;
                }

                break;
            }

            break; 
        } else {
            // Display error if priority is not in the range of 0-9
            char pcb_error_request1[] = "\n\nPriority must be between 0 and 9.";
            sys_req(WRITE, COM1, pcb_error_request1, sizeof(pcb_error_request1));
        }

    }

    

    // Remove pcb from the previous queue
    int success_code = pcb_remove(pcb);

    // Display error message if the pcb could not be removed from the queue 
    if (success_code == 1) {
        char error[] = "\n\nError: Failed to remove PCB from queue.";
        sys_req(WRITE, COM1, error, sizeof(error));
        return;
    }

    // Set next pcb to NULL
    pcb->next = NULL;

    // Change pcb priority 
    pcb->priority = atoi(new_priority);

    // Reinsert pcb in its appropriate queue in its new position based off priority 
    pcb_insert(pcb);

    char prompt3[] = "\n\nNew priority set to: ";
    sys_req(WRITE, COM1, prompt3, sizeof(prompt3));

    sys_req(WRITE, COM1, new_priority, sizeof(new_priority));

    return;
}

/**
 * @name pcbShow
 * @brief Displays the appropriate process control block based on user's input.
 * @param int type Terminates the appropriate command. type 0: show specific pcb, type 1: show ready pcb, type 2: show blocked pcb, type 3: show all pcb.
 * @return N/A
*/
void pcbShow(int type){
    
    //show specific pcb
    if(type == 0){
        
        //prompts for name of pcb
        printf("\n Please enter the name of the PCB to show.\n>",sizeof(char[46]));

        //reads user response for the name of pcb
        char response[30] = {0};
        sys_req(READ, COM1, response, sizeof(response));

        //Yield CPU before prompting for and reading user input
        CPU_yield();

        //finds specific pcb 
        pcbStruct* found = pcb_find(response); 

        // Check if PCB exists
        if (found == NULL){
            printf("\n\nPCB Not Found", sizeof(char[16]));
            return;
        }

        printf("\n ---------------- ",sizeof(char[20]));

        //Displays specific pcb's atrributes

        //display pcb's name
        printf("\n\n Name: ", sizeof(char[10]));

        printf(found->name, sizeof(found->name));

        //displays pcb's class
        printf("\n\n Class:",sizeof(char[10]));
        
        //checks for kernel level or user level pcb
        if (found->classType == 0){
        printf(" KERNEL", sizeof(char[8]));
        }

        if (found->classType == 1){
        printf(" USER", sizeof(char[6]));
        }

        //displays pcb's execution state
        printf("\n\n Execution State:", sizeof(char[20]));
        
        //checks for ready state or running state or blocked state
        if (found->execution_state == 1){
        printf(" READY", sizeof(char[7]));
        }

        if (found->execution_state == 2){
        printf(" RUNNING", sizeof(char[9]));
        }

        if (found->execution_state == 3){
        printf(" BLOCKED", sizeof(char[9]));
        }

        //displays pcb's status
        printf("\n\n Status:", sizeof(char[11]));
        
        //checks for suspended or active pcb status
        if (found->suspendStatus == 1){
        printf(" SUSPENDED", sizeof(char[11]));
        }

        if (found->suspendStatus == 2){
        printf(" ACTIVE", sizeof(char[8]));
        }

        //converts priority to an appropriate ascii value
        char priority[2];
        itoa(found->priority,priority);

        //displays pcb's priority
        printf("\n\n Priority: ", sizeof(char[14]));
        printf(priority, 1);

        printf("\n ---------------- \n", sizeof(char[21]));        
    }//end if for type 0

    //show ready pcb
    if(type == 1){
        
        //checks for existing ready and ready_suspended processes
        if((getProcess(1)->front == NULL) && (getProcess(3)->front == NULL)){
            printf("\nNo PCBs in ready state.", sizeof(char[25]));
            return;
        }

        //Get the head node of the ready queue
        pcbStruct* currPCB = getProcess(1)->front;

        if (currPCB != NULL){
            printf("\nREADY QUEUE", sizeof(char[13]));
        }

        while(currPCB != NULL){

            //promts for name of pcb
            printf("\n ---------------- ", sizeof(char[20]));

            //diplays pcb's name
            printf("\n\n Name: ", sizeof(char[10]));
            printf(currPCB->name, sizeof(currPCB->name));

            //displays pcb's class type
            printf("\n\n Class:", sizeof(char[10]));
            
            //checks for kernel level or user level pcb
            if (currPCB->classType == 0){
                printf(" KERNEL", sizeof(char[8]));
            }

            if (currPCB->classType == 1){
                printf(" USER", sizeof(char[6]));
            }

            //displays pcb's execution state
            printf("\n\n Execution State:", sizeof(char[20]));
            
            //checks for ready state or running state or blocked state
            if (currPCB->execution_state == 1){
                printf(" READY", sizeof(char[7]));
            }

            if (currPCB->execution_state == 2){
                printf(" RUNNING", sizeof(char[9]));
            }

            if (currPCB->execution_state == 3){
                printf(" BLOCKED", sizeof(char[9]));
            }

            //displays pcb's status
            printf("\n\n Status:", sizeof(char[11]));
            
            //checks for either suspended or active pcb status
            if (currPCB->suspendStatus == 1){
                printf(" SUSPENDED", sizeof(char[11]));
            }

            if (currPCB->suspendStatus == 2){
                printf(" ACTIVE", sizeof(char[8]));
            }

            //converts priority to an appropriate ascii value
            char priority[2];
            itoa(currPCB->priority,priority);

            //displays pcb's priority
            printf("\n\n Priority: ", sizeof(char[14]));
            printf(priority, 1);

            printf("\n ---------------- \n", sizeof(char[21]));

            currPCB = currPCB->next;

        }//end while

    
        //ready_suspended pcb
        currPCB = getProcess(3)->front;

        if (currPCB != NULL){
            printf("\nREADY SUSPENDED QUEUE", sizeof(char[23]));
        }

        while(currPCB != NULL){

            //promts for name of pcb
            printf("\n ---------------- ", sizeof(char[20]));

            //displays ready_suspended pcb's name
            printf("\n\n Name: ", sizeof(char[10]));
            printf(currPCB->name, sizeof(currPCB->name));

            //displays ready_suspended pcb's kernel class
            printf("\n\n Class:", sizeof(char[10]));
            
            //checks for ready_suspended pcb's level
            if (currPCB->classType == 0){
                printf(" KERNEL", sizeof(char[8]));
            }

            //break
            if (currPCB->classType == 1){
                printf(" USER", sizeof(char[6]));
            }

            //displays ready_suspended pcb's execution state
            printf("\n Execution State:", sizeof(char[19]));
            
            //checks if execution state is ready or running or blocked
            if (currPCB->execution_state == 1){
                printf(" READY", sizeof(char[7]));
            }

            if (currPCB->execution_state == 2){
                printf(" RUNNING", sizeof(char[9]));
            }

            if (currPCB->execution_state == 3){
                printf(" BLOCKED", sizeof(char[9]));
            }

            //displays ready_suspended pcb's status
            printf("\n Status:", sizeof(char[10]));
            
            //checks if status is suspended or active
            if (currPCB->suspendStatus == 1){
                printf(" SUSPENDED",sizeof(char[11]));
            }

            if (currPCB->suspendStatus == 2){
                printf(" ACTIVE", sizeof(char[8]));
            }

            //converts priority to an appropriate ascii value
            char priority[2];
            itoa(currPCB->priority,priority);

            //displays ready_suspended pcb's priority
            printf("\n Priority: ", sizeof(char[13]));

            printf(priority,1);

            printf("\n ---------------- \n",sizeof(char[21]));

            currPCB = currPCB->next;

        }//end while
        

    }//end ready outter if

    //show blocked pcb
    if(type == 2){
        
        //checks for existing blocked and blocked_suspended processes
        if((getProcess(2)->front == NULL) && (getProcess(4)->front == NULL)){
            printf("\nNo PCBs in blocked state.",sizeof(char[27]));
            return;
        }
        
        pcbStruct* currPCB = getProcess(2)->front;

        if (currPCB != NULL){
            printf("\nBLOCKED QUEUE", sizeof(char[15]));
        }

        while(currPCB != NULL){

            //promts for name of pcb
            printf("\n ---------------- ",sizeof(char[20]));

            //displays blocked_suspended pcb's name
            printf("\n Name: ", sizeof(char[9]));

            printf(currPCB->name, 12);

            //displays blocked pcb's class
            printf("\n Class:", sizeof(char[9]));
            
            //checks if blocked pcb is kernel level or user level
            if (currPCB->classType == 0){
                printf(" KERNEL", sizeof(char[8]));
            }

            if (currPCB->classType == 1){
                printf(" USER", sizeof(char[6]));
            }

            //displays blocked pcb's execution state
            printf("\n Execution State:", sizeof(char[19]));
            
            //checks if execution state is ready or running or blocked
            if (currPCB->execution_state == 1){
                printf(" READY", sizeof(char[7]));
            }

            if (currPCB->execution_state == 2){
                printf(" RUNNING", sizeof(char[9]));
            }

            if (currPCB->execution_state == 3){
                printf(" BLOCKED", sizeof(char[9]));
            }

            //displays blocked pcb's status
            printf("\n Status:", sizeof(char[10]));
            
            //checks if pcb status is suspended or active 
            if (currPCB->suspendStatus == 1){
                printf(" SUSPENDED", sizeof(char[11]));
            }

            if (currPCB->suspendStatus == 2){
                printf(" ACTIVE", sizeof(char[8]));
            }

            //converts priority to an appropriate ascii value
            char priority[2];
            itoa(currPCB->priority,priority);

            //displays blocked pcb's priority
            printf("\n Priority: ",sizeof(char[13]));

            printf(priority, 1);

            //promts for name of pcb
            printf("\n ---------------- \n",sizeof(char[21]));

            currPCB = currPCB->next;

        }//end while
        
        ////blocked_suspended pcb
        currPCB = getProcess(4)->front;

        if (currPCB != NULL){
            printf("\nBLOCKED SUSPENDED QUEUE", sizeof(char[25]));
        }

        while(currPCB != NULL){

            //promts for name of pcb
            printf("\n ---------------- ",sizeof(char[20]));

            //displays blocked_suspended pcb's name
            printf("\n Name: ", sizeof(char[9]));

            printf(currPCB->name,12);

            //displays blocked_suspended pcb's class
            printf("\n Class:", sizeof(char[9]));
            
            //checks for blocked_suspended level
            if (currPCB->classType == 0){
                printf(" KERNEL", sizeof(char[8]));
            }

            if (currPCB->classType == 1){
                printf(" USER", sizeof(char[6]));
            }

            //displays blocked_suspended pcb's state
            printf("\n Execution State:",sizeof(char[19]));
            
            //checks if execution state is ready or running or blocked
            if (currPCB->execution_state == 1){
                    printf(" READY", sizeof(char[7]));
                }

                if (currPCB->execution_state == 2){
                    printf(" RUNNING", sizeof(char[9]));
                }

                if (currPCB->execution_state == 3){
                    printf(" BLOCKED",sizeof(char[9]));
                }

            //displays blocked_suspended pcb's status
            printf("\n Status:",sizeof(char[10]));
            
            //checks if status is suspended or active
            if (currPCB->suspendStatus == 1){
                printf(" SUSPENDED",sizeof(char[11]));
            }

            if (currPCB->suspendStatus == 2){
                printf(" ACTIVE",sizeof(char[8]));
            }

            //converts priority to an appropriate ascii value
            char priority[2];
            itoa(currPCB->priority,priority);

            //displays blocked_suspended pcb's priority
            printf("\n Priority: ",sizeof(char[13]));
            printf(priority,1);

            //promts for name of pcb
            printf("\n ---------------- \n",sizeof(char[21]));

            currPCB = currPCB->next;

        }//end while
        
    }
}

/**
 * @name allocate
 * @brief Allocates heap memory and prints the address of the newly allocated block
*/
void allocate( void ) {

    //assuming address is of hex type
    printf("\nEnter the size of memory to allocate: ", sizeof(char[40]));
    
    char size[30] = {0};
    sys_req(READ, COM1, size, sizeof(size));
    CPU_yield();

    //Validate user input
    if (atoi(size) <= 0) {
        printf("\nInvalid input size. Size must a positve number larger than 0.\n", sizeof(char[64]));
        return;
    }

    //Allocate based off the given size 
    void* address = allocate_memory(atoi(size));

    char address_str[9];
    itoh((int)address, address_str);

    //Check to see if memory was properly allocated
    if (address == NULL) {
        printf("\nError: Memory Allocation Failure\n", sizeof(char[35]));
    } else {
        printf("\nSuccessfully allocated memory at address: ", sizeof(char[44]));
        printf(address_str, sizeof(address_str));
    }

    return;
}

/**
 * @name free
 * @brief Frees heap memory 
*/
void free( void ){

    //assuming address is of hex type
    printf("\nEnter the memory address to free in hexadecimal format: ", sizeof(char[58]));
    
    char hexAdd[30] = {0};
    sys_req(READ, COM1, hexAdd, sizeof(hexAdd));
    CPU_yield();

    // convert from hexadecimal to integer
    int add_int = htoi(hexAdd);

    //converts the hexadecimal address to appropriate type to be used in free_memory()
    void *address = (void*) &add_int;

    //unsucessful free
    if(free_memory(address) == -1){
        printf("\nMemory could not be freed.", sizeof(char[29]));
    } else {
        printf("\nMemory sucessfully freed.", sizeof(char[28]));

    }

    return;

}//end free

/**
 * @name show_mem
 * @brief shows allocated/free memory details as requested by user
 * @param func 2 for allocated memory and 1 for free memory. Follows memIndex
 */
void show_mem(int func){

    //show allocated memory details
    if(func == 2){

        //front node
        mcbStruct *curMCB = memory_list.front;

        printf("\nAllocated Memory Blocks: \n", sizeof(char[28]));
        
        if (curMCB == NULL) {
            printf("Empty\n", sizeof(char[7]));
        }

        while (curMCB != NULL) {
            
            if (curMCB->type == ALLOCATE){

                char address_str[9] = {0};
                itoh((int)curMCB->start, address_str);

                char size_str[8] = {0};
                itoa((int)curMCB->size, size_str);

                //print allocated memory details
                printf("\nStart Address: ", sizeof(char[17]));
                printf(address_str,sizeof(char[9]));
                printf("\nSize: ",sizeof(char[8]));
                printf(size_str, sizeof(char[8])); // need to convert from int to character
            }

            curMCB = curMCB->next;

        }//end while loop

    } //end if for allocated memory

    //show free memory details
    if(func == 1){

        //front node
        mcbStruct *curMCB = memory_list.front;

        printf("\nFree Memory Blocks: \n", sizeof(char[22]));

        if (curMCB == NULL) {
            printf("Empty\n", sizeof(char[7]));
        }

        while (curMCB != NULL) {
        
            if (curMCB->type == FREE){
                
                char address_str[9] = {0};
                itoh((int)curMCB->start, address_str);

                char size_str[8] = {0};
                itoa((int)curMCB->size, size_str);

                //print allocated memory details
                printf("\nStart Address: ", sizeof(char[17]));
                printf(address_str,sizeof(char[9]));
                printf("\nSize: ",sizeof(char[8]));
                printf(size_str, sizeof(char[8])); // need to convert from int to character
            }

            curMCB = curMCB->next;

        }//end while

    }//end if for free memory

    //show all memory details
    if(func == 0){

        //front node
        mcbStruct *curMCB = memory_list.front;

        printf("\nMemory Blocks: \n", sizeof(char[18]));


        if (curMCB == NULL) {
            printf("Empty\n", sizeof(char[7]));
        }

        while (curMCB != NULL) {
                
            char address_str[9] = {0};
            itoh((int)curMCB->start, address_str);

            char size_str[8] = {0};
            itoa((int)curMCB->size, size_str);

            //print allocated memory details
            printf("\nStart Address: ", sizeof(char[17]));
            printf(address_str,sizeof(char[9]));
            printf("\nSize: ",sizeof(char[8]));
            printf(size_str, sizeof(char[8])); // need to convert from int to character

            if (curMCB->type == FREE) {
                printf("\nType: FREE\n\n", sizeof(char[12]));;
            } else {
                printf("\nType: ALLOCATED\n\n", sizeof(char[17]));;

            }


            curMCB = curMCB->next;

        }//end while

    }//end if for free memory

    

    return;

}//end show_mem

/**
 * @name init_Heap
 * @brief User function to initalize the heap to the input size
 */
void init_Heap( void ){

    // Prompt user for input
    printf("\nHow many bytes would you like to allocate?\n>",sizeof(char[46]));

    // Read in input
    char heapBuff[20] = { 0 };
    sys_req(READ, COM1, heapBuff, sizeof(heapBuff));
    //Yield CPU after reading user input
    CPU_yield();

    //Error checking
    if (atoi(heapBuff) > 10000 || atoi(heapBuff) <= 0) {
        printf("\nFailed to initalize. Initialization request does not fall in range of 1-10000.", sizeof(char[80]));
        return;
    }

    // Initialize Heap
    initialize_heap(atoi(heapBuff));

    printf("\nMemory successfully initialized.", sizeof(char[34]));

    return;

}

